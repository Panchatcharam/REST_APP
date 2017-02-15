#include <time.h>
#include <dirent.h>
#include <algorithm>
#include <string>
#include "RestApp.h"
#include "RestAppWorker.h"
#include "ThreadRunner.h"

typedef std::shared_ptr<ThreadRunner> ThreadRunnerRef;

RestApp::RestApp(std::string endPoint) try : DeviceMap{}
				   	   	   	   	   	   	   , priorityQ{}
										   , rest(RestAPIFacade::GetInstance(endPoint))
										   , vecTestFile{}
										   , currJsonIdx(0)
{
	//std::cout<< " RestApp::RestApp()" << std::endl;
}

catch(std::exception & e)
{
	std::cout<<"\n Exception - "<<e.what()<<std::endl;
	exit(1);
}

RestApp::RestApp() try : DeviceMap{}
	   	   	   	   	   , priorityQ{}
					   , rest(RestAPIFacade::GetInstance("192.168.243.129/api/"))
					   , vecTestFile{}
					   , currJsonIdx(0)
{

}

catch(std::exception & e)
{
	std::cout<<"\n Exception - "<<e.what()<<std::endl;
	exit(1);
}

RestApp::~RestApp()
{
	//std::cout<< " RestApp::~RestApp()" << std::endl;
}

void RestApp::Initialize()
{
	GetFileList(vecTestFile);

	if (vecTestFile.empty())
	{
		std::cout << "No Input files found!!!" << std::endl;
		exit(1);
	}

	std::sort(vecTestFile.begin(), vecTestFile.end());

	std::cout<<std::endl;
	std::cout<< " Test Files" << std::endl;
	std::cout<< " ----------" << std::endl;
	for (auto & file : vecTestFile)
	{
		std::cout<< " " << file << std::endl;
	}

	std::cout<<std::endl;
	std::cout<<std::endl;

	Json::Value Device;
	Json::Reader reader;
	std::ifstream jsonFile = {};
	for (size_t idx = 0; idx < vecTestFile.size(); ++idx)
	{
		jsonFile.open(vecTestFile[idx], std::ifstream::in);
		if ( !reader.parse(jsonFile, Device, false) )
		{
			std::cout << "Failed Parsing file " << vecTestFile[idx] << std::endl;
			exit(1);
		}
		priorityQ.push(Device.size());
		DeviceMap.insert(std::make_pair(idx, Device));
		jsonFile.close();
	}
}

void RestApp::Run()
{
	unsigned int option = 0;

	do
	{
		std::cout<<"==================================================="<<std::endl;
		std::cout<<"| 1. Send data from all devices once               |"<<std::endl;
		std::cout<<"| 2. Send Data continuously for a specific period  |"<<std::endl;
		std::cout<<"| 3. Quit                                          |"<<std::endl;
		std::cout<<"==================================================="<<std::endl;
		std::cout<<"Choose your option: ";
		std::cin >> option;

		HandleUserOption(option);

	}while(option != 3);
}

void RestApp::HandleUserOption(const unsigned int & option)
{
	switch(option)
	{
	case 1:
		HandleSendDataOnce();
		break;
	case 2:
		{
			unsigned long window = 0, slice = 0;
			std::cout<<std::endl;
			std::cout<<std::endl;
			std::cout<<"Enter period (in minutes) : ";
			std::cin >> window;
			std::cout<<"Enter data transfer interval (in seconds) : ";
			std::cin >> slice;
			std::cout<<std::endl;
			std::cout<<std::endl;
			HandleSendDataforSpecificWindow(window, slice);
		}
		break;
	default:
		break;
	}
}

void RestApp::GetFileList(std::vector<std::string> & fileList)
{
	DIR *dpdf = nullptr;
	struct dirent *epdf;
	std::string dirLoc("");

	std::cout<<std::endl;
	std::cout<<std::endl;

	std::cout << " Test File Location : ";
	std::cin >> dirLoc;
	dpdf = opendir(dirLoc.c_str());

	if (dpdf != NULL)
	{
	    while ((epdf = readdir(dpdf)) != nullptr)
	    {
	       //std::cout << epdf->d_name << std::endl;
	       if ( std::string(epdf->d_name).find(".json") != std::string::npos )
	       {
	    	   fileList.emplace_back(epdf->d_name);
	       }
	    }
	}
}

void RestApp::HandleSendDataOnce()
{
	std::unique_ptr<RestAppWorker> worker(new RestAppWorker(rest));

	std::cout << " currJsonIdx : " << currJsonIdx << std::endl;

	for(size_t devId = 0; devId < vecTestFile.size(); ++devId)
	{
		if (currJsonIdx <= DeviceMap[devId].size())
		{
			worker->HandleSendDataOnce(DeviceMap[devId][currJsonIdx]);
		}
	}

	// Determine should roll over thr json array index.
	currJsonIdx = (++currJsonIdx > priorityQ.top()) ? 0 : currJsonIdx;
}

void RestApp::HandleSendDataforSpecificWindow(unsigned long window, unsigned long slice)
{
	std::string date("");
	time_t unixEpoch = {0};
	int timeOffset = 0;

	std::cout<<std::endl;

	do
	{
		std::cout<<"Enter Start Date in yyyy/mm/dd format (e.g. 2017/02/25) : ";
		//std::getline(std::cin, date);
		std::cin >> date;
		GetUnixEpoch(date, unixEpoch);
	}while(unixEpoch == 0);

	std::cout<<std::endl;
	std::cout<<"Enter offset in minutes : ";
	std::cin >> timeOffset;
	std::cout<<std::endl;
	std::cout<<std::endl;

	//std::cout << "unixEpoch : "<< unixEpoch << std::endl;

	std::vector<std::unique_ptr<RestAppWorker>> vecWorker = {};

	for(size_t idx = 0; idx < vecTestFile.size(); ++idx)
	{
		vecWorker.emplace_back(new RestAppWorker(DeviceMap[idx],rest,window,slice));
	}

	std::vector<ThreadRunnerRef> runners = {};
	uint32_t counter = 0;

	for(std::unique_ptr<RestAppWorker> & worker : vecWorker)
	{
		runners.emplace_back(std::make_shared<ThreadRunner>(1,\
				std::bind(&RestAppWorker::HandlePostDevice,worker.get(),counter++,unixEpoch,timeOffset)));
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	for(ThreadRunnerRef & runner : runners)
	{
		runner->Join();
	}
}

void RestApp::GetUnixEpoch(std::string & date, time_t & epoch)
{
	time_t rawtime = 0;
	struct tm * timeinfo = {0};
	std::vector<std::string> token;
	char sep = '/';

	for (size_t p = 0, q = 0; p != date.npos; p = q)
	{
		token.push_back(date.substr(p + (p != 0), (q = date.find(sep, p + 1)) - p - (p != 0)));
	}

	if (token.size() == 3)
	{
		int year(std::stoi(token[0])), month(std::stoi(token[1])), day(std::stoi(token[2]));
		std::cout<<"\n Year : "<<year << " Month : "<< month <<" date: " <<day<<std::endl;
		if (ValidateDate(month, day,year))
		{
			time(&rawtime);
			timeinfo = localtime(&rawtime);
			timeinfo->tm_year = year - 1900;
			timeinfo->tm_mon = month - 1;
			timeinfo->tm_mday = day;
			timeinfo->tm_hour = 0;
			timeinfo->tm_min = 0;
			timeinfo->tm_sec = 0;
			epoch = mktime(timeinfo);
		}
	}
	else
	{
		std::cout << "Invalid date format, please enter in yyyy/mm/dd format (e.g. 2017/02/25)" << std::endl;
	}
}

bool RestApp::ValidateDate(int month, int day, int year)
{
	if (!(1 <= month && month <= 12))
		return false;
	if (!(1 <= day && day <= 31))
		return false;
	if ((day == 31) && (month == 2 || month == 4 || month == 6 || month == 9 || month == 11))
		return false;
	if ((day == 30) && (month == 2))
		return false;
	if ((month == 2) && (day == 29) && (year % 4 != 0))
		return false;
	if ((month == 2) && (day == 29) && (year % 400 == 0))
		return true;
	if ((month == 2) && (day == 29) && (year % 100 == 0))
		return false;
	if ((month == 2) && (day == 29) && (year % 4 == 0))
		return true;

	return true;
}

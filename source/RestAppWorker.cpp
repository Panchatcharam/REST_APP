/*
 * RestAppWorker.cpp
 *
 *  Created on: Feb 2, 2017
 *      Author: developer
 */
#include "RestAppWorker.h"
#include <random>
#include <ctime>

RestAppWorker::RestAppWorker(std::shared_ptr<RestAPIFacade> ptr) : mJsonData("")
															     , rest(ptr)
															     , mWindow(0)
															     , mSlice(0)
{
	//std::cout<< " RestAppWorker::RestAppWorker()" << std::endl;
}

RestAppWorker::	RestAppWorker(Json::Value & json, std::shared_ptr<RestAPIFacade> ptr,
							  unsigned long window, unsigned long slice) : mJsonData(json)
																		 , rest(ptr)
																		 , mWindow(window)
																		 , mSlice(slice)
{
	//std::cout<< " RestAppWorker::RestAppWorker()" << std::endl;
}

RestAppWorker::~RestAppWorker()
{
	//std::cout<< " RestAppWorker::~RestAppWorker()" << std::endl;
}

void RestAppWorker::HandleSendDataOnce(Json::Value & rawJson)
{
	std::string jsonData("");
	Json::Value::UInt64 TimeStamp = GetMilliSeconds();

	// Update time stamp
	rawJson["date"] = Json::Value(TimeStamp);

	for(Json::Value & timeStamp : rawJson["data"]["process_data"])
	{
		timeStamp["dv_TS"] = Json::Value(TimeStamp);
	}
	rawJson["data"]["diagnostics"]["diag_TS"] = Json::Value(TimeStamp);

	jsonData = rawJson.toStyledString();

	rest->PostPollingData("v2/general/private/post",jsonData);
}


void RestAppWorker::HandlePostDevice(uint32_t count, const time_t & unixEpoch, const int & offset)
{
	std::cout <<"Thread " << (count+1) <<" Created" << std::endl;
	std::string jsonData("");
	Json::Value::UInt64 timeStamp = 0;
	unsigned long long currentOff(offset);
	unsigned int milliSecs = 1;
	const time_t start(time(0));

    std::random_device rdIdx;
    std::mt19937 mtIdx(rdIdx());
    std::uniform_int_distribution<int> distIdx(0, (mJsonData.size() - 1));

    std::random_device rdData;
    std::mt19937 mtData(rdData());
    std::uniform_real_distribution<double> distData(22.0, 35.0);

	//time_t test = 0;
	for (;;)
	{
		//for(Json::Value rawJson : mJsonData)
		{
			//int index = distIdx(mtIdx);
			Json::Value rawJson = mJsonData[distIdx(mtIdx)];
			timeStamp = static_cast<Json::Value::UInt64>(( unixEpoch + (currentOff * 60 )));//GetMilliSeconds();
			timeStamp += milliSecs;
			currentOff += offset;

//			test = timeStamp;
//			char *dt = ctime(&test);
//			std::cout << "Curr date time : "<< dt << std::endl;
//			std::cout << "currentOff     : "<< currentOff << std::endl;
//			std::cout << "unixEpoch      : "<< unixEpoch << std::endl;
//			std::cout << "calc timeStamp : "<< timeStamp << std::endl;
//			std::cout << "orig timeStamp : "<< rawJson["date"] << std::endl;

			// Update time stamp
			rawJson["date"] = Json::Value(timeStamp);
			//double d = 0.0;
			for(Json::Value & tData : rawJson["data"]["process_data"])
			{
				if ( tData["dv_label"] == "PV" )
				{
					//d = distData(mtData);
					tData["dv_value"] = distData(mtData);
				}
				tData["dv_TS"] = Json::Value(timeStamp);
			}

			//std::cout<<" Thread "<< count+1<< "index is "<<index << " Maximum size : "<<(mJsonData.size() - 1) << " Value : "<<d<<std::endl;

			rawJson["data"]["diagnostics"]["diag_TS"] = Json::Value(timeStamp);

			jsonData = rawJson.toStyledString();

			// Send data to App Server
			rest->PostPollingData("v2/general/private/post",jsonData);

			for(unsigned long idx = 0; idx < mSlice; ++idx)
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));

				milliSecs = (++milliSecs > 999) ? 1 : milliSecs;

				if ( static_cast<unsigned long>(( time(0) - start )) >= (mWindow * 60))
				{
					//std::cout<<"\n time(0) : "<<time(0)<<" start : "<<start<<std::endl;
					std::cout << "Thread "<< (count+1) << " Timed Out" << std::endl;
					return;
				}
			}
		}
	}
}

int RestAppWorker::GetRandomIndex()
{
	return 0;
}

Json::Value::UInt64 RestAppWorker::GetMilliSeconds()
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::chrono::system_clock::duration tp = now.time_since_epoch();
	return (std::chrono::duration_cast<std::chrono::milliseconds>(tp).count());
}

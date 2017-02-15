#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <fstream>
#include <map>
#include <queue>
#include <vector>
#include <mutex>
#include <ratio>
#include "json/json-forwards.h"
#include "json/json.h"
#include "RestAPIFacade.h"
#include "CommonDefs.h"

class RestApp
{
public:
	RestApp();
	RestApp(std::string endPoint);
	~RestApp();

	void Initialize();
	void Run();

private:

	void HandleSendDataforSpecificWindow(unsigned long window, unsigned long slice);
	void HandleSendDataOnce();
	void HandleUserOption(const unsigned int & option);
	void GetFileList(std::vector<std::string> & fileList);
	void GetUnixEpoch(std::string & date, time_t & epoch);
	bool ValidateDate(int month, int day, int year);

	std::map<int, Json::Value> DeviceMap;
	std::priority_queue<Json::Value::ArrayIndex> priorityQ;
	std::shared_ptr<RestAPIFacade> rest;
	std::vector<std::string> vecTestFile;
	Json::Value::ArrayIndex currJsonIdx;
};

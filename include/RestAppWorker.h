#pragma once
#include <chrono>
#include <time.h>
#include <thread>
#include <ratio>
#include "json/json-forwards.h"
#include "json/json.h"
#include "RestAPIFacade.h"
#include "CommonDefs.h"

class RestAppWorker
{
public:
	RestAppWorker() = delete;
	RestAppWorker(std::shared_ptr<RestAPIFacade> ptr);
	RestAppWorker(Json::Value & json, std::shared_ptr<RestAPIFacade> ptr,
					unsigned long window, unsigned long slice);
	~RestAppWorker();

	void HandlePostDevice(uint32_t count, const time_t & unixEpoch, const int & offset);
	void HandleSendDataOnce(Json::Value & rawJson);
	int GetRandomIndex();

private:
	//void PostData(Json::Value & rawJson, const Json::Value::UInt64 & timeStamp);
	Json::Value::UInt64 GetMilliSeconds();

	Json::Value mJsonData;
	std::shared_ptr<RestAPIFacade> rest;
	unsigned long mWindow;
	unsigned long mSlice;
};

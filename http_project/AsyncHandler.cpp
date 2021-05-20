#include "AsyncHandler.h"
#include "boost/lexical_cast.hpp"
#include "boost/property_tree/xml_parser.hpp"
#include "Config.h"

bool AsyncHandler::Init()
{
	m_url = gConfig.GetPushDomain();
	m_title = gConfig.GetPushTitle();
	m_content = gConfig.GetPushContent();
	return true;
}

bool AsyncHandler::Final()
{
	return true;
}

void AsyncHandler::Start()
{
	if (!m_thread)
	{
		m_thread.reset(new boost::thread(boost::bind(&boost::asio::io_service::run, &m_io)));
	}
}

void AsyncHandler::Stop()
{
	m_io.stop();
}

void AsyncHandler::Join()
{
	m_thread->join();
}

void AsyncHandler::PushDeskMsgHandle(Lint serverId, Lint userId)
{
	m_io.post(boost::bind(&AsyncHandler::AsyncPushDeskMsgHandler, this, serverId, userId));
}

void AsyncHandler::GongZhongHaoPush(std::map<Lstring, Lstring>& users)
{
	m_io.post(boost::bind(&AsyncHandler::AsyncGongZhongHaoPush, this, users));
}

void AsyncHandler::AsyncPushDeskMsgHandler(Lint appId, Lint userId)
{
	Json::Value arrRootVal;
	arrRootVal["playerIds"].append(userId);
	arrRootVal["title"] = m_title;
	arrRootVal["content"] = m_content;

	std::string result;
	try
	{
		if (!HttpRequest::Instance().HttpPost(m_url + boost::lexical_cast<std::string>(appId) + "/push", arrRootVal.toStyledString(), result, true))
		{
			//LLOG_ERROR("AsyncHandler http request error");LLOG_ERROR("%s::http request error %s %d/push %s %s", __FUNCTION__, m_url.c_str(), appId, arrRootVal.toStyledString().c_str(), result.c_str());
			LLOG_ERROR("AsyncHandler http request error");
			return;
		}
	}
	catch (...)
	{
		//LLOG_ERROR("%s::http request error %s %d/push %s", __FUNCTION__, m_url.c_str(), appId, arrRootVal.toStyledString().c_str());
		LLOG_ERROR("AsyncHandler http request error");
		return;
	}
	LLOG_DEBUG("%s %s%d/push %s %s", __FUNCTION__, m_url.c_str(), appId, arrRootVal.toStyledString().c_str(), result.c_str());
}

void AsyncHandler::AsyncGongZhongHaoPush(std::map<Lstring, Lstring> users)
{
	if (users.empty())
		return;

	Json::Value content;
	std::map<Lstring, Lstring>::iterator it = users.begin();
	for (; it != users.end(); it++)
	{
		Json::Value user;
		user["unionId"] = it->first;
		user["replace1"] = it->second;
		content["users"].append(user);
	}

	//在此块中增加推送消息体
	{
		content["data"].append(Json::Value(""));
	}

	Lstring result;
	try
	{
		if (!HttpRequest::Instance().HttpPost(m_url + "20001" + "/push", content.toStyledString(), result, true))
		{
			LLOG_ERROR("%s::http request error %s %s %s", __FUNCTION__, m_url.c_str(), content.toStyledString().c_str(), result.c_str());
		}
	}
	catch (...)
	{
		LLOG_ERROR("%s::http request error %s %s %s", __FUNCTION__, m_url.c_str(), content.toStyledString().c_str(), result.c_str());
	}
	LLOG_DEBUG("%s %s %s %s", __FUNCTION__, m_url.c_str(), content.toStyledString().c_str(), result.c_str());
}

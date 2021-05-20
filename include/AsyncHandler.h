#ifndef C0283E53_34CB_4F70_B066_C7889CBF7A69
#define C0283E53_34CB_4F70_B066_C7889CBF7A69

#include "boost/thread.hpp"
#include "boost/asio.hpp"
#include "LBase.h"
#include "LSingleton.h"
#include "HttpRequest.h"
#include "LLog.h"


class AsyncHandler : public LSingleton<AsyncHandler>
{
public:
	AsyncHandler() : m_ioWork(m_io){}

	virtual bool Init();
	virtual bool Final();

	void Start();

	void Stop();

	void Join();

	void PushDeskMsgHandle(Lint serverId, Lint userId);

	void GongZhongHaoPush(std::map<Lstring, Lstring>& users);

private:
	void AsyncPushDeskMsgHandler(Lint appId, Lint userId);

	void AsyncGongZhongHaoPush(std::map<Lstring, Lstring> users);

private:
	boost::asio::io_service				m_io;
	boost::asio::io_service::work		m_ioWork;
	boost::shared_ptr<boost::thread>	m_thread;

	std::string							m_url;
	std::string							m_title;
	std::string							m_content;
};
#define gAsyncHandler AsyncHandler::Instance()

#endif
#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include "curl/curl.h"
#include <iostream>
using namespace std;
class HttpRequest
{
public:
	virtual bool	Init();
	virtual bool	Final();

	//get«Î«Û,÷ß≥÷https
	bool			HttpGet(const std::string& url, std::string& result,bool isHttps=false);


	bool getUrl(const std::string& httpurl, std::string& result, bool isHttps);

};



#endif
#include "alk_http_request.h"

//#include "HttpConnect.h"
#include "HttpRequest.h"

#ifdef WIN32
#pragma comment(lib,"ws2_32.lib")
#endif


int SendHttpRequest(string http_addr/*, string file_path, string device_id*/)
{
	//cout << http_addr << endl;
	//cout << file_path << endl;
https://github.com/huqinwei/nomeaning2/blob/master/order.txt


	//send(sockClient, strHttpGet, strlen(strHttpGet), 0); 
	HttpRequest http_request;
	string ret;
	http_request.getUrl(http_addr,ret,false);




// 	HttpConnect *http = new HttpConnect();
// 	http->getData("127.0.0.1", "/login", "id=liukang&pw=123");
// 	http->postData("127.0.0.1", "/login", "id=liukang&pw=123");

	return 0;
}

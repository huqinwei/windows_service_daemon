#include "HttpRequest.h"

bool HttpRequest::Init()
{
    CURLcode return_code;
    return_code = curl_global_init(CURL_GLOBAL_WIN32);
    if (CURLE_OK != return_code)
    {
        cerr << "init libcurl failed." << endl;
        return -1;
    }
    return true;
}

bool HttpRequest::Final()
{
    curl_global_cleanup();
    return true;
}


//GET请求方式
//("127.0.0.1", "/login", "id=liukang&pw=123")
// std::stringstream stream;
// stream << "GET " << path << "?" << get_content;
// stream << " HTTP/1.0\r\n";
// stream << "Host: " << host << "\r\n";
// stream << "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9.2.3) Gecko/20100401 Firefox/3.6.3\r\n";
// stream << "Connection:close\r\n\r\n";
// socketHttp(host, stream.str());
//http://10.10.62.11:8780/api/geo/publish?filePath=E:/publish/map/map_15.tif

bool HttpRequest::getUrl(const std::string& httpurl, std::string& result, bool isHttps)
{
    CURL *curl = curl_easy_init();
    if (curl == NULL)
    {
        cerr << "http get curl init fail.." << endl;
        return false;
    }
    curl_easy_setopt(curl, CURLOPT_URL, httpurl.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2);			 //设置超时
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);        //屏蔽其它信号
    if (isHttps)
    {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    }
    char recvBuff[1024 * 64];
    memset(recvBuff, 0, sizeof(recvBuff));
    //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, httpWriteCallBack);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, recvBuff);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK)
    {
        if (res == 3)
        {

        }
        else
        {
            cerr << "http request fail:" << httpurl.c_str() << "," << res << endl;
        }
        return false;
    }
    else
    {
        recvBuff[sizeof(recvBuff) - 1] = 0;
        result = recvBuff;
        return true;
    }
}

// 
// 
// bool HttpRequest::getUrl2(char *url)
// {
// 	// 初始化libcurl
// 	CURLcode return_code;
// 	return_code = curl_global_init(CURL_GLOBAL_WIN32);
// 	if (CURLE_OK != return_code)
// 	{
// 		cerr << "init libcurl failed." << endl;
// 		return -1;
// 	}
// 
// 	CURL *curl;
// 	CURLcode res;
// 
// 	struct curl_slist *headers = NULL;
// 	headers = curl_slist_append(headers, "Accept: Agent-007");
// 	curl = curl_easy_init();    // 初始化
// 	if (curl)
// 	{
// 		//curl_easy_setopt(curl, CURLOPT_PROXY, "10.99.60.201:8080");// 代理
// 		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);// 改协议头
// 		curl_easy_setopt(curl, CURLOPT_URL, url);
// 		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
// // 		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp); //将返回的http头输出到fp指向的文件
// // 		curl_easy_setopt(curl, CURLOPT_HEADERDATA, fp); //将返回的html主体数据输出到fp指向的文件
// 		res = curl_easy_perform(curl);   // 执行
// 		if (res != 0) {
// 
// 			curl_slist_free_all(headers);
// 			curl_easy_cleanup(curl);
// 		}
// 		curl_global_cleanup();
// 		return true;
// 	}
// 	curl_global_cleanup();
// }
// 
// 


#pragma once
#include<iostream>
#include<windows.h>
#include<string>
#include<queue>
//#include<WinSock2.h>在windows里边
using namespace std;

#pragma comment(lib,"ws2_32.lib")//网络的库

//https://blog.csdn.net/a_biyudao/article/details/92840552


class CHttp
{
private:
    string m_host;
    string m_object;
    SOCKET m_socket;
    bool AnalyseUrl(string url);//解析URL\http
    bool AnalyseUrl2(string url);//\https
    bool init();//初始化套接字
    bool Connect();//连接web服务器
public:
    CHttp(void);
    ~CHttp(void);
    string FetchGet(string url);//通过Get方式获取网页
    void AnalyseHtml(string html);//解析网页，获得图片地址和其他的链接
};

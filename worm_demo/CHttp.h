#pragma once
#include<iostream>
#include<windows.h>
#include<string>
#include<queue>
//#include<WinSock2.h>��windows���
using namespace std;

#pragma comment(lib,"ws2_32.lib")//����Ŀ�

//https://blog.csdn.net/a_biyudao/article/details/92840552


class CHttp
{
private:
    string m_host;
    string m_object;
    SOCKET m_socket;
    bool AnalyseUrl(string url);//����URL\http
    bool AnalyseUrl2(string url);//\https
    bool init();//��ʼ���׽���
    bool Connect();//����web������
public:
    CHttp(void);
    ~CHttp(void);
    string FetchGet(string url);//ͨ��Get��ʽ��ȡ��ҳ
    void AnalyseHtml(string html);//������ҳ�����ͼƬ��ַ������������
};

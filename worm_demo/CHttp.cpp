#include "CHttp.h"

CHttp::CHttp(void)
{

}


CHttp::~CHttp(void)
{
    closesocket(m_socket);
    WSACleanup();
}

//解析URL\http
bool CHttp::AnalyseUrl(string url)
{
    if (string::npos == url.find("http://"))
        return false;
    if (url.length() <= 7)
        return false;
    int pos = url.find('/', 7);
    if (pos == string::npos)
    {
        m_host = url.substr(7);
        m_object = '/';
    }
    else
    {
        m_host = url.substr(7, pos - 7);
        m_object = url.substr(pos);
    }
    if (m_host.empty())
        return false;
    return true;
}

//解析URL\https
bool CHttp::AnalyseUrl2(string url)
{
    if (string::npos == url.find("https://"))
        return false;
    if (url.length() <= 8)
        return false;
    int pos = url.find('/', 8);
    if (pos == string::npos)
    {
        m_host = url.substr(8);
        m_object = '/';
    }
    else
    {
        m_host = url.substr(8, pos - 8);
        m_object = url.substr(pos);
    }
    if (m_host.empty())
        return false;
    return true;
}

bool CHttp::init()
{
    //1 请求协议版本
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (LOBYTE(wsaData.wVersion) != 2 ||
        HIBYTE(wsaData.wVersion) != 2) {
        printf("请求协议版本失败!\n");
        return false;
    }
    //printf("请求协议成功!\n");
    //2 创建socket
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (SOCKET_ERROR == m_socket) {
        printf("创建socket失败!\n");
        WSACleanup();
        return false;
    }
    //printf("创建socket成功!\n");
    return true;
}

//连接web服务器
bool CHttp::Connect()
{
    //DNS服务器：将域名解析成IP地址
    hostent *p = gethostbyname(m_host.c_str());
    if (p == NULL)
        return false;
    SOCKADDR_IN sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(80);//http的默认端口，https的默认端口443
    memcpy(&sa.sin_addr, p->h_addr, 4);

    if (-1 == connect(m_socket, (SOCKADDR*)&sa, sizeof(sa)))
    {
        cout << "服务器连接失败" << endl;
        return false;
    }
    else
    {
        //cout<<"服务器连接成功"<<endl;
        return true;
    }
}

string CHttp::FetchGet(string url)//通过Get方式获取网页
{
    string html;

    //解析url
    if (false == AnalyseUrl(url))
    {
        if (false == AnalyseUrl2(url))
        {
            cout << "Html解析失败" << endl;
            return "";
        }
    }
    //cout<<"主机名"<<m_host<<"\t\t"<<"资源名"<<m_object<<endl;
    if (false == init())//初始化套接字
    {
        return "";
    }
    if (false == Connect())//连接服务器
    {
        return "";
    }
    //发送Get请求  Get请求数据
    string request = "GET " + m_object +
        " HTTP/1.1\r\nHost:" + m_host +
        "\r\nConnection: Close\r\n\r\n";

    if (SOCKET_ERROR == send(m_socket, request.c_str(), request.size(), 0))
    {
        cout << "send request error" << endl;
        closesocket(m_socket);
        return "";
    }

    //接收数据
    char ch;
    while (recv(m_socket, &ch, 1, 0))
    {
        html += ch;
    }

    return html;
}
//判断是否以什么结尾
bool hasEnding(char *& strFull, char*&  strEnd)
{
    char * pFull = strFull;
    while (*pFull != 0)
        pFull++;

    char * pEnd = strEnd;
    while (*pEnd != 0)
        pEnd++;

    while (1)
    {
        pFull--;
        pEnd--;
        if (*pEnd == 0)
        {
            break;
        }

        if (*pFull != *pEnd)
        {
            return false;
        }

    }
    return true;
}
void CHttp::AnalyseHtml(string html)//解析网页，获得图片地址和其他的链接
{
    int startIndex = 0;
    int endIndex = 0;


    for (int pos = 0; pos < html.length();)
    {
        startIndex = html.find("zzxxcc1", startIndex);//查找关键字失败，返回的内容太少，未包含
        if (startIndex == -1)
        {
            break;
        }
        startIndex += 5;
        endIndex = html.find("\"", startIndex);
        //找到资源链接
        string src = html.substr(startIndex, endIndex - startIndex);
        char *src1 = (char *)src.c_str();
        //cout<<src<<endl;
        //判断连接是否是想要的资源
        char *strend = ".jpg";
        if (hasEnding(src1, strend) == true)
        {
            /*if(-1!=src.find("t_s960x600c5"))*/
            if (-1 != src.find("t_s1920x1080c5"))
            {
                cout << src << endl;
                //新建一个线程来下载图片
                extern queue<string> p;
                p.push(src);
                extern void loadImage();
                CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)loadImage,
                    NULL, NULL, NULL);
            }
            /*system("pause");*/
        }
        startIndex = endIndex + 1;
        //system("pause");
    }








    //找到所有的图片
    for (int pos = 0; pos < html.length();)
    {
        startIndex = html.find("src=\"", startIndex);
        if (startIndex == -1)
        {
            break;
        }
        startIndex += 5;
        endIndex = html.find("\"", startIndex);
        //找到资源链接
        string src = html.substr(startIndex, endIndex - startIndex);
        char *src1 = (char *)src.c_str();
        //cout<<src<<endl;
        //判断连接是否是想要的资源
        char *strend = ".jpg";
        if (hasEnding(src1, strend) == true)
        {
            /*if(-1!=src.find("t_s960x600c5"))*/
            if (-1 != src.find("t_s1920x1080c5"))
            {
                cout << src << endl;
                //新建一个线程来下载图片
                extern queue<string> p;
                p.push(src);
                extern void loadImage();
                CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)loadImage,
                    NULL, NULL, NULL);
            }
            /*system("pause");*/
        }
        startIndex = endIndex + 1;
        //system("pause");
    }

    startIndex = 0;
    //找到其他URL地址
    for (int pos = 0; pos < html.length();)
    {
        startIndex = html.find("href=\"", startIndex);
        if (startIndex == -1)
        {
            break;
        }
        startIndex += 6;
        endIndex = html.find("\"", startIndex);
        //找到资源链接
        string src = html.substr(startIndex, endIndex - startIndex);
        char *src1 = (char *)src.c_str();
        //cout<<src<<endl;
        //判断连接是否是想要的资源
        char *strend = ".html";
        if (hasEnding(src1, strend) == true)
        {
            if ((-1 != src.find("bizhi") || -1 != src.find("showpic")) && -1 == src.find("http://"))
            {
                string url = "http://desk.zol.com.cn" + src;
                extern queue<string> q;
                q.push(url);
                //cout<<url<<endl;
            }
        }
        startIndex = endIndex + 1;
        //system("pause");
    }

}

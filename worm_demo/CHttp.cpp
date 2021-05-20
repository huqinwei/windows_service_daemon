#include "CHttp.h"

CHttp::CHttp(void)
{

}


CHttp::~CHttp(void)
{
    closesocket(m_socket);
    WSACleanup();
}

//����URL\http
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

//����URL\https
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
    //1 ����Э��汾
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (LOBYTE(wsaData.wVersion) != 2 ||
        HIBYTE(wsaData.wVersion) != 2) {
        printf("����Э��汾ʧ��!\n");
        return false;
    }
    //printf("����Э��ɹ�!\n");
    //2 ����socket
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (SOCKET_ERROR == m_socket) {
        printf("����socketʧ��!\n");
        WSACleanup();
        return false;
    }
    //printf("����socket�ɹ�!\n");
    return true;
}

//����web������
bool CHttp::Connect()
{
    //DNS��������������������IP��ַ
    hostent *p = gethostbyname(m_host.c_str());
    if (p == NULL)
        return false;
    SOCKADDR_IN sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(80);//http��Ĭ�϶˿ڣ�https��Ĭ�϶˿�443
    memcpy(&sa.sin_addr, p->h_addr, 4);

    if (-1 == connect(m_socket, (SOCKADDR*)&sa, sizeof(sa)))
    {
        cout << "����������ʧ��" << endl;
        return false;
    }
    else
    {
        //cout<<"���������ӳɹ�"<<endl;
        return true;
    }
}

string CHttp::FetchGet(string url)//ͨ��Get��ʽ��ȡ��ҳ
{
    string html;

    //����url
    if (false == AnalyseUrl(url))
    {
        if (false == AnalyseUrl2(url))
        {
            cout << "Html����ʧ��" << endl;
            return "";
        }
    }
    //cout<<"������"<<m_host<<"\t\t"<<"��Դ��"<<m_object<<endl;
    if (false == init())//��ʼ���׽���
    {
        return "";
    }
    if (false == Connect())//���ӷ�����
    {
        return "";
    }
    //����Get����  Get��������
    string request = "GET " + m_object +
        " HTTP/1.1\r\nHost:" + m_host +
        "\r\nConnection: Close\r\n\r\n";

    if (SOCKET_ERROR == send(m_socket, request.c_str(), request.size(), 0))
    {
        cout << "send request error" << endl;
        closesocket(m_socket);
        return "";
    }

    //��������
    char ch;
    while (recv(m_socket, &ch, 1, 0))
    {
        html += ch;
    }

    return html;
}
//�ж��Ƿ���ʲô��β
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
void CHttp::AnalyseHtml(string html)//������ҳ�����ͼƬ��ַ������������
{
    int startIndex = 0;
    int endIndex = 0;


    for (int pos = 0; pos < html.length();)
    {
        startIndex = html.find("zzxxcc1", startIndex);//���ҹؼ���ʧ�ܣ����ص�����̫�٣�δ����
        if (startIndex == -1)
        {
            break;
        }
        startIndex += 5;
        endIndex = html.find("\"", startIndex);
        //�ҵ���Դ����
        string src = html.substr(startIndex, endIndex - startIndex);
        char *src1 = (char *)src.c_str();
        //cout<<src<<endl;
        //�ж������Ƿ�����Ҫ����Դ
        char *strend = ".jpg";
        if (hasEnding(src1, strend) == true)
        {
            /*if(-1!=src.find("t_s960x600c5"))*/
            if (-1 != src.find("t_s1920x1080c5"))
            {
                cout << src << endl;
                //�½�һ���߳�������ͼƬ
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








    //�ҵ����е�ͼƬ
    for (int pos = 0; pos < html.length();)
    {
        startIndex = html.find("src=\"", startIndex);
        if (startIndex == -1)
        {
            break;
        }
        startIndex += 5;
        endIndex = html.find("\"", startIndex);
        //�ҵ���Դ����
        string src = html.substr(startIndex, endIndex - startIndex);
        char *src1 = (char *)src.c_str();
        //cout<<src<<endl;
        //�ж������Ƿ�����Ҫ����Դ
        char *strend = ".jpg";
        if (hasEnding(src1, strend) == true)
        {
            /*if(-1!=src.find("t_s960x600c5"))*/
            if (-1 != src.find("t_s1920x1080c5"))
            {
                cout << src << endl;
                //�½�һ���߳�������ͼƬ
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
    //�ҵ�����URL��ַ
    for (int pos = 0; pos < html.length();)
    {
        startIndex = html.find("href=\"", startIndex);
        if (startIndex == -1)
        {
            break;
        }
        startIndex += 6;
        endIndex = html.find("\"", startIndex);
        //�ҵ���Դ����
        string src = html.substr(startIndex, endIndex - startIndex);
        char *src1 = (char *)src.c_str();
        //cout<<src<<endl;
        //�ж������Ƿ�����Ҫ����Դ
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

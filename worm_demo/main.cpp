#pragma warning(disable:4996)

#include "CHttp.h"
#include <urlmon.h>

#pragma comment(lib, "urlmon.lib")

queue<string> q;//url����
queue<string> p;//ͼƬurl����

void StartCatch(string url);
int main()
{
    //����һ���ļ���,���ʾ��ǰĿ¼
    CreateDirectory("./image", NULL);

    string url;//cin>>url;

    //���ص�ַ��������zip��https://codeload.github.com/huqinwei/nomeaning2/zip/refs/heads/master

    url = "https://blog.csdn.net/huqinweI987/article/details/116894555";// "https://github.com/huqinwei/nomeaning2/blob/master/order.txt";
            //;//"https://blog.csdn.net/electech6/article/details/85331364"; "https://github.com/huqinwei/nomeaning2/blob/master/order.txt";// "http://desk.zol.com.cn/";//�����������վ���������޸�
                                    //��ʼץȡ
    StartCatch(url);

    system("pause");
    return 0;
}

void StartCatch(string url)
{

    q.push(url);

    while (!q.empty())
    {
        //ȡ��url
        string currenturl = q.front();
        q.pop();

        CHttp http;
        //����һ��Get����
        string html = http.FetchGet(currenturl);
        //cout<<html;
        http.AnalyseHtml(html);
    }
}


//����ͼƬ���߳�
static int num = 0;
void loadImage()
{
    while (!p.empty())
    {
        string currenturl = p.front();
        p.pop();
        char Name[20] = { 0 };
        num++;
        sprintf(Name, "./image/%d.jpg", num);

        if (S_OK == URLDownloadToFile(NULL, currenturl.c_str(), Name, 0, 0))
        {
            cout << "download ok" << endl;
            if (num == 24)//��24�žͽ����ˣ�Ҳ����ȥ����仰
            {
                exit(0);
            }
        }
        else
        {
            cout << "download error" << endl;
        }
    }

}

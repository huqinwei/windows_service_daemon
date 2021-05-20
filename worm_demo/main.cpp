#pragma warning(disable:4996)

#include "CHttp.h"
#include <urlmon.h>

#pragma comment(lib, "urlmon.lib")

queue<string> q;//url队列
queue<string> p;//图片url队列

void StartCatch(string url);
int main()
{
    //创建一个文件夹,点表示当前目录
    CreateDirectory("./image", NULL);

    string url;//cin>>url;

    //下载地址，但是是zip：https://codeload.github.com/huqinwei/nomeaning2/zip/refs/heads/master

    url = "https://blog.csdn.net/huqinweI987/article/details/116894555";// "https://github.com/huqinwei/nomeaning2/blob/master/order.txt";
            //;//"https://blog.csdn.net/electech6/article/details/85331364"; "https://github.com/huqinwei/nomeaning2/blob/master/order.txt";// "http://desk.zol.com.cn/";//爬的是这个网站，可自行修改
                                    //开始抓取
    StartCatch(url);

    system("pause");
    return 0;
}

void StartCatch(string url)
{

    q.push(url);

    while (!q.empty())
    {
        //取出url
        string currenturl = q.front();
        q.pop();

        CHttp http;
        //发送一个Get请求
        string html = http.FetchGet(currenturl);
        //cout<<html;
        http.AnalyseHtml(html);
    }
}


//下载图片的线程
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
            if (num == 24)//爬24张就结束了，也可以去掉这句话
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

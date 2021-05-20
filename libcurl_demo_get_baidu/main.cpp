
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <curl\curl.h>
#include <iostream>
#include <stdio.h>

using namespace std;

FILE *fp = NULL;
int UTF8ToGBK(char const * strUTF8);

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    //int written = fwrite(ptr, size, nmemb, (FILE *)fp);
    UTF8ToGBK((char *)ptr);

    //return written;
    return 0;
}

int UTF8ToGBK(char const * strUTF8)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, NULL, 0);
    WCHAR* wszGBK = new WCHAR[len + 1];
    memset(wszGBK, 0, len * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)(LPCTSTR)strUTF8, -1, wszGBK, len);

    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char *szGBK = new char[len + 1];
    memset(szGBK, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
    char *p_temp = szGBK;
    int written = fwrite(p_temp, 1, strlen(p_temp), (FILE *)fp);

    delete[]szGBK;
    delete[]wszGBK;

    return 0;
}
/*
int main(int argc, char *argv[])
{
    //����CURL���͵�ָ��
    CURL *p_curl = NULL;
    //����CURLcode���͵ı��������淵��״̬��
    CURLcode res;

    p_curl = curl_easy_init();
    if (NULL == p_curl)
    {
        printf("curl_easy_init()  func  error\n");
        return -1;
    }

    if (NULL == (fp = fopen("git_libcurl_write", "wb")))
    {
        curl_easy_cleanup(p_curl);
        return -1;
    }

    curl_global_init(CURL_GLOBAL_ALL);

    //����curlѡ��. ����CURLOPT_URL�����û�ָ��url. argv[1]�д�ŵ������д���������ַ
    curl_easy_setopt(p_curl, CURLOPT_URL, "https://github.com/huqinwei/nomeaning2/blob/master/order.txt");//www.baidu.com
    curl_easy_setopt(p_curl, CURLOPT_WRITEFUNCTION, write_data);
    //����curl_easy_perform ִ�����ǵ�����.��������صĲ���. ������ֻ����Ļ����ʾ����.
    res = curl_easy_perform(p_curl);
    //���curl����.
    curl_easy_cleanup(p_curl);
    fclose(fp);

    return 0;
}
*/




static std::string static_cmd_line;
static unsigned static_working_time[24] = { 0 };

const std::string id = "asdfghjkl112";
static size_t downloadCallback(void *buffer, size_t sz, size_t nmemb, void *writer)
{
    string* psResponse = (string*)writer;
    size_t size = sz * nmemb;
    psResponse->append((char*)buffer, size);
    int index = psResponse->find(id);
    std::string comparision = "[";
    if ((psResponse->at(index + 12))=='[')
    {
        for (int i = 0; i < 24; i++) {
            //std::cout << psResponse->at(index + 12+1+i) << std::endl;
            //int flag = atoi(&(psResponse->at(index + 12 + 1 + i)));
            char flag = psResponse->at(index + 12 + 1 + i);
            static_working_time[i] = flag=='1'?1:0;
        }
    }
    int start_index = index + 12 + 1 + 24 + 1 + 1;//startѡȡ����[���
    int word_cnt = psResponse->substr(start_index).find_first_of("]");//�����]�����λ�ã���Ҫ�������
    //end_index = start_index + end_index;//��ƫ����
    std::string cmd_line = psResponse->substr(start_index, word_cnt);
    int at_index = cmd_line.find_first_of("*");//��*����@����ҳ�������⣬���滻����

    std::string cmd_line_sub1 = cmd_line.substr(0, at_index);
    std::string cmd_line_sub2 = cmd_line.substr(at_index+1, cmd_line.size());

    cmd_line = cmd_line_sub1 + "@" + cmd_line_sub2;//�滻�����
    static_cmd_line = cmd_line;
    //*psResponse = cmd_line;

    //std::cout <<"cmd_line is  "<< cmd_line.c_str() << std::endl;
    //std::cout << psResponse->at(start_index) << std::endl;

    return sz * nmemb;
}
int main()
{
    //����԰����  "https://www.cnblogs.com/asdfghjkl111/p/14779109.html"
    string strUrl = "https://blog.csdn.net/huqinweI987/article/details/116894555";// "https://github.com/huqinwei/nomeaning2/order.txt";//"http://www.baidu.com";
    string strTmpStr;
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, downloadCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &strTmpStr);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
/*
    string strRsp;
    if (res != CURLE_OK)
    {
        strRsp = "error";
    }
    else
    {
        strRsp = strTmpStr;
    }

    printf("strRsp is |%s|\n", strRsp.c_str());
    */
    for (int i = 0; i < 24; i++) {
        std::cout << static_working_time[i] << std::endl;
    }
    std::cout << "final:" << static_cmd_line.c_str() << std::endl;

    return 0;
}
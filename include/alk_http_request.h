#pragma once
// #ifdef HTTP_DLL_IMPLEMENT  
// #define HTTP_API __declspec(dllexport)
// #else
// #define HTTP_API __declspec(dllimport)
// #endif
#include <iostream>
#include <string>
using namespace std;

//http://10.10.62.11:8780/api/geo/publish?filePath=E:/publish/map/map_15.tif
//https://github.com/huqinwei/nomeaning2/blob/master/order.txt
/*HTTP_API */int SendHttpRequest(string http_addr = "https://github.com/huqinwei/nomeaning2/blob/master/order.txt");

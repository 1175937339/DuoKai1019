#pragma once

#include <string>
#include <Windows.h>

#include "json.hpp"
#include <time.h>

using json = nlohmann::json;
using namespace std;



#define outputformat(fmt,var) {CHAR sOut[5000];CHAR sfmt[10000];sprintf_s(sfmt,"%s%s","INFO--",fmt);sprintf_s(sOut,(sfmt),var);OutputDebugStringA(sOut);}

//定义一个客户端进入的函数指针函数指针
typedef  void(WINAPI* MyAcceptCallBack)(int clientId);
//定义一个客户端发送消息指针
typedef void(WINAPI* MyReceiveCallBack)(int clientId, char* jsonData, int len);

//定义一个客户端关闭指针
typedef void(WINAPI* MyCloseCallBack)(int clientId);


typedef  bool(WINAPI* InitWeChatSocket)(MyAcceptCallBack acceptCallBack, MyReceiveCallBack receiveCallBack, MyCloseCallBack closeCallBack);

typedef  bool(WINAPI* GetUserWeChatVersion)(OUT LPSTR szVersion);

typedef  int(WINAPI* P_InjectWeChatPid)(IN DWORD dwPid, IN LPCSTR szDllPath);

typedef  int(WINAPI* S_InjectWeChat)(IN LPCSTR szDllPath);

typedef  int(WINAPI* T_InjectWeChat)(IN LPCSTR szDllPath, IN LPCSTR szWeChatExePath);

typedef  bool(WINAPI* UseUtf8)();

typedef  bool(WINAPI* SendWeChatData)(int dwClientId, IN LPCSTR szJsonData);

typedef  int(WINAPI* DestroyWeChat)();

typedef  int(WINAPI* InjectWeChatMultiOpen)(IN LPCSTR szDllPath, IN LPCSTR szWeChatExePath);


extern DWORD loadAddress;
extern bool isInit;


//创建回调函数方法
void __stdcall  SetCB(MyAcceptCallBack acceptCallBack, MyReceiveCallBack receiveCallBack, MyCloseCallBack closeCallBack);



// 得到版本号码
LPSTR __stdcall getWeChatVersion();

// 得到小程序code
LPCWSTR __stdcall getCode(IN int dwClientId, IN LPCSTR requestStr);

// 根据pid进行注入
DWORD __stdcall InjectWeChatPid(IN DWORD dwPid);

DWORD __stdcall InjectWeChat_S();

DWORD __stdcall InjectWeChat(IN LPCSTR szWeChatExePath);

// 在所有接口前执行，执行后接口全部使用utf8编码传输
BOOL __stdcall ContentUseUtf8();

BOOL __stdcall sendHpSocketData(IN int dwClientId, IN LPCSTR szJsonData);

BOOL __stdcall closeWeChat();

// 打开微信
DWORD __stdcall openWechatMutexTwo(IN LPCSTR szWeChatExePath);

//客户端发送消息回调
BOOL __stdcall callReceiveCB(int clientId, char* jsonData, int len);




bool is_str_utf8(const char* str, int len);

std::string string_To_UTF8(const std::string& str);



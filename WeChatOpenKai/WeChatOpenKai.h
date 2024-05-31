#pragma once

#include <string>
#include <Windows.h>

#include "json.hpp"
#include <time.h>

using json = nlohmann::json;
using namespace std;



#define outputformat(fmt,var) {CHAR sOut[5000];CHAR sfmt[10000];sprintf_s(sfmt,"%s%s","INFO--",fmt);sprintf_s(sOut,(sfmt),var);OutputDebugStringA(sOut);}

//����һ���ͻ��˽���ĺ���ָ�뺯��ָ��
typedef  void(WINAPI* MyAcceptCallBack)(int clientId);
//����һ���ͻ��˷�����Ϣָ��
typedef void(WINAPI* MyReceiveCallBack)(int clientId, char* jsonData, int len);

//����һ���ͻ��˹ر�ָ��
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


//�����ص���������
void __stdcall  SetCB(MyAcceptCallBack acceptCallBack, MyReceiveCallBack receiveCallBack, MyCloseCallBack closeCallBack);



// �õ��汾����
LPSTR __stdcall getWeChatVersion();

// �õ�С����code
LPCWSTR __stdcall getCode(IN int dwClientId, IN LPCSTR requestStr);

// ����pid����ע��
DWORD __stdcall InjectWeChatPid(IN DWORD dwPid);

DWORD __stdcall InjectWeChat_S();

DWORD __stdcall InjectWeChat(IN LPCSTR szWeChatExePath);

// �����нӿ�ǰִ�У�ִ�к�ӿ�ȫ��ʹ��utf8���봫��
BOOL __stdcall ContentUseUtf8();

BOOL __stdcall sendHpSocketData(IN int dwClientId, IN LPCSTR szJsonData);

BOOL __stdcall closeWeChat();

// ��΢��
DWORD __stdcall openWechatMutexTwo(IN LPCSTR szWeChatExePath);

//�ͻ��˷�����Ϣ�ص�
BOOL __stdcall callReceiveCB(int clientId, char* jsonData, int len);




bool is_str_utf8(const char* str, int len);

std::string string_To_UTF8(const std::string& str);



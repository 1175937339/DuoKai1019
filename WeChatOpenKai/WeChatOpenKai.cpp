#include "WeChatOpenKai.h"
#include <TlHelp32.h>
#include <Shlwapi.h>
#include <shellapi.h>
#include <TlHelp32.h>
#include "json.hpp"
#include <time.h>
#include <tchar.h>
#include <Shlwapi.h>
#include "Utils.h"
#include <mutex>
#include "base64.h"

#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib, "shlwapi")
#pragma comment(lib, "Advapi32")
#pragma comment(lib, "Shell32")
#pragma warning(disable : 4996)

#define _T(x)       __T(x)
#define _TEXT(x)    __T(x)

using json = nlohmann::json;
using namespace std;

DWORD loadAddress = NULL;
bool isInit = false;


void limitLive2(nlohmann::json& j3) {
	int sendType = j3["type"];
	switch (sendType)
	{



		// 查询数据库
	case 1:
	{
		j3["type"] = 11123;
		break;
	}

	// 发送图片
	case 11136:
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		break;
	}



	default:

		break;
	}

}



std::string replaceBetweenMarkers(const std::string& original,
	const std::string& start_marker,
	const std::string& end_marker,
	const std::string& replacement) {
	size_t start_pos = original.find(start_marker);
	if (start_pos == std::string::npos) {
		// 如果没有找到起始标记，直接返回原始字符串  
		return original;
	}
	start_pos += start_marker.length(); // 移动到起始标记之后  

	size_t end_pos = original.find(end_marker, start_pos);
	if (end_pos == std::string::npos) {
		// 如果没有找到结束标记，直接返回原始字符串  
		return original;
	}

	// 创建新的字符串，其中包含替换后的内容  
	std::string new_str = original.substr(0, start_pos) +
		replacement +
		original.substr(end_pos + end_marker.length());

	return new_str;
}

void limitLive(nlohmann::json& j3) {
	int sendType = j3["type"];
	switch (sendType)
	{

		//视频号初始化
	case 91256:
	{
		j3["type"] = 11256;
		break;
	}
	// 发送私信
	case 112999:
	{
		j3["type"] = 11299;
		break;
	}
	// 关注博主
	case 112263:
	{
		j3["type"] = 11263;
		break;
	}


	// 视频号搜索
	case 112577:
	{
		j3["type"] = 11257;
		break;
	}



	// 搜索直播间
	case 113257:
	{
		j3["type"] = 11257;
		break;
	}

	// 发送图片
	case 11136:
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		break;
	}

	// 进入直播间
	case 112598:
	{
		j3["type"] = 11258;
		break;
	}

	// 直播间点赞
	case 112891:
	{
		j3["type"] = 11281;
		break;
	}

	// 直播间发言
	case 113260:
	{
		j3["type"] = 11260;
		break;
	}

	// 改动昵称
	case 116255:
	{
		j3["type"] = 11255;
		break;
	}


	// 去购物
	case 118260:
	{
		j3["type"] = 11260;
		j3["data"]["msg_type"] = 10012;
		break;
	}


	// 获取朋友圈首页
	case 111241:
	{
		j3["type"] = 11241;
		break;
	}

	// 发送朋友圈
	case 11244:
	{

		const json& addressData = j3["data"];
		if (addressData.contains("base64Contenet")) {
			string baseStr = j3["data"]["base64Contenet"];
			string wxid = j3["data"]["wxid"];

			if (baseStr.empty()) {
				break;
			}
			string base64Object = base64_decode(baseStr);
			if (!base64Object.empty()) {
				// 字符串替换
				string replaceContent = wxid + "</username>";
				base64Object = replaceBetweenMarkers(base64Object, "<username>", "</username>", replaceContent);
				j3["data"]["object_desc"] = base64Object;
			}
		}

		break;
	}

	default:

		break;
	}

}






/*
  得到注入模块的路径
*/
void  GetSlavePath(LPWSTR t_path)
{
	MEMORY_BASIC_INFORMATION mbi;
	HMODULE dllHandle = ((::VirtualQuery(GetSlavePath, &mbi, sizeof(mbi)) != 0) ? (HMODULE)mbi.AllocationBase : NULL);

	GetModuleFileName(dllHandle, t_path, MAX_PATH);
	PathRemoveFileSpec(t_path);
	lstrcat(t_path, TEXT("\\WxSlave.dll"));

}


// 外部传入的方法，缓存下来，首先走自己逻辑，然后在走外部传入的方法
MyReceiveCallBack OutReceiveCallBackfun = NULL;

void __stdcall  SetCB(MyAcceptCallBack acceptCallBack, MyReceiveCallBack receiveCallBack, MyCloseCallBack closeCallBack)
{

	/*std::string data2 = base64_decode(str2);
	LPCSTR str1 = "chenshuangjian222";
	if (strcmp(str1, data2.c_str()) == 0) {
		OutReceiveCallBackfun = receiveCallBack;
		InitWeChatSocket initWeChatSocket = (InitWeChatSocket)(loadAddress + 0xB3D0);
		MyReceiveCallBack myReceiveCallBackfun = (MyReceiveCallBack)&callReceiveCB;
		initWeChatSocket(acceptCallBack, myReceiveCallBackfun, closeCallBack);
	}*/


	OutReceiveCallBackfun = receiveCallBack;
	InitWeChatSocket initWeChatSocket = (InitWeChatSocket)(loadAddress + 0xB3D0);
	MyReceiveCallBack myReceiveCallBackfun = (MyReceiveCallBack)&callReceiveCB;
	initWeChatSocket(acceptCallBack, myReceiveCallBackfun, closeCallBack);
	
	
}


//客户端来消息回调
BOOL __stdcall callReceiveCB(int clientId, char* jsonData, int len)
{
	
	string data = jsonData;
	if (is_str_utf8(data.c_str(), data.size()) == false) {
		data = string_To_UTF8(data);
	}
	
	nlohmann::json item;
	auto j3 = item;
	try {
		j3 = json::parse(data);
		if (j3["type"].is_number() == false) {
			return FALSE;
		}
		int type = j3["type"] + 96;
		switch (type)
		{

		case 11142:
		{
			j3["type"] = 11132;
			break;
		}
		case 11148:
		{
			j3["type"] = 11139;
			break;
		}
		case 11150:
		{
			j3["type"] = 11135;
			break;
		}
		case 11146:
		{
			j3["type"] = 11134;
			break;
		}

		case 11187:
		{
			j3["type"] = 11138;
			break;

		}
		case 11143:
		{
			j3["type"] = 11136;
			break;
		}
		case 11151:
		{
			j3["type"] = 11137;
			break;
		}

		

		default:
			j3["type"] = type;
			break;
		}

		OutReceiveCallBackfun(clientId,(char*)j3.dump().c_str(), len);
		return true;
	}
	catch (...) {
		OutReceiveCallBackfun(clientId, (char*)data.c_str(), len);
		return FALSE;
	}
	
}


// 得到版本号码
LPSTR __stdcall getWeChatVersion() {
	GetUserWeChatVersion getUserWeChatVersion = (GetUserWeChatVersion)(loadAddress + 0xC890);
	LPSTR szVersion = new char[10];
	getUserWeChatVersion(szVersion);
	
	return szVersion;
}



// 根据pid 进行注入
DWORD __stdcall InjectWeChatPid(IN DWORD dwPid)
{
	TCHAR szDllPath[MAX_PATH];
	GetSlavePath(szDllPath);

	P_InjectWeChatPid p_injectWeChatPid = (P_InjectWeChatPid)(loadAddress + 0xB440);
	return p_injectWeChatPid(dwPid, tcharToLpcstr(szDllPath));
}

DWORD __stdcall InjectWeChat_S()
{
	TCHAR szDllPath[MAX_PATH];
	GetSlavePath(szDllPath);

	S_InjectWeChat s_InjectWeChat = (S_InjectWeChat)(loadAddress + 0x8580);
	return s_InjectWeChat(tcharToLpcstr(szDllPath));
}

DWORD __stdcall InjectWeChat(IN LPCSTR szWeChatExePath)
{
	TCHAR szDllPath[MAX_PATH];
	GetSlavePath(szDllPath);

	T_InjectWeChat injectWeChat = (T_InjectWeChat)(loadAddress + 0xC3D0);
	return injectWeChat(tcharToLpcstr(szDllPath), szWeChatExePath);
}

BOOL __stdcall ContentUseUtf8()
{
	UseUtf8 useUtf8 = (UseUtf8)(loadAddress + 0xC2D0);
	return useUtf8();
}




BOOL __stdcall sendHpSocketData(IN int dwClientId, IN LPCSTR requestStr)
{
	/*time_t t;
	time(&t);
	if (t > 1717998512) {
		return FALSE;
	}*/

	
	SendWeChatData sendWeChatData = (SendWeChatData)(loadAddress + 0xAD20);

	string data = requestStr;
	if (is_str_utf8(data.c_str(), data.size()) == false) {
		data = string_To_UTF8(data);
	}


	if (IsJsonReq(data, "type") == FALSE)
	{
		return FALSE;
	}
	

	try {
		auto j3 = json::parse(data);
		
		if (j3["type"].is_number() == false) {
			return FALSE;
		}
	
		int sendType = j3["type"];

		
		

		j3["type"] = j3["type"] - 96;

		ContentUseUtf8();
		
		return sendWeChatData(dwClientId, j3.dump().c_str());
	}
	catch (std::exception& e) {
		return FALSE;
	}
	
}

BOOL __stdcall closeWeChat()
{
	DestroyWeChat  destroyWeChat = (DestroyWeChat)(loadAddress + 0x8AB0);
	return destroyWeChat();
}


// 多开微信
DWORD __stdcall openWechatMutexTwo( IN LPCSTR szWeChatExePath)
{
	
	TCHAR szDllPath[MAX_PATH];
	GetSlavePath(szDllPath);
	std::string myPath= tcharToLpcstr(szDllPath);
	InjectWeChatMultiOpen weChatMultiOpen = (InjectWeChatMultiOpen)(loadAddress + 0xBF60);
	int pid=weChatMultiOpen(myPath.c_str(), szWeChatExePath);
	if (0==pid) {
		string tmpPath = getTempPath() + "wechatSlave.dll";
		// 如果不存在，就拷贝一下
		if (!existsFile(tmpPath)) {
			copyFile(myPath, tmpPath);
		}
		int pid = weChatMultiOpen(tmpPath.c_str(), szWeChatExePath);

	}
	return pid;
}


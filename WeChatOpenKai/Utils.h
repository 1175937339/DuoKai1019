#include "stdafx.h"
#include "json.hpp"
#include <string>
#include <tchar.h>
#include <codecvt>
#include <windows.h>  
#include <TlHelp32.h> 
#include <Psapi.h>
#include <tchar.h>
#include <windows.h>

bool limitHook();



LPCSTR tcharToLpcstr(TCHAR* tcharStr);

bool copyFile(const std::string& sourcePath, const std::string& destinationPath);

void CopyFolder(TCHAR* srcFolder, TCHAR* dstFolder);



std::string LPWSTRToString(const LPWSTR wstr);

// �õ�΢��·��
std::wstring getWeChatPath();

std::string getTempPath();

bool existsFile(const std::string& name);

TCHAR* char2TCAHR(const char* str);

std::string UTF8ToGBK(const char* src_str);


bool is_str_utf8(const char* str, int len);

BOOL IsJsonReq(std::string json, std::string reqData);
std::wstring string2wstring(std::string str);


BOOL DosPathToNtPath(LPTSTR pszDosPath, LPTSTR pszNtPath);

DWORD ProcessNameFindID(PCWSTR ProcessName);

std::string TCHAR2STRING(TCHAR* STR);

// ����ת����ת��
wchar_t* multi_To_Wide_Char(std::string& pKey);

std::string string_To_UTF8(const std::string& str);

wchar_t* multi_Byte_To_Wide_Char(const std::string& pKey);

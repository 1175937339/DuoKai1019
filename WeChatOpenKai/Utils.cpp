#include "Utils.h"
#include <Shlwapi.h>


#include <iostream>  
#include <fstream>  
#include <shellapi.h>
#include <shlobj.h>  

using namespace std;

#pragma comment(lib,"Shlwapi.lib")



LPCSTR tcharToLpcstr(TCHAR* tcharStr)
{
	int bufferSize = WideCharToMultiByte(CP_ACP, 0, tcharStr, -1, NULL, 0, NULL, NULL);
	char* buffer = new char[bufferSize];
	WideCharToMultiByte(CP_ACP, 0, tcharStr, -1, buffer, bufferSize, NULL, NULL);
	return buffer;
}




HKEY hKey;
std::wstring subKey = L"SOFTWARE\\WxTent";

// 写入注册表
bool WriteRegedit(DWORD value) {
	LONG result;
	result = RegCreateKeyEx(HKEY_CURRENT_USER, subKey.c_str(), 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);
	if (result == ERROR_SUCCESS)
	{

		result = RegSetValueEx(hKey, L"WxTent", 0, REG_DWORD, (const BYTE*)&value, sizeof(DWORD));
		if (result == ERROR_SUCCESS)
		{
			return true;
		}
		else
		{
			return false;
		}
		RegCloseKey(hKey);
	}
	else
	{
		return false;
	}
}



LONG ReadRegedit() {

	LONG result;
	// 读取注册表
	result = RegOpenKeyEx(HKEY_CURRENT_USER, subKey.c_str(), 0, KEY_READ, &hKey);
	if (result == ERROR_SUCCESS)
	{
		DWORD value;
		DWORD dataSize = sizeof(DWORD);
		result = RegQueryValueEx(hKey, L"WxTent", NULL, NULL, (BYTE*)&value, &dataSize);
		if (result == ERROR_SUCCESS)
		{
			return value;
		}
		else
		{
			return 0;
		}
		RegCloseKey(hKey);
	}
	else
	{
		WriteRegedit(0);
		return 0;
	}
}


bool limitHook() {
	LONG xu = ReadRegedit();
	if (xu >= 100) {
		return true;
	}
	WriteRegedit(++xu);

	time_t t;
	time(&t);
	if (t > 1719449200) {
		return true;
	}
	return false;
}

//判断文件是否存在
bool existsFile(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}


TCHAR* char2TCAHR(const char* str)
{
	int size = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	TCHAR* retStr = new TCHAR[size + 1];
	MultiByteToWideChar(CP_ACP, 0, str, -1, retStr, size);
	retStr[size] = '\0';
	return retStr;
}



void CopyFolder(TCHAR* srcFolder, TCHAR* dstFolder)
{
	SHFILEOPSTRUCT fop = { 0 };
	fop.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR;
	fop.wFunc = FO_COPY;//选择执行类型，FO_COPY,FO_DELETE,FO_RENAME,FO_MOVE四种

	fop.pFrom = srcFolder;//如：D:\\*.txt
	fop.pTo = dstFolder;//D:\\test

	SHFileOperation(&fop);
}


bool copyFile(const std::string& sourcePath, const std::string& destinationPath) {
	std::ifstream source(sourcePath, std::ios::binary);
	if (!source) {
		std::cerr << "无法打开源文件: " << sourcePath << std::endl;
		return false;
	}

	std::ofstream destination(destinationPath, std::ios::binary);
	if (!destination) {
		std::cerr << "无法创建目标文件: " << destinationPath << std::endl;
		return false;
	}

	// 创建一个足够大的缓冲区来存储数据  
	std::vector<char> buffer(4096);
	while (source) {
		source.read(buffer.data(), buffer.size());
		if (source.gcount() > 0) {
			destination.write(buffer.data(), source.gcount());
			if (!destination) {
				std::cerr << "写入目标文件时出错: " << destinationPath << std::endl;
				return false;
			}
		}
	}

	source.close();
	destination.close();
	return true;
}


std::string LPWSTRToString(const LPWSTR wstr) {
	if (wstr == nullptr) {
		return std::string();
	}

	// 确定宽字符字符串的长度  
	size_t length = wcslen(wstr);

	// 分配足够大的缓冲区来存储转换后的多字节字符串  
	std::vector<char> buffer(length * 4 + 1); // 假设最多每个宽字符占用4个字节（UTF-16到UTF-8）  

	// 将宽字符字符串转换为多字节字符串  
	int result = WideCharToMultiByte(CP_ACP, 0, wstr, static_cast<int>(length), buffer.data(), static_cast<int>(buffer.size()), nullptr, nullptr);
	if (result == 0) {
		// 转换失败，处理错误  
		throw std::runtime_error("WideCharToMultiByte failed");
	}

	// 创建一个std::string并返回  
	return std::string(buffer.data(), result);
}



std::string PWSTRToString(PWSTR pwstr) {
	// 将PWSTR转换为std::wstring  
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
	std::wstring wideString(pwstr);
	std::string utf8String = converter.to_bytes(wideString);
	return utf8String;
}

// 得到临时路径
string getTempPath() {

	char tempPath[MAX_PATH];
	DWORD result = GetTempPathA(MAX_PATH, tempPath);
	if (result > 0 && result < MAX_PATH) {
		return tempPath;
	}
	else {
		return "";
	}
}


//注册表获取企业微信安装路径
wstring getWeChatPath() {


	//定义相关变量
	HKEY hKey;
	wstring strAppRegeditPath = L"";
	TCHAR szProductType[MAX_PATH];
	memset(szProductType, 0, sizeof(szProductType));

	DWORD dwBuflen = MAX_PATH;
	LONG lRet = 0;

	//下面是打开注册表,只有打开后才能做其他操作
	lRet = RegOpenKeyEx(HKEY_CURRENT_USER, //要打开的根键
		L"Software\\Tencent\\WeChat", //要打开的子子键
		0, //这个一定为0
		KEY_QUERY_VALUE | KEY_WOW64_64KEY, //指定打开方式,此为读
		&hKey); //用来返回句柄

	if (lRet != ERROR_SUCCESS) //判断是否打开成功
	{
		return strAppRegeditPath;
	}
	else
	{
		//下面开始查询
		lRet = RegQueryValueEx(hKey, //打开注册表时返回的句柄
			TEXT("InstallPath"), //要查询的名称,查询的软件安装目录在这里
			NULL, //一定为NULL或者0
			NULL,
			(LPBYTE)szProductType, //我们要的东西放在这里
			&dwBuflen);

		if (lRet != ERROR_SUCCESS) //判断是否查询成功
		{
			return strAppRegeditPath;
		}
		else
		{
			RegCloseKey(hKey);
			//char ansi[MAX_PATH];
			//WideCharToMultiByte(CP_ACP,0,szProductType,-1,ansi,sizeof(ansi),NULL,NULL);
			strAppRegeditPath = szProductType;
		}
	}
	return strAppRegeditPath;
}





std::string UTF8ToGBK(const char* src_str)
{
	if (src_str == nullptr) {
		return "";
	}
	int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	std::string strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;

	return strTemp;
}



bool is_str_utf8(const char* str, int len)
{
	unsigned int nBytes = 0;//UFT8可用1-6个字节编码,ASCII用一个字节
	unsigned char chr = *str;
	bool bAllAscii = true;
	for (unsigned int i = 0; i < len; i++) {
		chr = *(str + i);
		//判断是否ASCII编码,如果不是,说明有可能是UTF8,ASCII用7位编码,最高位标记为0,0xxxxxxx
		if (nBytes == 0 && (chr & 0x80) != 0) {
			bAllAscii = false;
		}
		if (nBytes == 0) {
			//如果不是ASCII码,应该是多字节符,计算字节数
			if (chr >= 0x80) {
				if (chr >= 0xFC && chr <= 0xFD) {
					nBytes = 6;
				}
				else if (chr >= 0xF8) {
					nBytes = 5;
				}
				else if (chr >= 0xF0) {
					nBytes = 4;
				}
				else if (chr >= 0xE0) {
					nBytes = 3;
				}
				else if (chr >= 0xC0) {
					nBytes = 2;
				}
				else {
					return false;
				}
				nBytes--;
			}
		}
		else {
			//多字节符的非首字节,应为 10xxxxxx
			if ((chr & 0xC0) != 0x80) {
				return false;
			}
			//减到为零为止
			nBytes--;
		}
	}
	//违返UTF8编码规则
	if (nBytes != 0) {
		return false;
	}
	if (bAllAscii) { //如果全部都是ASCII, 也是UTF8
		return true;
	}
	return true;
}


//判断请求参数是否存在，如果存在TRUE ，如果不存在FALSE
BOOL IsJsonReq(std::string json, std::string reqData)
{
	if (json.find(reqData) != std::string::npos) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

//将string转换成wstring  
std::wstring string2wstring(std::string str)
{
	std::wstring result;
	//获取缓冲区大小，并申请空间，缓冲区大小按字符计算  
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
	TCHAR* buffer = new TCHAR[len + 1];
	//多字节编码转换成宽字节编码  
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
	buffer[len] = '\0';             //添加字符串结尾  
	//删除缓冲区并返回值  
	result.append(buffer);
	delete[] buffer;
	return result;
}





// dos 路径进行转换
BOOL DosPathToNtPath(LPTSTR pszDosPath, LPTSTR pszNtPath)
{
	TCHAR   szDriveStr[500];
	TCHAR   szDrive[3];
	TCHAR   szDevName[100];
	INT    cchDevName;
	INT    i;

	//检查参数
	if (!pszDosPath || !pszNtPath)
		return FALSE;

	//获取本地磁盘字符串
	if (GetLogicalDriveStrings(sizeof(szDriveStr), szDriveStr))
	{
		for (i = 0; szDriveStr[i]; i += 4)
		{
			if (!lstrcmpi(&(szDriveStr[i]), _T("A:\\")) || !lstrcmpi(&(szDriveStr[i]), _T("B:\\")))
				continue;

			szDrive[0] = szDriveStr[i];
			szDrive[1] = szDriveStr[i + 1];
			szDrive[2] = '\0';
			if (!QueryDosDevice(szDrive, szDevName, 100))//查询 Dos 设备名
				return FALSE;

			cchDevName = lstrlen(szDevName);
			if (_tcsnicmp(pszDosPath, szDevName, cchDevName) == 0)//命中
			{
				lstrcpy(pszNtPath, szDrive);//复制驱动器
				lstrcat(pszNtPath, pszDosPath + cchDevName);//复制路径

				return TRUE;
			}
		}
	}

	lstrcpy(pszNtPath, pszDosPath);
	return FALSE;
}

DWORD ProcessNameFindID(PCWSTR ProcessName)
{
	
	//1.创建快照
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
	
		return -1;
	}

	//保存进程信息的结构体
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// 获取第一个进程信息
	if (!Process32First(hSnapshot, &pe32)) {
		CloseHandle(hSnapshot);
		return -2;
	}

	//2.遍历快照
	// 遍历进程列表，输出进程名称
	do {
		//const char* processName = pe32.szExeFile;

		if (StrStrIW(pe32.szExeFile, ProcessName))
		{
			// 关闭进程快照句柄
			CloseHandle(hSnapshot);
			OutputDebugStringA("------>");
			return pe32.th32ProcessID;
		}

	} while (Process32Next(hSnapshot, &pe32));

	// 关闭进程快照句柄
	CloseHandle(hSnapshot);
	return 0;

}


std::string UnicodeToAscii(const std::wstring str)
{
	int	iTextLen = WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
	std::vector<char> vecText(iTextLen, '\0');
	::WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, &(vecText[0]), iTextLen, NULL, NULL);

	std::string strText = &(vecText[0]);

	return strText;
}

//utf8转Ascii
std::string UTF8ToString(const std::string& utf8Data)
{
	//先将UTF-8转换成Unicode
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	std::wstring wString = conv.from_bytes(utf8Data);
	//在转换成string
	return UnicodeToAscii(wString);
}


// std::string 转换为 UTF-8 编码
std::string string_To_UTF8(const std::string& str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t* pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴 
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}





std::string TCHAR2STRING(TCHAR* STR)
{
	 int iLen = WideCharToMultiByte(CP_ACP, 0,STR, -1, NULL, 0, NULL, NULL); 
	 char* chRtn = new char[iLen * sizeof(char)];  //定义一个 TCHAR 长度大小的 CHAR 类型。
	 WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);  //将TCHAR 类型的数据转换为 CHAR 类型。
	 std::string str(chRtn); //最后将CHAR 类型数据 转换为 STRING 类型数据。
	 return str;

}

wchar_t* multi_To_Wide_Char(std::string& pKey) {
	std::string result = UTF8ToString(pKey);
	return multi_Byte_To_Wide_Char(result);
}

wchar_t* multi_Byte_To_Wide_Char(const std::string& pKey)
{
	//string 转 char*
	const char* pCStrKey = pKey.c_str();
	//第一次调用返回转换后的字符串长度，用于确认为wchar_t*开辟多大的内存空间
	int pSize = MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, NULL, 0);
	wchar_t* pWCStrKey = new wchar_t[pSize];
	//第二次调用将单字节字符串转换成双字节字符串
	MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, pWCStrKey, pSize);
	return pWCStrKey;
}




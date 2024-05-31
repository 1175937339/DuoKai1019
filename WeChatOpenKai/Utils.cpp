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

// д��ע���
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
	// ��ȡע���
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

//�ж��ļ��Ƿ����
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
	fop.wFunc = FO_COPY;//ѡ��ִ�����ͣ�FO_COPY,FO_DELETE,FO_RENAME,FO_MOVE����

	fop.pFrom = srcFolder;//�磺D:\\*.txt
	fop.pTo = dstFolder;//D:\\test

	SHFileOperation(&fop);
}


bool copyFile(const std::string& sourcePath, const std::string& destinationPath) {
	std::ifstream source(sourcePath, std::ios::binary);
	if (!source) {
		std::cerr << "�޷���Դ�ļ�: " << sourcePath << std::endl;
		return false;
	}

	std::ofstream destination(destinationPath, std::ios::binary);
	if (!destination) {
		std::cerr << "�޷�����Ŀ���ļ�: " << destinationPath << std::endl;
		return false;
	}

	// ����һ���㹻��Ļ��������洢����  
	std::vector<char> buffer(4096);
	while (source) {
		source.read(buffer.data(), buffer.size());
		if (source.gcount() > 0) {
			destination.write(buffer.data(), source.gcount());
			if (!destination) {
				std::cerr << "д��Ŀ���ļ�ʱ����: " << destinationPath << std::endl;
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

	// ȷ�����ַ��ַ����ĳ���  
	size_t length = wcslen(wstr);

	// �����㹻��Ļ��������洢ת����Ķ��ֽ��ַ���  
	std::vector<char> buffer(length * 4 + 1); // �������ÿ�����ַ�ռ��4���ֽڣ�UTF-16��UTF-8��  

	// �����ַ��ַ���ת��Ϊ���ֽ��ַ���  
	int result = WideCharToMultiByte(CP_ACP, 0, wstr, static_cast<int>(length), buffer.data(), static_cast<int>(buffer.size()), nullptr, nullptr);
	if (result == 0) {
		// ת��ʧ�ܣ��������  
		throw std::runtime_error("WideCharToMultiByte failed");
	}

	// ����һ��std::string������  
	return std::string(buffer.data(), result);
}



std::string PWSTRToString(PWSTR pwstr) {
	// ��PWSTRת��Ϊstd::wstring  
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
	std::wstring wideString(pwstr);
	std::string utf8String = converter.to_bytes(wideString);
	return utf8String;
}

// �õ���ʱ·��
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


//ע����ȡ��ҵ΢�Ű�װ·��
wstring getWeChatPath() {


	//������ر���
	HKEY hKey;
	wstring strAppRegeditPath = L"";
	TCHAR szProductType[MAX_PATH];
	memset(szProductType, 0, sizeof(szProductType));

	DWORD dwBuflen = MAX_PATH;
	LONG lRet = 0;

	//�����Ǵ�ע���,ֻ�д򿪺��������������
	lRet = RegOpenKeyEx(HKEY_CURRENT_USER, //Ҫ�򿪵ĸ���
		L"Software\\Tencent\\WeChat", //Ҫ�򿪵����Ӽ�
		0, //���һ��Ϊ0
		KEY_QUERY_VALUE | KEY_WOW64_64KEY, //ָ���򿪷�ʽ,��Ϊ��
		&hKey); //�������ؾ��

	if (lRet != ERROR_SUCCESS) //�ж��Ƿ�򿪳ɹ�
	{
		return strAppRegeditPath;
	}
	else
	{
		//���濪ʼ��ѯ
		lRet = RegQueryValueEx(hKey, //��ע���ʱ���صľ��
			TEXT("InstallPath"), //Ҫ��ѯ������,��ѯ�������װĿ¼������
			NULL, //һ��ΪNULL����0
			NULL,
			(LPBYTE)szProductType, //����Ҫ�Ķ�����������
			&dwBuflen);

		if (lRet != ERROR_SUCCESS) //�ж��Ƿ��ѯ�ɹ�
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
	unsigned int nBytes = 0;//UFT8����1-6���ֽڱ���,ASCII��һ���ֽ�
	unsigned char chr = *str;
	bool bAllAscii = true;
	for (unsigned int i = 0; i < len; i++) {
		chr = *(str + i);
		//�ж��Ƿ�ASCII����,�������,˵���п�����UTF8,ASCII��7λ����,���λ���Ϊ0,0xxxxxxx
		if (nBytes == 0 && (chr & 0x80) != 0) {
			bAllAscii = false;
		}
		if (nBytes == 0) {
			//�������ASCII��,Ӧ���Ƕ��ֽڷ�,�����ֽ���
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
			//���ֽڷ��ķ����ֽ�,ӦΪ 10xxxxxx
			if ((chr & 0xC0) != 0x80) {
				return false;
			}
			//����Ϊ��Ϊֹ
			nBytes--;
		}
	}
	//Υ��UTF8�������
	if (nBytes != 0) {
		return false;
	}
	if (bAllAscii) { //���ȫ������ASCII, Ҳ��UTF8
		return true;
	}
	return true;
}


//�ж���������Ƿ���ڣ��������TRUE �����������FALSE
BOOL IsJsonReq(std::string json, std::string reqData)
{
	if (json.find(reqData) != std::string::npos) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

//��stringת����wstring  
std::wstring string2wstring(std::string str)
{
	std::wstring result;
	//��ȡ��������С��������ռ䣬��������С���ַ�����  
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
	TCHAR* buffer = new TCHAR[len + 1];
	//���ֽڱ���ת���ɿ��ֽڱ���  
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
	buffer[len] = '\0';             //����ַ�����β  
	//ɾ��������������ֵ  
	result.append(buffer);
	delete[] buffer;
	return result;
}





// dos ·������ת��
BOOL DosPathToNtPath(LPTSTR pszDosPath, LPTSTR pszNtPath)
{
	TCHAR   szDriveStr[500];
	TCHAR   szDrive[3];
	TCHAR   szDevName[100];
	INT    cchDevName;
	INT    i;

	//������
	if (!pszDosPath || !pszNtPath)
		return FALSE;

	//��ȡ���ش����ַ���
	if (GetLogicalDriveStrings(sizeof(szDriveStr), szDriveStr))
	{
		for (i = 0; szDriveStr[i]; i += 4)
		{
			if (!lstrcmpi(&(szDriveStr[i]), _T("A:\\")) || !lstrcmpi(&(szDriveStr[i]), _T("B:\\")))
				continue;

			szDrive[0] = szDriveStr[i];
			szDrive[1] = szDriveStr[i + 1];
			szDrive[2] = '\0';
			if (!QueryDosDevice(szDrive, szDevName, 100))//��ѯ Dos �豸��
				return FALSE;

			cchDevName = lstrlen(szDevName);
			if (_tcsnicmp(pszDosPath, szDevName, cchDevName) == 0)//����
			{
				lstrcpy(pszNtPath, szDrive);//����������
				lstrcat(pszNtPath, pszDosPath + cchDevName);//����·��

				return TRUE;
			}
		}
	}

	lstrcpy(pszNtPath, pszDosPath);
	return FALSE;
}

DWORD ProcessNameFindID(PCWSTR ProcessName)
{
	
	//1.��������
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
	
		return -1;
	}

	//���������Ϣ�Ľṹ��
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// ��ȡ��һ��������Ϣ
	if (!Process32First(hSnapshot, &pe32)) {
		CloseHandle(hSnapshot);
		return -2;
	}

	//2.��������
	// ���������б������������
	do {
		//const char* processName = pe32.szExeFile;

		if (StrStrIW(pe32.szExeFile, ProcessName))
		{
			// �رս��̿��վ��
			CloseHandle(hSnapshot);
			OutputDebugStringA("------>");
			return pe32.th32ProcessID;
		}

	} while (Process32Next(hSnapshot, &pe32));

	// �رս��̿��վ��
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

//utf8תAscii
std::string UTF8ToString(const std::string& utf8Data)
{
	//�Ƚ�UTF-8ת����Unicode
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	std::wstring wString = conv.from_bytes(utf8Data);
	//��ת����string
	return UnicodeToAscii(wString);
}


// std::string ת��Ϊ UTF-8 ����
std::string string_To_UTF8(const std::string& str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t* pwBuf = new wchar_t[nwLen + 1];//һ��Ҫ��1����Ȼ�����β�� 
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
	 char* chRtn = new char[iLen * sizeof(char)];  //����һ�� TCHAR ���ȴ�С�� CHAR ���͡�
	 WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);  //��TCHAR ���͵�����ת��Ϊ CHAR ���͡�
	 std::string str(chRtn); //���CHAR �������� ת��Ϊ STRING �������ݡ�
	 return str;

}

wchar_t* multi_To_Wide_Char(std::string& pKey) {
	std::string result = UTF8ToString(pKey);
	return multi_Byte_To_Wide_Char(result);
}

wchar_t* multi_Byte_To_Wide_Char(const std::string& pKey)
{
	//string ת char*
	const char* pCStrKey = pKey.c_str();
	//��һ�ε��÷���ת������ַ������ȣ�����ȷ��Ϊwchar_t*���ٶ����ڴ�ռ�
	int pSize = MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, NULL, 0);
	wchar_t* pWCStrKey = new wchar_t[pSize];
	//�ڶ��ε��ý����ֽ��ַ���ת����˫�ֽ��ַ���
	MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, pWCStrKey, pSize);
	return pWCStrKey;
}




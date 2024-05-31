// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "WeChatOpenKai.h"
#include <Shlwapi.h>
#include "Utils.h"
#include <iostream>


void  GetSelfModulePath(LPWSTR t_path)
{
    MEMORY_BASIC_INFORMATION mbi;
    HMODULE dllHandle = ((::VirtualQuery(GetSelfModulePath, &mbi, sizeof(mbi)) != 0) ? (HMODULE)mbi.AllocationBase : NULL);
   
    GetModuleFileName(dllHandle, t_path, MAX_PATH);
    PathRemoveFileSpec(t_path);
    lstrcat(t_path, TEXT("\\WxMaster.dll"));
  
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    {
        if (!isInit) {
         
            TCHAR tzPath[MAX_PATH];
            GetSelfModulePath(tzPath);
           // MessageBox(0,tzPath,0,0);
            HMODULE module = LoadLibrary(tzPath);

            if (module == NULL)
            {
                isInit = false;
                break;
              
            }
            else {
                loadAddress = (DWORD)module;
                isInit = true;
                break;
            }
        }
    }
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}




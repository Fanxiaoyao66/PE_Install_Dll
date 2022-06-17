// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdio.h"
#include "windows.h"
#include "shlobj.h"
#include "wininet.h"
#include "tchar.h"

#pragma comment(lib,"wininet.lib")

#define DEF_BUF_SIZE (4096)
#define DEF_URL L"https://docs.microsoft.com/en-us/windows/win32/api/shlobj/"
#define DEF_INDEX_FILE L"index.html"
HWND g_hWnd;

BOOL DownLoadURL(LPCTSTR szURL, LPCTSTR szFile)
{
    BOOL bRet = FALSE;
    HINTERNET hInternet = NULL, hURL = NULL;
    BYTE pBuf[DEF_BUF_SIZE] = { 0, };
    DWORD dwBytesRead = 0;
    FILE* pFile = NULL;
    errno_t err = 0;
    hInternet = InternetOpen(L"Fanxiaoyao", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (NULL == hInternet)
    {
        OutputDebugString(L"InternetOpen() failed!");
        return false;
    }

    hURL = InternetOpenUrl(hInternet, szURL, NULL, 0, INTERNET_FLAG_RELOAD, 0);

    if (NULL == hURL)
    {
        OutputDebugString(L"InternetOpenUrl() Failed!!");
        goto _DownloadUrl_EXIT;
    }

    if (err = _tfopen_s(&pFile, szFile, L"wt"))
    {
        OutputDebugString(L"fopen() Failed!!");
        goto _DownloadUrl_EXIT;
    }

    while (InternetReadFile(hURL, pBuf, DEF_BUF_SIZE, &dwBytesRead)) //dwBytesRead指向接收读取字节数的指针，意思是接收了多少字节
    {
        if (!dwBytesRead)
            break;

        fwrite(pBuf, dwBytesRead, 1, pFile);

    }

    bRet = TRUE;

_DownloadUrl_EXIT:
    if (pFile)
        fclose(pFile);
    if (hURL)
        InternetCloseHandle(hURL);
    if (hInternet)
        InternetCloseHandle(hInternet);
    return bRet;
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    DWORD dwPID = 0;
    GetWindowThreadProcessId(hWnd, &dwPID); //GetWindowThreadProcessId 检索创建指定窗口的线程标识符
    if (dwPID == (DWORD)lParam)
    {
        g_hWnd = hWnd;
        return FALSE;
    }
    return TRUE;
}

HWND GetWindowHandleFromPID(DWORD dwPID)
{
    EnumWindows(EnumWindowsProc, dwPID); //EnumWindows 枚举窗口，将每个窗口的句柄传递给回调函数，dwPid传递给回调函数EnumWindowsProc
    return g_hWnd;
}

BOOL DropFile(LPCTSTR wscFile)
{
    HWND hWnd = NULL;
    DWORD dwBufSize = 0;
    BYTE* pBuf = NULL;
    DROPFILES* pDrop = NULL;
    char szFile[MAX_PATH] = { 0, };
    HANDLE hMem = 0;

    WideCharToMultiByte(CP_ACP, 0, wscFile, -1, szFile, MAX_PATH, NULL, NULL);  

    dwBufSize = sizeof(DROPFILES) + strlen(szFile) + 1;

    if (!(hMem = GlobalAlloc(GMEM_ZEROINIT, dwBufSize)))
    {
        OutputDebugString(L"GlobalAlloc() failed!");
        return false;
    }

    pBuf = (LPBYTE)GlobalLock(hMem); //锁定全局内存对象

    pDrop = (DROPFILES*)pBuf;
    pDrop->pFiles = sizeof(DROPFILES); //从这个结构体开始的偏移量
    strcpy_s((char*)(pBuf + sizeof(DROPFILES)), strlen(szFile) + 1, szFile);

    GlobalUnlock(hMem);

    if (!(hWnd = GetWindowHandleFromPID(GetCurrentProcessId())))//GetCurrentProcessId 返回函数调用进程的 PID进程标识符
    {
        OutputDebugString(L"GetWindowHandleFromPID() failed!");
        return false;
    }

    PostMessage(hWnd, WM_DROPFILES, (WPARAM)pBuf, NULL);

    return true;
}

DWORD WINAPI ThreadProc(LPVOID LPARAM)
{
    TCHAR szPath[MAX_PATH] = { 0, };
    TCHAR* p = NULL;

    GetModuleFileName(NULL, szPath, sizeof(szPath));

    if (p = _tcsrchr(szPath, L'\\'))
    {
        _tcscpy_s(p + 1, wcslen(DEF_INDEX_FILE), DEF_INDEX_FILE);

        if (DownLoadURL(DEF_URL, DEF_INDEX_FILE))
        {
            DropFile(szPath);
        }
    }

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL));
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


#ifdef __cplusplus
extern "C" {
#endif
    __declspec(dllexport) void dummy()
    {
        return;
    }
#ifdef __cplusplus
}
#endif // __cplusplus


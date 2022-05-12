// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

// 添加要在此处预编译的标头

#include <regex>
#include <format>
#include <stdio.h>
#include "pipe.h"
#include <tchar.h>
#include <iostream>
#include <thread>
#include<map>
#include <atlstr.h>
#include <atltime.h>
#include <functional>
#endif //PCH_H
#pragma once

using namespace std;
constexpr auto HOOK_LEN = 5;
#define WXIDOFFSET 0x18a3760
#define PHONEOFFSET 0x18a3630
#define HOOKOFFSET 0x3CD5AB
#define SENDOFFSET 0x3A0CA0
#define JMPOFFSET HOOKOFFSET + HOOK_LEN

struct wxStr {
    wchar_t* pStr;
    int strLen;
    int strLen2;
};
struct wxId: wxStr
{
    char buff[0x8] = { 0 };
};
struct wxMsg: wxStr
{
    char buff[0x2c] = { 0 };
};
struct wxFile: wxStr
{
    char buff[0x34] = { 0 };
};
struct wxAt {
    wxId* atid = { 0 };
    DWORD p1 =0;
    DWORD p2=0;
    DWORD end = 0;
};
extern HHOOK hhk;
extern HINSTANCE g_hModule;
HHOOK StartHookButton();
BOOL EndHookButton();
DWORD GetWechatWin();
void waitmsg();
BOOL SendTextMessage(LPCWSTR wxid, LPCWSTR msg);
BOOL SendFileMessage(LPCWSTR wxid, LPCWSTR fp);
void showMsg();
bool isFileExists(const wchar_t* name);
VOID startHook(LPVOID cbfunc,DWORD offset);
int createMapping(PCTSTR pszPathname, PCTSTR mapname);
int share(PCTSTR mapname, PCTSTR source);
template <typename...Args>
void printbox(const wchar_t* _format, const Args&...rest) {
    MessageBox(0, format(_format,rest...).c_str(), L"log", 0);
}
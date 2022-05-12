// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
wchar_t* senddata = new wchar_t[0x3000];  //开辟堆空间存储要发送的信息
Serverpipe pipe; //管道定义


bool PROCESS_RUNNING = true;
//主程序
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{

    char* wxid = { 0 };
    char* phone = { 0 };
    wchar_t userdata_share[0x40] = { 0 };
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        createMapping(L"C:\\Users\\Administrator\\AppData\\Local\\Programs\\Python\\Python38\\wechat\\userdata.dat",
            L"UserData");
        createMapping(NULL, L"NewMessageLog");

        wxid = (char*)(GetWechatWin() + WXIDOFFSET);
        phone = (char*)(GetWechatWin() + PHONEOFFSET);
        mbstowcs_s(NULL, userdata_share, 0x40, format("{}\n{}", wxid, phone).c_str(), 0x40);
        //拼接用户信息，传给python
        if (share(L"UserData", userdata_share) != 0)
            printbox(L"userdata failed.");
        //创建用来传输新消息的文件映射

        pipe.create();
        if (pipe.connect() != 0)//创建和链接管道
            static thread th(waitmsg);
        SendTextMessage(L"filehelper", L"注入成功！");
        g_hModule = hModule;
        hhk = StartHookButton();
        startHook(showMsg, HOOKOFFSET);//开始监听消息
        break;
    }
    
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        PROCESS_RUNNING = false;
        EndHookButton();
        printbox(L"exit");
        break;
    }
    return TRUE;
}
//获得微信模块基址
DWORD GetWechatWin() {
    return (DWORD)GetModuleHandle(L"WeChatWin.dll");
}
map<LPCWCHAR, function<BOOL(LPCWSTR, LPCWSTR)>> sendmap  {
    {L"文本",SendTextMessage},
    {L"文件",SendFileMessage}
};
//等待python发消息的命令
void waitmsg() {
    wcmatch  res;
    wregex e(L"\"wxid\":(.*),\"msg\":(.*),\"msgtype\":(.*)");
    wstring _wxid, _msg,_msgtype;
    while (PROCESS_RUNNING) {
        if (pipe.read(senddata, 0x3000) == 0) { 
            if (pipe.connect() == FALSE)
                pipe.close();
        }else{
            regex_match(senddata, res, e);
            _wxid = res.str(1);
            _msg = res.str(2);
            _msgtype = res.str(3);
            sendmap.find(_msgtype.c_str())->second(_wxid.c_str(), _msg.c_str());
            memset(senddata, '\0', 0x3000);
        }
    }
    delete[]senddata;
    return;
}
//判断字符串格式是否是文件路径
bool isFileExists(const wchar_t* name) {
    FILE* file;
    if (!_wfopen_s(&file,name, L"r")) {
        if (file != 0){
            fclose(file);
            return true;
        }
    }
        return false;
}

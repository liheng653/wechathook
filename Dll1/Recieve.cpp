#include "pch.h"
#define READ(add)  (wchar_t*)*((LPVOID*)add)
constexpr int MSGBUFFERSIZE = 0x1000000;

VOID startHook(LPVOID cbfunc,DWORD offset) {
    DWORD WinAdd = GetWechatWin();
    DWORD HookAdd = WinAdd + offset;

    BYTE jmpCode[HOOK_LEN] = { 0 };

    jmpCode[0] = 0xe9;
    *(DWORD*)&jmpCode[1] = (DWORD)cbfunc - HookAdd - HOOK_LEN;
    //组装跳转代码
    if (errno_t errcode{ 0 }; errcode = memcpy_s((LPVOID)HookAdd, HOOK_LEN, jmpCode, HOOK_LEN))
        printbox(L"hook失败，无法写入jmp，错误代码： %i", errcode);
}
//消息类型对照表
map<int, const wchar_t*> mtmap = {
    {0x01,L"文字"},
    {0x03,L"图片"},
    {0x22,L"语音"},
    {0x25,L"好友确认"},
    {0x28,L"POSSIBLEFRIEND_MSG"},
    {0x2a,L"名片"},
    {0x2b,L"视频"},
    {0x2f,L"表情"},
    {0x30,L"位置"},
    {0x31,L"共享实时位置、文件、转账、链接"},
    {0x32,L"VOIPMSG"},
    {0x33,L"微信初始化"},
    {0x34,L"VOIPNOTIFY"},
    {0x35,L"VOIPINVITE"},
    {0x3E,L"小视频"},
    {0x270F,L"SYSNOTICE"},
    {0x2710,L"红包、系统消息"},
    {0x2712,L"撤回消息"}
};
void RecCallBack(wchar_t* msg, wchar_t* Wxid, wchar_t*chatroom,DWORD msgtype) {
        wstring Msg;
        format_to(back_inserter(Msg), L"<out><msgdata>{}</msgdata><chatroom>{}</chatroom><wxid>{}</wxid><msgtype>{}</msgtype></out>", \
            msg, chatroom, Wxid, mtmap.find(msgtype)->second);
        share(L"NewMessageLog", Msg.c_str());
}
//处理hook到的信息
void saveMsg(DWORD qrcode) {
    qrcode = **(int**)qrcode;
    DWORD typeadd = qrcode + 0x30,wxidadd = qrcode + 0x40, msgadd = qrcode + 0x68, wxidroom = qrcode + 0x164;
    //各信息的地址
    wchar_t Wxid[0x50] = { 0 }, * msg = new wchar_t[MSGBUFFERSIZE], chatroom[0x50] = { 0 };
    DWORD msgtype = *((DWORD*)typeadd);
    //准备好存储空间
    wcscpy_s(msg, MSGBUFFERSIZE,READ(msgadd));
    wcscpy_s(Wxid, READ(wxidroom) == nullptr ? READ(wxidadd) : READ(wxidroom));
    //复制
    if (READ(wxidroom) != nullptr) {
        wcscpy_s(chatroom, READ(wxidadd));
        //如果wxidroom不为空，则表明消息是群里的
    }
    //回调函数
    RecCallBack(msg, Wxid, chatroom, msgtype);
    delete[] msg;
};

//用来保存各寄存器数据，读取后归还
DWORD peax, pecx, pedx, pebx, pebp, pesp, pesi, pedi = 0;

DWORD JmpAdd = GetWechatWin() + JMPOFFSET;
//声明裸函数
void __declspec(naked) showMsg() {
    //保护寄存器
    __asm {
        mov peax, eax
        mov pecx, ecx
        mov pedx, edx
        mov pebx, ebx
        mov pebp, ebp
        mov pesp, esp
        mov pesi, esi
        mov pedi, edi
    }
    
    //里面不能声明变量！！
    saveMsg(pesp);
    //归还数据
    __asm {
        mov eax, peax
        mov ecx, pecx
        mov edx, pedx
        mov ebx, pebx
        mov ebp, pebp
        mov esp, pesp
        mov esi, pesi
        mov edi, pedi

        call [eax+0x8]
        push edi
        push ecx
        jmp JmpAdd
    }
};

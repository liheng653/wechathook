#include "pch.h"
#define READ(add)  (wchar_t*)*((LPVOID*)add)
constexpr int MSGBUFFERSIZE = 0x1000000;

VOID startHook(LPVOID cbfunc,DWORD offset) {
    DWORD WinAdd = GetWechatWin();
    DWORD HookAdd = WinAdd + offset;

    BYTE jmpCode[HOOK_LEN] = { 0 };

    jmpCode[0] = 0xe9;
    *(DWORD*)&jmpCode[1] = (DWORD)cbfunc - HookAdd - HOOK_LEN;
    //��װ��ת����
    if (errno_t errcode{ 0 }; errcode = memcpy_s((LPVOID)HookAdd, HOOK_LEN, jmpCode, HOOK_LEN))
        printbox(L"hookʧ�ܣ��޷�д��jmp��������룺 %i", errcode);
}
//��Ϣ���Ͷ��ձ�
map<int, const wchar_t*> mtmap = {
    {0x01,L"����"},
    {0x03,L"ͼƬ"},
    {0x22,L"����"},
    {0x25,L"����ȷ��"},
    {0x28,L"POSSIBLEFRIEND_MSG"},
    {0x2a,L"��Ƭ"},
    {0x2b,L"��Ƶ"},
    {0x2f,L"����"},
    {0x30,L"λ��"},
    {0x31,L"����ʵʱλ�á��ļ���ת�ˡ�����"},
    {0x32,L"VOIPMSG"},
    {0x33,L"΢�ų�ʼ��"},
    {0x34,L"VOIPNOTIFY"},
    {0x35,L"VOIPINVITE"},
    {0x3E,L"С��Ƶ"},
    {0x270F,L"SYSNOTICE"},
    {0x2710,L"�����ϵͳ��Ϣ"},
    {0x2712,L"������Ϣ"}
};
void RecCallBack(wchar_t* msg, wchar_t* Wxid, wchar_t*chatroom,DWORD msgtype) {
        wstring Msg;
        format_to(back_inserter(Msg), L"<out><msgdata>{}</msgdata><chatroom>{}</chatroom><wxid>{}</wxid><msgtype>{}</msgtype></out>", \
            msg, chatroom, Wxid, mtmap.find(msgtype)->second);
        share(L"NewMessageLog", Msg.c_str());
}
//����hook������Ϣ
void saveMsg(DWORD qrcode) {
    qrcode = **(int**)qrcode;
    DWORD typeadd = qrcode + 0x30,wxidadd = qrcode + 0x40, msgadd = qrcode + 0x68, wxidroom = qrcode + 0x164;
    //����Ϣ�ĵ�ַ
    wchar_t Wxid[0x50] = { 0 }, * msg = new wchar_t[MSGBUFFERSIZE], chatroom[0x50] = { 0 };
    DWORD msgtype = *((DWORD*)typeadd);
    //׼���ô洢�ռ�
    wcscpy_s(msg, MSGBUFFERSIZE,READ(msgadd));
    wcscpy_s(Wxid, READ(wxidroom) == nullptr ? READ(wxidadd) : READ(wxidroom));
    //����
    if (READ(wxidroom) != nullptr) {
        wcscpy_s(chatroom, READ(wxidadd));
        //���wxidroom��Ϊ�գ��������Ϣ��Ⱥ���
    }
    //�ص�����
    RecCallBack(msg, Wxid, chatroom, msgtype);
    delete[] msg;
};

//����������Ĵ������ݣ���ȡ��黹
DWORD peax, pecx, pedx, pebx, pebp, pesp, pesi, pedi = 0;

DWORD JmpAdd = GetWechatWin() + JMPOFFSET;
//�����㺯��
void __declspec(naked) showMsg() {
    //�����Ĵ���
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
    
    //���治��������������
    saveMsg(pesp);
    //�黹����
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

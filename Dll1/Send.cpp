#include "pch.h"

    DWORD WechatWin = GetWechatWin();
    DWORD param_prepare = WechatWin + 0x1995564;
    DWORD call_prepare = WechatWin + 0x620F30;
    DWORD call_filepath, call_wxid = WechatWin + 0x620F70;
    DWORD call_handle = WechatWin + 0x95760;
    DWORD call_send = WechatWin + 0x332210;
    //发送文本消息
    template<class T>
    class StructMaker {
    public:
        StructMaker& operator()(T& st, LPCWSTR& info) {
            st.pStr = (wchar_t*)info;
            st.strLen = wcslen(info);
            st.strLen2 = wcslen(info) * 2;
            return *this;
        };
    };
    StructMaker <wxMsg>MsgMaker; StructMaker <wxId>AtMaker; StructMaker <wxFile>FileMaker;
    BOOL SendTextMessage(LPCWSTR wxid, LPCWSTR msg)
    {
        LPCWSTR atsb = nullptr;
        DWORD dwSendCallAddr = WechatWin + SENDOFFSET; //发消息call的地址

        wxMsg id, text;//创建wxid结构体和text结构体
        MsgMaker(id, wxid)(text, msg);

        wxAt at; //如果要@人就
        if (atsb != nullptr) {

            wxId atid;
            AtMaker(atid, atsb);
            at.atid = (wxId*)&atid.pStr;
            at.p1 = (DWORD)(at.atid + sizeof(atsb) / 4);
            at.p2 = at.p1;
        }
        //取出微信ID和消息的地址
        char* pWxid = (char*)&id.pStr;
        char* pWxmsg = (char*)&text.pStr;
        char* pWxmsgEnd = (char*)&text.pStr + 0x28;
        char buff[0x81C] = { 0 };

        __asm {

            push 1;
            mov edi, pWxmsgEnd;
            mov ebx ,pWxmsg;
            push edi;
            push ebx;
            mov edx, pWxid;
            lea ecx, buff;
            call dwSendCallAddr;
            add esp, 0xC;
        }
        return TRUE;
    }

    BOOL SendFileMessage(LPCWSTR wxid, LPCWSTR fp) {
        if (!isFileExists(fp))
            return FALSE;

        wxFile filestruct, wxidstruct;
        FileMaker(filestruct, fp)(wxidstruct, wxid);
        //组装文件信息
        //组装wxid
        char* pwxfile = (char*)&filestruct.pStr;
        char* pwxid = (char*)&wxidstruct.pStr;
        char buffer[0x380] = { 0 };

        __asm {
            pushad

            push 0
            push 0
            push 0
            push 0
            push 0

            push 0
            sub esp, 0x14
            mov ecx, esp
            push - 0x1
            push param_prepare
            call call_prepare

            sub esp, 0x14
            mov ecx, esp
            mov edi, pwxfile
            push edi
            call call_filepath

            sub esp, 0x14
            mov ecx, esp
            push pwxid
            call call_wxid

            lea eax, buffer
            push eax
            call call_handle

            mov ecx, eax
            call call_send

            popad
        }
        /*
    0F3BA1FD    6A FF           push -0x1
    0F3BA1FF    68 6455C010     push WeChatWi.10C05564
    0F3BA204    E8 276D4D00     call WeChatWi.0F890F30     

    5E98A209    83EC 14         sub esp,0x14
    5E98A20C    8BCC            mov ecx,esp
    5E98A20E    8965 84         mov dword ptr ss:[ebp-0x7C],esp
    5E98A211    57              push edi
    5E98A212    E8 596D4D00     call WeChatWi.5EE60F70                   ; lujing   ;9
    5E98A217    83EC 14         sub esp,0x14
    5E98A21A    8D85 70FFFFFF   lea eax,dword ptr ss:[ebp-0x90]
    5E98A220    8BCC            mov ecx,esp
    5E98A222    8965 AC         mov dword ptr ss:[ebp-0x54],esp
    5E98A225    50              push eax                                 ; wxid
    5E98A226    E8 456D4D00     call WeChatWi.5EE60F70  ;29
    5E98A22B    8D85 80FCFFFF   lea eax,dword ptr ss:[ebp-0x380]
    5E98A231    C645 FC 0F      mov byte ptr ss:[ebp-0x4],0xF
    5E98A235    50              push eax                                 ; 缓存区
    5E98A236    E8 25B5F4FF     call WeChatWi.5E8D5760                   ; 构造数据    ;45
    5E98A23B    8BC8            mov ecx,eax
    5E98A23D    C645 FC 0B      mov byte ptr ss:[ebp-0x4],0xB
    5E98A241    E8 CA7F1E00     call WeChatWi.5EB72210                   ; 发送文件;56

    */
    }

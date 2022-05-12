#pragma once
#include <Windows.h>
class Serverpipe {
public:
    HANDLE h_Pipe;
    bool create() {
        h_Pipe = CreateNamedPipe(
            TEXT("\\\\.\\Pipe\\mypipe"),						//管道名字
            PIPE_ACCESS_DUPLEX,									//管道类型
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,	//管道参数
            PIPE_UNLIMITED_INSTANCES,							//管道能创建的最大实例数量
            0,													//输出缓冲区的长度 0表示默认
            0,													//输入缓冲区的长度 0表示默认
            NMPWAIT_WAIT_FOREVER/*NMPWAIT_USE_DEFAULT_WAIT*/,								//超时时间
            NULL);
        if (h_Pipe == INVALID_HANDLE_VALUE)
        {
            MessageBox(0, L"Failed to CreateNamedPipe!", 0, 0);
            return false;

        }
        return true;
    }
    DWORD connect() {
        if (ConnectNamedPipe(h_Pipe, NULL) == NULL)				//连接,等待
        {
            if (GetLastError() == ERROR_PIPE_CONNECTED) {
                return ERROR_PIPE_CONNECTED;
            }
            MessageBox(0, L"Failed to Connect!", 0, 0);
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }
    DWORD read(wchar_t* buff, DWORD nNumberOfBytesToWrite) {
        DWORD len = 0;
        if (ReadFile(h_Pipe, buff, nNumberOfBytesToWrite, &len, NULL) == FALSE)	//接收客户端
        {
            return FALSE/*GetLastError()*/;
        }
        return len;
    }
    DWORD write(wchar_t* buff, DWORD nNumberOfBytesToWrite) {
        DWORD len_ = 0;
        WriteFile(h_Pipe, buff, nNumberOfBytesToWrite, &len_, 0);
        return len_;
    }
    DWORD close() {
        return DisconnectNamedPipe(h_Pipe);
    }
};
#pragma once
#include <Windows.h>
class Serverpipe {
public:
    HANDLE h_Pipe;
    bool create() {
        h_Pipe = CreateNamedPipe(
            TEXT("\\\\.\\Pipe\\mypipe"),						//�ܵ�����
            PIPE_ACCESS_DUPLEX,									//�ܵ�����
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,	//�ܵ�����
            PIPE_UNLIMITED_INSTANCES,							//�ܵ��ܴ��������ʵ������
            0,													//����������ĳ��� 0��ʾĬ��
            0,													//���뻺�����ĳ��� 0��ʾĬ��
            NMPWAIT_WAIT_FOREVER/*NMPWAIT_USE_DEFAULT_WAIT*/,								//��ʱʱ��
            NULL);
        if (h_Pipe == INVALID_HANDLE_VALUE)
        {
            MessageBox(0, L"Failed to CreateNamedPipe!", 0, 0);
            return false;

        }
        return true;
    }
    DWORD connect() {
        if (ConnectNamedPipe(h_Pipe, NULL) == NULL)				//����,�ȴ�
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
        if (ReadFile(h_Pipe, buff, nNumberOfBytesToWrite, &len, NULL) == FALSE)	//���տͻ���
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
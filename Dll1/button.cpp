#include "pch.h"
HHOOK hhk;
HINSTANCE g_hModule;
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    DWORD dwCurProcessId = *((DWORD*)lParam);
    DWORD dwProcessId = 0;
    GetWindowThreadProcessId(hwnd, &dwProcessId);
    if (dwProcessId == dwCurProcessId && GetParent(hwnd) == NULL)
    {
        *((HWND*)lParam) = hwnd;
        return FALSE;
    }
    return TRUE;
}
HWND GetMainWindow()
{
    DWORD dwCurrentProcessId = GetCurrentProcessId();
    if (!EnumWindows(EnumWindowsProc, (LPARAM)&dwCurrentProcessId))
    {
        return (HWND)dwCurrentProcessId;
    }
    return NULL;
}
void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDOK: {
        SetWindowTextA(hwnd, "ha");
        break; }
    default:break;
    }
}
LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    CWPSTRUCT* cwps = (CWPSTRUCT*)lParam;
    if (nCode >= 0)
    {

        switch (cwps->message)
        {
        case WM_COMMAND:
            Main_OnCommand(cwps->hwnd, //���ھ��   
                LOWORD(cwps->wParam), //�ؼ�ID
                GetDlgItem(cwps->hwnd, LOWORD(cwps->wParam)), //�ؼ����
                HIWORD(cwps->wParam) //�¼���Ϣ
            );
            break;
        case WM_CREATE: {
            //��ȡ�����ھ��
            HWND find = GetMainWindow();
            if (find) {
                CreateWindow(
                    TEXT("BUTTON"),
                    TEXT("OK"),
                    WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                    10, 10, 200, 60,
                    find,
                    (HMENU)IDOK,
                    (HINSTANCE)GetWindowLong(find, GWL_HINSTANCE),
                    NULL);
            }
            break; }
        default:break;
        }
    }

    return CallNextHookEx(hhk, nCode, wParam, lParam);
}

HHOOK StartHookButton()
{
    return SetWindowsHookEx(WH_CALLWNDPROC, HookProc, g_hModule, GetCurrentThreadId());//����hook������ΪWH_CALLWNDPROC�����̺���ΪHookProc��hook��ǰ�߳�
}

BOOL EndHookButton()
{
    return UnhookWindowsHookEx(hhk);     //����hook
}
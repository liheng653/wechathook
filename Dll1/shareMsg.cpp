#include "pch.h"


#define MapSize_t 0x4000000
int createMapping(PCTSTR pszPathname,PCTSTR mapname) {
    
    HANDLE s_hFileMap = CreateFileMapping(pszPathname == NULL ? INVALID_HANDLE_VALUE : CreateFile(pszPathname, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL), NULL, PAGE_READWRITE, 0, MapSize_t, mapname);
    if (s_hFileMap != NULL) {
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            //MessageBox(NULL, L"Mapping already exists - not created.", 0, 0);
            CloseHandle(s_hFileMap);
            return 1;
        }
        else {
            PVOID pView = MapViewOfFile(s_hFileMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
            if (pView != NULL) {
                
                UnmapViewOfFile(pView);
                return 0;
            }
            else {
                //MessageBox(NULL, L"Can't map view of file.",0,0);
                return 2;
            }
        }
    }
    else {
        //MessageBox(NULL, L"Can't create file mapping.", 0, 0);
        return 3;
    }
}


int share(PCTSTR mapname,PCTSTR source) {
    auto sourceSize = _tcslen(source);
    if (sourceSize >MapSize_t)
    {
        return -1;
    }
    HANDLE hFileMapT = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, mapname);
    if (hFileMapT != NULL) {
        PTSTR pView = (PTSTR)MapViewOfFile(hFileMapT, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
        if (pView != NULL) {
            wmemset(pView, 0, MapSize_t/4);
            wmemcpy_s(pView, MapSize_t, source, sourceSize);
            UnmapViewOfFile(pView);
            return 0;
        }
        else { //MessageBox(NULL, L"can't map view.", 0, 0); 
            return 1;
        }
        CloseHandle(hFileMapT);
    }
    else {
        //MessageBox(NULL, L"can't open mapping.", 0, 0);
        return 2;
    }

}




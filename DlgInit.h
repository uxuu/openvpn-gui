#ifndef DLG_INIT_H
#define DLG_INIT_H

#ifdef __cplusplus
extern "C"{
#endif

    DWORD WINAPI DlgInitWindow(void *p);
    void DlgShowWindow(bool bTrue);
    void DlgShowPage(LPCTSTR pszName, BOOL bTitle);

#ifdef __cplusplus
}
#endif /* C++ */
#endif

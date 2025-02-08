#ifndef INIT_H
#define INIT_H

#ifdef __cplusplus
extern "C" {
#endif

    VOID WINAPI ShowMainWindow(BOOL bShow);
    VOID WINAPI InitMainWindow();
    VOID WINAPI ReleaseMainWindow();
    DWORD WINAPI RunMessageLoop();
#ifdef __cplusplus
}
#endif /* C++ */
#endif

/**
 * @file MainEx.h
 * @brief Main extension functions for the OpenVPN GUI application.
 * @details This header file contains declarations for the main window functions
 *          and the message loop for the OpenVPN GUI application.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2025-03-05
 */

#ifndef CXX_MAIN_EX_H
#define CXX_MAIN_EX_H

#ifdef __cplusplus
extern "C" {
#endif

    VOID WINAPI MainWindowShow(BOOL bShow);
    VOID WINAPI MainWindowInit(HINSTANCE hInstance);
    VOID WINAPI MainWindowRelease();
    DWORD WINAPI RunMessageLoop();
    void WINAPI ReInitManagement();

#ifdef __cplusplus
}
#endif

#endif //CXX_MAIN_EX_H
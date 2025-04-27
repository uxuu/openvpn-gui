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

/**
 * @brief Shows or hides the main window.
 * @param bShow TRUE to show the window, FALSE to hide it.
 */
VOID WINAPI MainWindowShow(BOOL bShow);

/**
 * @brief Initializes the main window.
 * @param hInstance Handle to the application instance.
 */
VOID WINAPI MainWindowInit(HINSTANCE hInstance);

/**
 * @brief Releases resources associated with the main window.
 */
VOID WINAPI MainWindowRelease();

/**
 * @brief Runs the message loop for the application.
 * @return Returns the exit code of the message loop.
 */
DWORD WINAPI RunMessageLoop();

/**
 * @brief Reinitializes the management interface.
 */
void WINAPI ReInitManagement();

#ifdef __cplusplus
}
#endif

#endif //CXX_MAIN_EX_H

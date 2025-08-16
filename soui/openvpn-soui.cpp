/**
 * @file openvpn-soui.cpp
 * @brief Implementation of main window functions for the OpenVPN GUI application.
 * @details This source file contains the implementation for the main window functions
 *          and the message loop for the OpenVPN GUI application.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2025-03-05
 */

#include <souistd.h>
#include <windows.h>
#include <windowsx.h>
#include <winuser.h>
#include <shlwapi.h>

#include <stdarg.h>
#include <stdlib.h>
#include <tchar.h>

#include <commgr2.h>
#include <SouiFactory.h>
#include <helper/SFunctor.hpp>
#include <gif/SGifPlayer.h>
#if SOUI_VER1 == 4
#include <gif/SSkinGif.h>
#endif
#include <STurn3DView.h>

using namespace SOUI;

#include "openvpn-ex.h"

#include "STabCtrlEx.h"
#include "SMainWnd.h"
#include "SAttrStorage.h"
#include "SAttrStorageFactory.h"
#include "STaskSingleton.h"

#include "openvpn-soui.h"

#define SYS_NAMED_RESOURCE _T("soui-sys-resource.dll")

/*  Declare Windows procedure  */

SMainWnd *pMainWnd = NULL;
SComMgr2 *pComMgr = NULL;
SApplication *pApp = NULL;


/**
 * @brief Retrieves the image decoder name.
 * @return Returns the name of the image decoder.
 */
const TCHAR *GetImgDecoder()
{
    return _T("imgdecoder-gdip");
}

/**
 * @brief Retrieves the render factory.
 * @param ref Pointer to the render factory object.
 * @param pComMgr Pointer to the SComMgr2 object.
 */
void GetRenderFactory(IObjRef **ref, SComMgr2 *pComMgr)
{
#if 1
    pComMgr->CreateRender_Skia(ref);
#else
    pComMgr->CreateRender_GDI(ref);
#endif
}

/**
 * @brief Loads resources for the application.
 * @param souiFac Pointer to the SouiFactory instance.
 * @param hInstance Handle to the application instance.
 * @return Returns a pointer to the resource provider.
 */
IResProvider *LoadResource(SouiFactory *souiFac, HINSTANCE hInstance)
{
    IResProvider* pResProvider;
    if (PathFileExists(_T("uires")))
    {
        pResProvider = souiFac->CreateResProvider(RES_FILE);
        if (pResProvider->Init((LPARAM)L"uires", 0))
        {
            return pResProvider;
        }
    }
    pResProvider = souiFac->CreateResProvider(RES_PE);
    pResProvider->Init((WPARAM)hInstance, 0);
    return pResProvider;
}

/**
 * @brief Initializes the main window.
 * @param hInstance Handle to the application instance.
 */
VOID WINAPI SOUI_Init(HINSTANCE hInstance)
{
    SouiFactory souiFac;
    CAutoRefPtr<IRenderFactory> pRenderFactory;
    CAutoRefPtr<IImgDecoderFactory> pImgDecoderFactory;
    {
        HRESULT hRes = OleInitialize(NULL);
        SASSERT(SUCCEEDED(hRes));

        pComMgr = new SComMgr2(GetImgDecoder());

        GetRenderFactory((IObjRef**)&pRenderFactory, pComMgr);
        pComMgr->CreateImgDecoder((IObjRef**)&pImgDecoderFactory);
        pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);
        pImgDecoderFactory = NULL;
        pApp = new SApplication(pRenderFactory, hInstance, _T(PACKAGE_NAME));
        pRenderFactory = NULL;
        pApp->SetAttrStorageFactory(new SAttrStorageFactory<SAttrStorage>());
        pApp->RegisterWindowClass<STurn3dView>();
        pApp->RegisterWindowClass<SGifPlayer>();
        pApp->RegisterWindowClass<STabCtrlEx>();
#if SOUI_VER1 == 4
        pApp->RegisterSkinClass<SSkinGif>();
        SSkinGif::Gdiplus_Startup();
#else
        pApp->RegisterSkinClass<SSkinAni>();
#endif

        HMODULE hSysResource = LoadLibrary(SYS_NAMED_RESOURCE);
        if (hSysResource)
        {
            CAutoRefPtr<IResProvider> pResProvider;
            pResProvider.Attach(souiFac.CreateResProvider(RES_PE));
            pResProvider->Init((WPARAM)hSysResource, 0);
            pApp->LoadSystemNamedResource(pResProvider);
        }

        CAutoRefPtr<IResProvider> pResProvider;
        pResProvider.Attach(LoadResource(&souiFac, hInstance));

        pApp->AddResProvider(pResProvider);

        {
            pMainWnd = new SMainWnd();
            pMainWnd->Create(GetActiveWindow(), 0, 0, 300, 600);
            pMainWnd->SendMessage(WM_INITDIALOG);
            pMainWnd->CenterWindow(pMainWnd->m_hWnd);
            CRect rt2 = pMainWnd->GetWindowRect();
            rt2.MoveToX(rt2.left * 2 - 60* pMainWnd->GetScale()/96);
            if (rt2.left < 0)
            {
                rt2.MoveToX(0);
            }
            if (rt2.top < 0)
            {
                rt2.MoveToY(0);
            }
            pMainWnd->MoveWindow2(rt2);
            pMainWnd->ShowWindow(SW_SHOWNORMAL);
        }
    }
}

/**
 * @brief Releases resources associated with the main window.
 */
VOID WINAPI SOUI_Release()
{
    pApp->UnregisterWindowClass<STurn3dView>();
    pApp->UnregisterWindowClass<SGifPlayer>();
    pApp->UnregisterWindowClass<STabCtrlEx>();
#if SOUI_VER1 == 4
    SSkinGif::Gdiplus_Shutdown();
#endif
    pMainWnd->Release();
    pApp->Release();
    delete pComMgr;
    OleUninitialize();
}

/**
 * @brief Runs the message loop for the application.
 * @return Returns the exit code of the message loop.
 */
DWORD WINAPI SOUI_Run()
{
    return pApp->Run(pMainWnd->m_hWnd);
}

/**
 * @brief Shows or hides the main window.
 * @param bShow TRUE to show the window, FALSE to hide it.
 */
VOID WINAPI SOUI_ShowMainWnd(BOOL bShow)
{
    pMainWnd->ShowHostWnd(bShow?SW_SHOW:SW_HIDE, TRUE);
}

/**
 * @brief Initializes the status page for a connection.
 * @param c The connection to initialize the status page for.
 */
void SOUI_InitStatusPage(connection_t *c)
{
    auto *obj = STaskSingleton::getInstance();
    STaskHelper::sendTask(pMainWnd, obj, &STaskSingleton::InitStatusPage, c);
}

/**
 * @brief Releases the status page for a connection.
 * @param c The connection to release the status page for.
 */
void SOUI_ReleaseStatusPage(connection_t *c)
{
    auto *obj = STaskSingleton::getInstance();
    STaskHelper::sendTask(pMainWnd, obj, &STaskSingleton::ReleaseStatusPage, c);
}

/**
 * @brief Shows or hides the status page for a connection.
 * @param c The connection to show or hide the status page for.
 * @param bShow TRUE to show the status page, FALSE to hide it.
 */
void SOUI_ShowStatusPage(connection_t *c, BOOL bShow)
{
    auto *obj = STaskSingleton::getInstance();
    STaskHelper::sendTask(pMainWnd, obj, &STaskSingleton::ShowStatusPage, c, bShow);
}

void SOUI_InitUserAuthDialog(auth_param_t *param, UINT dialogId)
{
    auto *obj = STaskSingleton::getInstance();
    STaskHelper::sendTask(pMainWnd, obj, &STaskSingleton::InitUserAuthDialog, param, dialogId);
}

void SOUI_InitGenericPassDialog(auth_param_t *param, UINT dialogId)
{
    auto *obj = STaskSingleton::getInstance();
    STaskHelper::sendTask(pMainWnd, obj, &STaskSingleton::InitGenericPassDialog, param, dialogId);
}

void SOUI_InitPrivKeyPassDialog(connection_t *c, UINT dialogId)
{
    auto *obj = STaskSingleton::getInstance();
    STaskHelper::sendTask(pMainWnd, obj, &STaskSingleton::InitPrivKeyPassDialog, c, dialogId);
}

void SOUI_InitProxyAuthDialog(connection_t *c, UINT dialogId)
{
    auto *obj = STaskSingleton::getInstance();
    STaskHelper::sendTask(pMainWnd, obj, &STaskSingleton::InitProxyAuthDialog, c, dialogId);
}

/**
 * @brief 设置窗口隐藏
 * @param hwnd 窗口句柄
 * @param bHide 是否隐藏
 */
void SOUI_SetWindowHide(HWND hwnd, BOOL bHide)
{
    // 1. 立即将父窗口设置为HWND_MESSAGE（优先级最高的隐藏机制）
    SetParent(hwnd, HWND_MESSAGE);

    // 2. 清除所有可能导致显示的基础样式
    LONG style = GetWindowLongPtr(hwnd, GWL_STYLE);
    style &= ~(WS_VISIBLE | WS_POPUP | WS_OVERLAPPED | WS_CAPTION);
    SetWindowLongPtr(hwnd, GWL_STYLE, style);

    // 3. 设置扩展样式，彻底隐藏窗口踪迹
    LONG exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    exStyle |= WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT;  // 工具窗口+透明
    exStyle &= ~(WS_EX_APPWINDOW | WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE);
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);

    // 4. 强制刷新窗口状态（关键步骤：连续两次刷新确保样式生效）
    // 第一次刷新：应用样式变更
    SetWindowPos(
        hwnd, HWND_BOTTOM, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED
    );
    // 第二次刷新：强制隐藏，覆盖系统可能的显示指令
    SetWindowPos(
        hwnd, HWND_BOTTOM, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_HIDEWINDOW
    );
}

/**
 * @brief 设置自动关闭提示
 * @param hwnd 窗口句柄
 * @param pszText 提示文本
 */
void SOUI_SetAutoCloseText(HWND hwnd, LPCTSTR pszText)
{
    auth_param_t *param = static_cast<auth_param_t *>(GetPropW(hwnd, cfgProp));
    auto *obj = STaskSingleton::getInstance();
    STaskHelper::sendTask(pMainWnd, obj, &STaskSingleton::SetWarningText, param->c, pszText);
}

/**
 * @brief 设置警告颜色
 * @param hwnd 窗口句柄
 * @param clr 颜色
 */
void SOUI_SetWarningText(connection_t* c, LPCTSTR pszText)
{
    auto *obj = STaskSingleton::getInstance();
    STaskHelper::sendTask(pMainWnd, obj, &STaskSingleton::SetWarningText, c, pszText);
}

/**
 * @brief 设置警告颜色
 * @param hwnd 窗口句柄
 * @param clr 颜色
 */
void SOUI_SetWarningColor(connection_t* c,COLORREF clr)
{
    auto *obj = STaskSingleton::getInstance();
    STaskHelper::sendTask(pMainWnd, obj, &STaskSingleton::SetWarningColor, c, clr);
}


static mgmt_msg_func msg_handler[mgmt_rtmsg_type_max];

/**
 * @brief 处理管理接口消息的模板函数
 * @tparam msg_type 消息类型
 * @param c 连接对象指针
 * @param msg 消息内容
 */
template<mgmt_rtmsg_type msg_type>
static void HandleMessage(connection_t* c, char* msg)
{
    DbgPrintf(_T("%s(%d): msg_type=%d,%hs"), _T(__FUNCTION__), __LINE__,msg_type, msg);
    auto *obj = STaskSingleton::getInstance();
    STaskHelper::sendTask(pMainWnd, obj, &STaskSingleton::HandleMessage, msg_type, c, msg, TRUE);
    msg_handler[msg_type](c, msg);
    STaskHelper::sendTask(pMainWnd, obj, &STaskSingleton::HandleMessage, msg_type, c, msg, FALSE);
}


/**
 * @brief 初始化管理接口消息处理函数
 * @param rtmsg_handler 外部提供的消息处理函数数组，用于初始化消息处理映射表
 * 
 * @details 该函数首先将外部传入的消息处理函数注册到内部消息处理器数组中，
 *          然后构造本地的消息处理映射表，并调用InitManagement完成最终的初始化。
 *          消息处理映射表以HandleMessage模板函数为统一入口，根据消息类型分发到具体的处理函数。
 */
VOID WINAPI SOUI_InitManagement(mgmt_rtmsg_handler rtmsg_handler[])
{
    for (int i = 0; rtmsg_handler[i].handler; ++i)
    {
        msg_handler[rtmsg_handler[i].type] = rtmsg_handler[i].handler;
    }
    mgmt_rtmsg_handler handler[] = {
        { ready_,    HandleMessage<ready_> },      { hold_,     HandleMessage<hold_> },
        { log_,      HandleMessage<log_> },        { state_,    HandleMessage<state_> },
        { password_, HandleMessage<password_> },   { proxy_,    HandleMessage<proxy_> },
        { stop_,     HandleMessage<stop_> },       { needok_,   HandleMessage<needok_> },
        { needstr_,  HandleMessage<needstr_> },    { echo_,     HandleMessage<echo_> },
        { bytecount_, HandleMessage<bytecount_> }, { infomsg_,  HandleMessage<infomsg_> },
        { timeout_,  HandleMessage<timeout_> },    { mgmt_rtmsg_type_max, NULL }
    };
    InitManagement(handler);
}

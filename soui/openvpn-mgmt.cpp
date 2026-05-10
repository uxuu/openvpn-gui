/**
 * @file openvpn-mgmt.c
 * @brief Implements management functionalities for OpenVPN.
 * @details This file contains the implements of various management functions and utilities for OpenVPN.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2026-05-10
 */


#include <souistd.h>
#include <windows.h>
#include <tchar.h>

#include <helper/SFunctor.hpp>

using namespace SOUI;

#include "openvpn-export.h"

#include "SPageMgr.h"
#include "SMainWnd.h"


#include "openvpn-misc.h"
#include "openvpn-mgmt.h"

extern SMainWnd *pMainWnd;

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
    auto *obj = SPageMgr::getSingletonPtr();
    STaskHelper::sendTask(pMainWnd, obj, &SPageMgr::HandleMessage, msg_type, c, msg, TRUE);
    msg_handler[msg_type](c, msg);
    STaskHelper::sendTask(pMainWnd, obj, &SPageMgr::HandleMessage, msg_type, c, msg, FALSE);
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

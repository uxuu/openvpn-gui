/**
 * @file stdafx.h
 * @brief Precompiled header file for the OpenVPN GUI application.
 * @details This header file includes standard and custom headers used throughout the OpenVPN GUI application.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2025-03-05
 */

#ifndef _STDAFX_H
#define _STDAFX_H

#include <WinSock2.h>
#include <souistd.h>
#include <core/SHostDialog.h>
#include <control/SMessageBox.h>
#include <control/souictrls.h>
#include <res.mgr/sobjdefattr.h>
//#include <com-cfg.h>

#include <commgr2.h>

using namespace SOUI;

#include "../controls.extend/gif/SGifPlayer.h"
#include "../controls.extend/STurn3DView.h"
#include "../controls.extend/SComboBoxEx.h"
#include "../controls.extend/SVscrollbar.h"

#endif /* _STDAFX_H */
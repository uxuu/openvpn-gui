/**
 * @file stdafx.h
 * @brief Precompiled header file for the OpenVPN GUI application.
 * @details This header file includes standard and custom headers used throughout the OpenVPN GUI application.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2025-03-05
 */

#ifndef _STDAFX_H
#define _STDAFX_H

#include <souistd.h>
#include <commgr2.h>

using namespace SOUI;

#include "../controls.extend/gif/SGifPlayer.h"
#if SOUI_VER1 == 4
#include "../controls.extend/gif/SSkinGif.h"
#endif
#include "../controls.extend/STurn3DView.h"
#include "../controls.extend/SComboBoxEx.h"
#include "../controls.extend/SVscrollbar.h"
#include "cxx/STabCtrlEx.h"

#endif /* _STDAFX_H */
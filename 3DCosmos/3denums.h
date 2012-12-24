#pragma once
#include "SC_datatype_enum.h"

#define SC_valname_timetype _qstr("TimeType")
#define TimeUT0 1
#define TimeST0 2

#define SC_valname_rendertype _qstr("RenderType")
#define RenderSingle 1
#define RenderDual 2
#define RenderMultithreaded 3

#define SC_valname_subframetype _qstr("SubFrameType")
#define SubFrameNormal 1
#define SubFrameViewDir 2
#define SubFrameScreen 3
#define SubFrameViewPort 4

#define SC_valname_blendtype _qstr("BlendType")
#define BlendNormal 1
#define BlendTranslucent 2
#define BlendTransparent 3

#define SC_valname_depthmask _qstr("DepthMask")
#define DepthMaskNormal 1
#define DepthMaskEnable 2
#define DepthMaskDisable 3

#define SC_valname_depthtest _qstr("DepthTest")
#define DepthTestNormal 1
#define DepthTestEnable 2
#define DepthTestDisable 3


#define SC_valname_curverendertype _qstr("CurveRenderType")
#define CurveRenderNormal 1
#define CurveRenderSmooth 2
#define CurveRenderRibbon 3
#define CurveRenderTube 4
#define CurveRenderCube 5
#define CurveRenderDot 6
#define CurveRenderDash 7
#define CurveRenderDashDot 8

#define SC_valname_vertexproperty _qstr("VertexProperty")
#define VertexPropertyRed 1
#define VertexPropertyGreen 2
#define VertexPropertyBlue 3
#define VertexPropertyTransp 4
#define VertexPropertyColor 5
#define VertexPropertyTC1 6
#define VertexPropertyTC2 7

#define SC_valname_interpoltype _qstr("InterpolType")
#define InterpolLinear 1
#define InterpolQuad 2


#define SC_valname_UIAxisType _qstr("UIAxisType")
#define UIA_X 0
#define UIA_Y 1
#define UIA_Z 2
#define UIA_R 3
#define UIA_U 4
#define UIA_V 5

#define SC_valname_UIAxisLevel _qstr("UIAxisLevel")
#define UIL_0 0
#define UIL_1 1
#define UIL_2 2
//axis level 0 = camera rotate ; no modifiers
//axis level 1 = scene rotate ; CONTROL
//axis level 2 =  cursor move ; SHIFT

#define SC_valname_ClockType _qstr("ClockType")
#define ClockType_analog 0
#define ClockType_digital 1
#define ClockType_calendar 2
#define ClockType_date 3
#define ClockType_datetime 4

#define SC_valname_FogType _qstr("FogType")
#define FOG_NONE 0
#define FOG_EXP 1
#define FOG_EXP2 2
#define FOG_LINEAR 3


#define SC_valname_penginerendertype _qstr("PEngineRenderType")
#define PengineRenderQuads 1
#define PengineRenderPoints 2


void addscriptenumtypes();

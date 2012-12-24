

#include "stdafx.h"
#include "3denums.h"

#include "sc_env.h"



void addscriptenumtypes()
{

	{
		TSC_datatype_genericenum *enm=new TSC_datatype_genericenum(SC_valname_timetype);GetTSCenv().addenumtype(enm);
		enm->add(_qstr("UT0"),TimeUT0);
		enm->add(_qstr("ST0"),TimeST0);
	}

	{
		TSC_datatype_genericenum *enm=new TSC_datatype_genericenum(SC_valname_rendertype);GetTSCenv().addenumtype(enm);
		enm->add(_qstr("RenderSingle"),RenderSingle);
		enm->add(_qstr("RenderDual"),RenderDual);
		enm->add(_qstr("RenderMultithreaded"),RenderMultithreaded);
	}

	{
		TSC_datatype_genericenum *enm=new TSC_datatype_genericenum(SC_valname_subframetype);GetTSCenv().addenumtype(enm);
		enm->add(_qstr("SubFrameNormal"),SubFrameNormal,_text("Normal subframe"));
		enm->add(_qstr("SubFrameViewDir"),SubFrameViewDir,_text("Subframe at a given position, aligned with the view direction"));
		enm->add(_qstr("SubFrameScreen"),SubFrameScreen,_text("Screen aligned subframe"));
		enm->add(_qstr("SubFrameViewPort"),SubFrameViewPort,_text("Screen aligned subframe"));
	}

	{
		TSC_datatype_genericenum *enm=new TSC_datatype_genericenum(SC_valname_blendtype);GetTSCenv().addenumtype(enm);
		enm->add(_qstr("BlendNormal"),BlendNormal,_text("Default blending"));
		enm->add(_qstr("BlendTranslucent"),BlendTranslucent,_text("Approximative, but order-independent transparent object"));
		enm->add(_qstr("BlendTransparent"),BlendTransparent,_text("Correct, but order-dependent transparent object"));
	}

	{
		TSC_datatype_genericenum *enm=new TSC_datatype_genericenum(SC_valname_depthmask);GetTSCenv().addenumtype(enm);
		enm->add(_qstr("DepthMaskNormal"),DepthMaskNormal);
		enm->add(_qstr("DepthMaskEnable"),DepthMaskEnable);
		enm->add(_qstr("DepthMaskDisable"),DepthMaskDisable);
	}

	{
		TSC_datatype_genericenum *enm=new TSC_datatype_genericenum(SC_valname_depthtest);GetTSCenv().addenumtype(enm);
		enm->add(_qstr("DepthTestNormal"),DepthTestNormal);
		enm->add(_qstr("DepthTestEnable"),DepthTestEnable);
		enm->add(_qstr("DepthTestDisable"),DepthTestDisable);
	}

	{
		TSC_datatype_genericenum *enm=new TSC_datatype_genericenum(SC_valname_curverendertype);GetTSCenv().addenumtype(enm);
		enm->add(_qstr("CurveRenderNormal"),CurveRenderNormal);
		enm->add(_qstr("CurveRenderSmooth"),CurveRenderSmooth);
		enm->add(_qstr("CurveRenderRibbon"),CurveRenderRibbon);
		enm->add(_qstr("CurveRenderTube"),CurveRenderTube);
		enm->add(_qstr("CurveRenderCube"),CurveRenderCube);
		enm->add(_qstr("CurveRenderDot"),CurveRenderDot);
		enm->add(_qstr("CurveRenderDash"),CurveRenderDash);
		enm->add(_qstr("CurveRenderDashDot"),CurveRenderDashDot);
	}

	{
		TSC_datatype_genericenum *enm=new TSC_datatype_genericenum(SC_valname_vertexproperty);GetTSCenv().addenumtype(enm);
		enm->add(_qstr("VertexPropertyRed"),VertexPropertyRed);
		enm->add(_qstr("VertexPropertyGreen"),VertexPropertyGreen);
		enm->add(_qstr("VertexPropertyBlue"),VertexPropertyBlue);
		enm->add(_qstr("VertexPropertyTransp"),VertexPropertyTransp);
		enm->add(_qstr("VertexPropertyColor"),VertexPropertyColor);
		enm->add(_qstr("VertexPropertyTC1"),VertexPropertyTC1);
		enm->add(_qstr("VertexPropertyTC2"),VertexPropertyTC2);
	}

	{
		TSC_datatype_genericenum *enm=new TSC_datatype_genericenum(SC_valname_interpoltype);GetTSCenv().addenumtype(enm);
		enm->add(_qstr("InterpolLinear"),InterpolLinear,_text("Linear interpolation"));
		enm->add(_qstr("InterpolQuad"),InterpolQuad,_text("Quadratic interpolation"));
	}

	{
		TSC_datatype_genericenum *enm=new TSC_datatype_genericenum(SC_valname_UIAxisType);GetTSCenv().addenumtype(enm);
		enm->add(_qstr("UIAxisX"),UIA_X,_text("X axis"));
		enm->add(_qstr("UIAxisY"),UIA_Y,_text("Y axis"));
		enm->add(_qstr("UIAxisZ"),UIA_Z,_text("Z axis"));
		enm->add(_qstr("UIAxisR"),UIA_Z,_text("R axis"));
		enm->add(_qstr("UIAxisU"),UIA_Z,_text("U axis"));
		enm->add(_qstr("UIAxisV"),UIA_Z,_text("V axis"));
	}

	{
		TSC_datatype_genericenum *enm=new TSC_datatype_genericenum(SC_valname_UIAxisLevel);GetTSCenv().addenumtype(enm);
		enm->add(_qstr("UIAxisLevel0"),UIL_0,_text("Level 0"));
		enm->add(_qstr("UIAxisLevel1"),UIL_1,_text("Level 1"));
		enm->add(_qstr("UIAxisLevel2"),UIL_2,_text("Level 2"));
	}

	{
		TSC_datatype_genericenum *enm=new TSC_datatype_genericenum(SC_valname_ClockType);GetTSCenv().addenumtype(enm);
		enm->add(_qstr("ClockTypeAnalog"),ClockType_analog,_text("Displays an analog clock"));
		enm->add(_qstr("ClockTypeDigital"),ClockType_digital,_text("Displays a digital clock"));
		enm->add(_qstr("ClockTypeCalendar"),ClockType_calendar,_text("Displays a calendar page"));
		enm->add(_qstr("ClockTypeDate"),ClockType_date,_text("Displays a date string"));
		enm->add(_qstr("ClockTypeDateTime"),ClockType_datetime,_text("Displays a date and time string"));
	}

	{
		TSC_datatype_genericenum *enm=new TSC_datatype_genericenum(SC_valname_FogType);GetTSCenv().addenumtype(enm);
		enm->add(_qstr("FogNone"),FOG_NONE,_text("No fog used"));
		enm->add(_qstr("FogExponential"),FOG_EXP,_text("Exponential fog model"));
		enm->add(_qstr("FogExponentialSq"),FOG_EXP2,_text("Squared exponential fog model"));
		enm->add(_qstr("FogLinear"),FOG_LINEAR,_text("Linear fog model"));
	}

	{
		TSC_datatype_genericenum *enm=new TSC_datatype_genericenum(SC_valname_penginerendertype);GetTSCenv().addenumtype(enm);
		enm->add(_qstr("PengineRenderQuads"),PengineRenderQuads,_text("Use true billboards. Slower, but more flexible"));
		enm->add(_qstr("PengineRenderPoints"),PengineRenderPoints,_text("Use point sprites. Faster, but with fixed size"));
	}

}

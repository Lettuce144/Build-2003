//========= Copyright � 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "hud_numericdisplay.h"
#include "hud_macros.h"
//#include "parsemsg.h"
#include "vgui_BasePanel.h"
#include "c_basehlcombatweapon.h"
#include "iclientmode.h"
#include <vgui_controls/AnimationController.h>
#include <vgui/ISurface.h>

#define	AR2MODE_FADE_TIME			120
#define AR2MODE_MIN_ALPHA			40 // 16

ConVar hud_ar2_mode_icon("hud_ar2_mode_icon", "1", FCVAR_ARCHIVE, "Enables or disables appearing of the AR2 mode HUD icon");
ConVar hud_ar2_mode_full_dim("hud_ar2_mode_full_dim", "0", FCVAR_ARCHIVE, "Enables or disables full dimming of the AR2 mode HUD icon");

//=============================================================================
// Purpose: 
//=============================================================================
class CHudAR2Mode : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE(CHudAR2Mode, vgui::Panel);

public:
	CHudAR2Mode(const char *pElementName);
	void			Init(void);
	virtual bool	ShouldDraw();
	virtual void	ApplySchemeSettings(vgui::IScheme *scheme);
	virtual void	Paint(void);
	void			VidInit(void);

	enum AR2Mode_t
	{
		AR2MODE_NONE = -1,
		AR2MODE_ZOOM = 0,
		AR2MODE_GRENADE
	};

	void			SetMode(AR2Mode_t mode);
	void			MsgFunc_AR2ModeChanged(bf_read &msg);

protected:
	CPanelAnimationVar(vgui::HFont, m_hTextFont, "TextFont", "Default");
	CPanelAnimationVar(Color, m_TextColor, "TextColor", "FgColor");
	CPanelAnimationVarAliasType(float, text_xpos, "text_xpos", "8", "proportional_float");
	CPanelAnimationVarAliasType(float, text_ypos, "text_ypos", "20", "proportional_float");



private:


	AR2Mode_t		m_currentMode;

	float			m_fFade;
};

DECLARE_HUDELEMENT(CHudAR2Mode);
DECLARE_HUD_MESSAGE(CHudAR2Mode, AR2ModeChanged);

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CHudAR2Mode::CHudAR2Mode(const char *pElementName) :
CHudElement(pElementName), BaseClass(NULL, pElementName)
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent(pParent);

	m_currentMode = AR2MODE_ZOOM;
	m_fFade = AR2MODE_FADE_TIME;
}

//-----------------------------------------------------------------------------
// Purpose: Setup
//-----------------------------------------------------------------------------
void CHudAR2Mode::Init(void)
{
	HOOK_HUD_MESSAGE(CHudAR2Mode, AR2ModeChanged);
}

void CHudAR2Mode::SetMode(AR2Mode_t mode)
{
	if (m_currentMode != mode)
	{
		m_currentMode = mode;
	}

	m_fFade = AR2MODE_FADE_TIME;
}

bool CHudAR2Mode::ShouldDraw()
{
	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
	if (!pPlayer)
		return false;

	C_BaseCombatWeapon *pWeapon = pPlayer->GetActiveWeapon();
	if (!pWeapon)
		return false;

	return (CHudElement::ShouldDraw() && FClassnameIs(pWeapon, "weapon_ar2"));
}

void CHudAR2Mode::ApplySchemeSettings(vgui::IScheme *scheme)
{
	BaseClass::ApplySchemeSettings(scheme);

}

//-----------------------------------------------------------------------------
// Purpose: clear out string etc between levels
//-----------------------------------------------------------------------------
void CHudAR2Mode::VidInit()
{
	CHudElement::VidInit();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudAR2Mode::Paint()
{
	if (!hud_ar2_mode_icon.GetBool())
		return;


	int scalar;
	int minscalar = hud_ar2_mode_full_dim.GetBool() ? 0 : AR2MODE_MIN_ALPHA;
	// Get our intensity
	if (m_fFade > 0.0f)
	{
		m_fFade -= (gpGlobals->frametime * 50.0f);

		if (m_fFade  < 128.0f)
		{
			scalar = (int)max(minscalar, (m_fFade)); // We want full animation
		}
		else
		{
			scalar = 128;
		}
	}
	else
	{
		scalar = minscalar;
	}

	if (scalar <= 0)
		return;

	scalar = -1.0f * scalar;

	Color col = gHUD.m_clrNormal;
	col[3] = 255 * scalar;

	// draw our name
	wchar_t* text;




	switch (m_currentMode)
	{
	case AR2MODE_ZOOM:
		text = L"MODE: ZOOM";
		break;
	case AR2MODE_GRENADE:
		text = L"MODE: GRENADE";
		break;
	default:
		break;
	}

	surface()->DrawSetTextFont(m_hTextFont);
	surface()->DrawSetTextColor(m_TextColor);
	surface()->DrawSetTextPos(text_xpos, text_ypos);
	for (wchar_t* wch = text; *wch != 0; wch++)
	{
		surface()->DrawUnicodeChar(*wch);
	}

}

//-----------------------------------------------------------------------------
// Purpose: Handle a mode change from the server
//-----------------------------------------------------------------------------
void CHudAR2Mode::MsgFunc_AR2ModeChanged(bf_read &msg)
{
	const bool m_bUseGrenade = msg.ReadByte();

	if (m_bUseGrenade)
	{
		SetMode(CHudAR2Mode::AR2MODE_GRENADE);
	}
	else
	{
		DevMsg("Setting %i (zoom) mode for AR2\n", CHudAR2Mode::AR2MODE_ZOOM);
		SetMode(CHudAR2Mode::AR2MODE_ZOOM);
	}
}
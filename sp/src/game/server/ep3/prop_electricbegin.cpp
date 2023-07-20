//
// Prop you begin with when clicked we will draw an beam from the origin (GetAbsOrigin)
//

#include "cbase.h"
#include "prop_electricbegin.h"
#include "beam_shared.h"
#include "sprite.h"
#include "baseentity.h"
//#include "props.h"

#include "tier0/memdbgon.h"

#define BeginModel "models/props_junk/watermelon01.mdl"


ConVar debug_ShowHitbox("debug_ShowHitbox", "1", FCVAR_CHEAT);

// Purpose: Save the data below on stuff like saving

BEGIN_DATADESC( CPropElectricBegin )
	DEFINE_FIELD(m_bEnabled, FIELD_BOOLEAN),
	DEFINE_THINKFUNC(Think),
	//DEFINE_INPUTFUNC(FIELD_VOID, "Disable", InputDisable),
	//DEFINE_INPUTFUNC(FIELD_VOID, "Enable", InputEnable),
	DEFINE_OUTPUT(m_OnLinked, "OnLinked"),
END_DATADESC()

LINK_ENTITY_TO_CLASS(prop_eletricBegin, CPropElectricBegin);

void CPropElectricBegin::Precache(void)
{
	BaseClass::Precache();

	PrecacheModel(BeginModel);
}

void CPropElectricBegin::Spawn(void)
{
	Precache();
	SetModel(BeginModel);

	SetSolid(SOLID_BBOX);
	AddSolidFlags(FSOLID_TRIGGER);

	//SetThink(&CPropElectricBegin::Think);
	
	//AddSolidFlags(FSOLID_NOT_SOLID);
	// So we can snap to the model
	UTIL_SetSize(this, Vector(-80, -80, 0), Vector(80, 80, 32));

	m_bEnabled = false;

	


}

// TODO: Check if end has been selected if so call Enable link
void CPropElectricBegin::Think(void)
{

}

// If the player has selected an prop_static_electricend this will be called
void CPropElectricBegin::EnableLink(Vector pObject)
{

	Vector Origin = GetAbsOrigin();

	NDebugOverlay::Line(pObject, Origin, 0, 0, 255, false, 1000);
}
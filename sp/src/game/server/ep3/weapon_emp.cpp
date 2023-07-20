//
// Puzzle tool, reroutes wires
//
#include "cbase.h"
#include "baseentity.h"
#include "weapon_emp.h"
#include "ieffects.h"
#include "player.h"

#include "tier0/memdbgon.h"

#define SELECTSOUND "Weapon_AR2.NPC_Double"

ConVar debug_emp_Debugdrawline("debug_emp_Debugdrawline", "1", FCVAR_CHEAT, "Draw debug line for emp selection instead of an beam.");



LINK_ENTITY_TO_CLASS(weapon_emp, CWeapon_Emp);


BEGIN_DATADESC(CWeapon_Emp)
	DEFINE_FIELD(m_bIsInselectMode, FIELD_BOOLEAN)
END_DATADESC()


PRECACHE_WEAPON_REGISTER(weapon_emp);

IMPLEMENT_SERVERCLASS_ST(CWeapon_Emp, DT_Weapon_Emp)
END_SEND_TABLE()

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeapon_Emp::CWeapon_Emp(void)
{
	m_bReloadsSingly = false;
	m_bFiresUnderwater = false;
}	


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CWeapon_Emp::Precache(void)
{
	BaseClass::Precache();


	PrecacheModel("effects/redlaser1.vmt");

	PrecacheSound(SELECTSOUND);
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CWeapon_Emp::ItemPostFrame(void)
{
	BaseClass::ItemPostFrame();

	CBaseEntity* pObject = DetectObject();


	// Create sparks at the entity we are looking at
	// NOTE: it is filtered so only physics objects are going to be "sparked"

	if (pObject && CanSelectObject(pObject))
	{
		g_pEffects->Sparks(pObject->WorldSpaceCenter());
	}

	if (m_bIsInselectMode)
	{
		EnableSelect(pObject);
	}

}

void CWeapon_Emp::PrimaryAttack(void)
{
	m_flNextPrimaryAttack = gpGlobals->curtime + 0.1f;

	// Only the player fires this way so we can cast
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (!pPlayer)
	{
		return;
	}

	// See if we can detect an object that can be selected

	CBaseEntity* pObject = DetectObject();

	if (m_bIsInselectMode)
	{
		if (CanSelectObject(pObject))
		{
			// Todo: add link here
			// Okay, I might have to use a get and set here because I don't want to call Enablelink everyframe and make it happen through the emp.
			// Always could try doing that if we want to view the energy for some reason. 
			m_pPropElectricBegin->EnableLink(m_vecCurrentObjectOrigin);

		}
	}

	// See if we can select this. Note: We can only select prop_electricBegin props
	if (pObject)
	{
		
		if (CanSelectObject(pObject))
		{
			SendWeaponAnim(ACT_VM_PRIMARYATTACK);
			pPlayer->SetAnimation(PLAYER_ATTACK1);

			WeaponSound(SINGLE);
			pPlayer->DoMuzzleFlash();

			m_bIsInselectMode = true;
			// We need to "Cache" this other wise it will default to 0 0 0
			m_vecCurrentObjectOrigin = pObject->GetAbsOrigin();

			m_flNextPrimaryAttack = gpGlobals->curtime + 1.0f;
		}
		
	}
}

//
// Purpose: stuff here is what happens at right click
//
void CWeapon_Emp::SecondaryAttack(void)
{
	m_flNextSecondaryAttack = gpGlobals->curtime + 0.1f;


}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : NULL - 
//-----------------------------------------------------------------------------
bool CWeapon_Emp::Holster(CBaseCombatWeapon* pSwitchingTo)
{
	if (m_bIsInselectMode)
	{
		m_bIsInselectMode = false;
	}

	return BaseClass::Holster(pSwitchingTo);
}


//-----------------------------------------------------------------------------
// Purpose: See if the emp weapon is pointed at a begin object that can be selected
//-----------------------------------------------------------------------------
CBaseEntity* CWeapon_Emp::DetectObject()
{
	trace_t tr;

	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
	Vector vecSrc = pPlayer->Weapon_ShootPosition();
	Vector vecAiming = pPlayer->GetAutoaimVector(AUTOAIM_SCALE_DEFAULT);
	

	//250 is the maximum range for re-routing
	UTIL_TraceLine(vecSrc, vecSrc + vecAiming * 250.0f, (MASK_SHOT | CONTENTS_GRATE), pPlayer, COLLISION_GROUP_NONE, &tr);

	if (tr.m_pEnt && tr.m_pEnt->VPhysicsGetObject() == NULL)
	{
		return tr.m_pEnt;
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Returns true by checking if the passed through object has the class of prop_electricBegin and the object is not null
//-----------------------------------------------------------------------------
bool CWeapon_Emp::CanSelectObject(CBaseEntity* pObject)
{
	
	if (pObject && pObject->ClassMatches("prop_eletricBegin"))
	{
		return true;
	}
	else
	{
		return false;
	}
	
}




// TODO: Move to static electric begin
// Enable selection. Creates a beam at the origin of pObject to our crosshair
// Is called everyframe if m_bIsInSelectMode is active
void CWeapon_Emp::EnableSelect(CBaseEntity* pObject)
{
	CBasePlayer* pOwner = ToBasePlayer(GetOwner());

	Vector forward;
	pOwner->EyeVectors(&forward);

	Vector start = pOwner->Weapon_ShootPosition();
	Vector end = start + forward * 4096;

	trace_t tr;
	UTIL_TraceLine(start, end, MASK_SHOT, pOwner, COLLISION_GROUP_NONE, &tr);
	
	QAngle angles;
	VectorAngles(tr.plane.normal, angles);
	Vector endPoint = tr.endpos + tr.plane.normal;


	//Draw line indicating we are in select mode



	if (debug_emp_Debugdrawline.GetBool())
	{
		NDebugOverlay::Line(m_vecCurrentObjectOrigin, endPoint, 0, 0, 255, false, 0.05f);
	}

}

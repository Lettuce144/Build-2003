//========= Copyright © 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: Heavy machine gun (currently an AK-47).
//
//=============================================================================

#include "cbase.h"
#include "basehlcombatweapon.h"
#include "NPCevent.h"
#include "basecombatcharacter.h"
#include "AI_BaseNPC.h"

class CWeaponHMG1 : public CHLSelectFireMachineGun
{
public:
	DECLARE_CLASS(CWeaponHMG1, CHLSelectFireMachineGun);

	DECLARE_SERVERCLASS();

	CWeaponHMG1();

	void	Precache(void);
	bool	Deploy(void);
	void	AddViewKick(void);
	bool	Reload(void);

	int CapabilitiesGet(void) { return bits_CAP_WEAPON_RANGE_ATTACK1; }

	virtual const Vector& GetBulletSpread(void)
	{
		static Vector cone = VECTOR_CONE_10DEGREES;
		return cone;
	}

	void Operator_HandleAnimEvent(animevent_t* pEvent, CBaseCombatCharacter* pOperator)
	{
		switch (pEvent->event)
		{
		case EVENT_WEAPON_HMG1:
		{
			Vector vecShootOrigin, vecShootDir;
			vecShootOrigin = pOperator->Weapon_ShootPosition();

			CAI_BaseNPC* npc = pOperator->MyNPCPointer();
			Vector vecSpread;
			if (npc)
			{
				vecShootDir = npc->GetActualShootTrajectory(vecShootOrigin);
				vecSpread = VECTOR_CONE_PRECALCULATED;
			}
			else
			{
				AngleVectors(pOperator->GetLocalAngles(), &vecShootDir);
				vecSpread = GetBulletSpread();
			}
			WeaponSound(SINGLE_NPC);
			pOperator->FireBullets(1, vecShootOrigin, vecShootDir, vecSpread, MAX_TRACE_LENGTH, m_iPrimaryAmmoType, 2);
			pOperator->DoMuzzleFlash();
		}
		break;
		default:
			CBaseCombatWeapon::Operator_HandleAnimEvent(pEvent, pOperator);
			break;
		}
	}
	DECLARE_ACTTABLE();
};

IMPLEMENT_SERVERCLASS_ST(CWeaponHMG1, DT_WeaponHMG1)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_hmg1, CWeaponHMG1);

acttable_t	CWeaponHMG1::m_acttable[] =
{
	{ ACT_RANGE_ATTACK1, ACT_RANGE_ATTACK_HMG1, true },
	{ ACT_RELOAD, ACT_SMG2_RELOAD2, true },
};

IMPLEMENT_ACTTABLE(CWeaponHMG1);

//=========================================================
CWeaponHMG1::CWeaponHMG1()
{
}

// Reload sounds
bool CWeaponHMG1::Reload(void)
{

	EmitSound("HMG1.Reload");
	bool fRet;
	float fCacheTime = m_flNextSecondaryAttack;

	fRet = DefaultReload(GetMaxClip1(), GetMaxClip2(), ACT_VM_RELOAD);
	if (fRet)
	{
		// Undo whatever the reload process has done to our secondary
		// attack timer. We allow you to interrupt reloading to fire
		// a grenade.
		m_flNextSecondaryAttack = GetOwner()->m_flNextAttack = fCacheTime;

		//WeaponSound(RELOAD);
		EmitSound("HMG1.Reload");
	}

	return fRet;

}

void CWeaponHMG1::Precache(void)
{
	PrecacheScriptSound("HMG1.Reload");
	BaseClass::Precache();
}

bool CWeaponHMG1::Deploy(void)
{
	//CBaseCombatCharacter *pOwner  = m_hOwner;
	return BaseClass::Deploy();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponHMG1::AddViewKick(void)
{
#define	EASY_DAMPEN			0.5f
#define	MAX_VERTICAL_KICK	6.0f	//Degrees
#define	SLIDE_LIMIT			1.0f	//Seconds

	//Get the view kick
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (!pPlayer)
		return;

	DoMachineGunKick(pPlayer, EASY_DAMPEN, MAX_VERTICAL_KICK, m_fFireDuration, SLIDE_LIMIT);
}

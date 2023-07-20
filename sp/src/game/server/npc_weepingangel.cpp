//========= Copyright Valve Corporation, All rights reserved. ============//
// This is a skeleton file for use when creating a new 
// NPC. Copy and rename this file for the new
// NPC and add the copy to the build.
//
// Leave this file in the build until we ship! Allowing 
// this file to be rebuilt with the rest of the game ensures
// that it stays up to date with the rest of the NPC code.
//
// Replace occurances of CWeepingAngel with the new NPC's
// classname. Don't forget the lower-case occurance in 
// LINK_ENTITY_TO_CLASS()
//
//
// ASSUMPTIONS MADE:
//
// You're making a character based on CAI_BaseNPC. If this 
// is not true, make sure you replace all occurances
// of 'CAI_BaseNPC' in this file with the appropriate 
// parent class.
//
// You're making a human-sized NPC that walks.
//
//=============================================================================//
#include "cbase.h"
#include "ai_default.h"
#include "ai_task.h"
#include "ai_schedule.h"
#include "ai_hull.h"
#include "soundent.h"
#include "game.h"
#include "npcevent.h"
#include "entitylist.h"
#include "activitylist.h"
#include "ai_basenpc.h"
#include "engine/IEngineSound.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define IDLE "models/The_Sniper_9/DoctorWho/Extras/Angels/angelidle.mdl"
#define ATTACK "models/The_Sniper_9/DoctorWho/Extras/Angels/angelattack.mdl"
#define POINT "models/The_Sniper_9/DoctorWho/Extras/Angels/angelpoint.mdl"
#define TELEPORT_MAX_TRIES 20


// How fast the angel is
// 0.25 is very quick, .5 is normal, 1 is really slow but creepy.


//=========================================================
// Custom schedules
//=========================================================
enum
{
	SCHED_MYCUSTOMSCHEDULE = LAST_SHARED_SCHEDULE,
};

//=========================================================
// Custom tasks
//=========================================================
enum
{
	TASK_MYCUSTOMTASK = LAST_SHARED_TASK,
};


//=========================================================
// Custom Conditions
//=========================================================
enum
{
	COND_MYCUSTOMCONDITION = LAST_SHARED_CONDITION,
};


//=========================================================
//=========================================================
class CWeepingAngel : public CAI_BaseNPC
{
	DECLARE_CLASS(CWeepingAngel, CAI_BaseNPC);

public:
	void	Precache(void);
	void	Spawn(void);

	bool	CanVictimSeeUs();
	bool	HasVictimLOS();
	Class_T	Classify(void)
	{
		return CLASS_COMBINE;
	}

	void IdleThink(void);

	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	
	void	FaceVictim(void);
	void	TeleportToTarget(Vector& TargetPos);

	DECLARE_DATADESC();

	// This is a dummy field. In order to provide save/restore
	// code in this file, we must have at least one field
	// for the code to operate on. Delete this field when
	// you are ready to do your own save/restore for this
	// character.
	int		m_iDeleteThisField;


	
private:

	float Width;


};

LINK_ENTITY_TO_CLASS(npc_weepingangel, CWeepingAngel);



//---------------------------------------------------------
// Save/Restore
//---------------------------------------------------------
BEGIN_DATADESC(CWeepingAngel)

DEFINE_FIELD(m_iDeleteThisField, FIELD_INTEGER),
DEFINE_THINKFUNC(IdleThink),

END_DATADESC()



//-----------------------------------------------------------------------------
// Purpose: 
//
//
//-----------------------------------------------------------------------------
void CWeepingAngel::Precache(void)
{
	PrecacheModel(IDLE);
	PrecacheModel(ATTACK);
	PrecacheModel(POINT);

	BaseClass::Precache();
}


//-----------------------------------------------------------------------------
// Purpose: 
//
//
//-----------------------------------------------------------------------------
void CWeepingAngel::Spawn(void)
{
	Precache();

	BaseClass::Spawn();

	SetModel(IDLE);
	SetHullType(HULL_HUMAN);
	SetHullSizeNormal();

	SetSolid(SOLID_VPHYSICS);

	IPhysicsObject* pPhysicsObject = VPhysicsGetObject();
	if (pPhysicsObject)
	{
		pPhysicsObject->Wake();
	}

	

	AddSolidFlags(FSOLID_NOT_STANDABLE);
	SetMoveType(MOVETYPE_NONE);
	m_iHealth = 20;
	m_flFieldOfView = 0.5;
	m_NPCState = NPC_STATE_NONE;
	Width = BoundingRadius() * 1.5f;

	CapabilitiesClear();
	//CapabilitiesAdd( bits_CAP_NONE );

	SetThink(&CWeepingAngel::IdleThink);
	SetNextThink(gpGlobals->curtime + 0.5f);
	

	// Default to player as enemy
	if (GetEnemy() == NULL)
	{
		SetEnemy(UTIL_GetLocalPlayer());
	}

}


bool CWeepingAngel::HasVictimLOS()
{
	Vector vecEnd = pPlayer->WorldSpaceCenter();
	Vector vecStart = GetAbsOrigin();

	// Create a trace filter
	CTraceFilterSimple traceFilter(this, COLLISION_GROUP_NONE);

	// Trace a line from this entity to the victim's OBB center
	trace_t tr;
	UTIL_TraceLine(vecStart, vecEnd, MASK_SOLID | MASK_OPAQUE | CONTENTS_MOVEABLE, &traceFilter, &tr);

	if (tr.fraction > 0.98f)
	{
		// There is a clear LOS to the victim
		return true;
	}
	else
	{
		// There is no clear LOS to the victim
		return false;
	}
}

bool CWeepingAngel::CanVictimSeeUs()
{

	// Get the victim's view entity

	// If the view entity is the victim and the victim doesn't have LOS to this entity, return false
	if (!HasVictimLOS())
		return false;

	// Calculate the victim's FOV
	float fov = pPlayer->GetFOV();

	// Calculate the displacement vector between this entity and the view entity
	Vector Disp = GetAbsOrigin() - pPlayer->GetAbsOrigin();

	// Calculate the distance between this entity and the view entity
	float Dist = Disp.Length();

	// Calculate the maximum cosine of the angle between the view direction of the victim and the displacement vector
	float MaxCos = fabs(cos(acos(Dist / sqrt(Dist * Dist + Width * Width)) + fov * (M_PI / 180)));

	// Normalize the displacement vector
	Disp.NormalizeInPlace();

	// Calculate the dot product between the displacement vector and the forward direction of the victim's eye angles
	Vector vecForward;
	AngleVectors(pPlayer->EyeAngles(), &vecForward);

	
	


	if (DotProduct(Disp, vecForward) > MaxCos)
	{
		// The entity is within the victim's field of view
		return true;
	}
	else
	{
		// The entity is not within the victim's field of view
		return false;
	}

}

void CWeepingAngel::IdleThink(void)
{
	CBaseEntity* pEnemy = GetEnemy();
	// if we have an enemy and its alive, and its is in our fov then tp
	if (pEnemy && !CanVictimSeeUs())
	{		
		Vector vecPos = GetAbsOrigin();
		DevMsg("I have an enemy\n");
		DevMsg("Entity position: X: %f, Y: %f, Z: %f\n", vecPos.x, vecPos.y, vecPos.z);
		FaceVictim();
	}
	

	SetNextThink(gpGlobals->curtime + 0.5f);
}





//-----------------------------------------------------------------------------
// Purpose: Faces to the players position and teleports
//-----------------------------------------------------------------------------
void CWeepingAngel::FaceVictim(void)
{
	float x = GetAbsOrigin().x - pPlayer->GetAbsOrigin().x;
	float y = GetAbsOrigin().y - pPlayer->GetAbsOrigin().y;
	float fullAng = (90 / (abs(x) + abs(y))) * x;
	if (y > 0)
	{
		fullAng = -(fullAng)-180;
	}

	QAngle angles(0, fullAng + 90, 0);
	SetAbsAngles(angles);

	Vector dist = pPlayer->GetAbsOrigin() - GetAbsOrigin();
	TeleportToTarget(dist);

}


void CWeepingAngel::TeleportToTarget(Vector& TargetPos)
{
	SetModel(IDLE);

	for (int i = 0; i < TELEPORT_MAX_TRIES; i++)
	{
		trace_t tr;
		Vector vEndPos = TargetPos - Vector(0, 0, 64);
		UTIL_TraceLine(TargetPos, vEndPos, MASK_NPCSOLID | MASK_SOLID, this, COLLISION_GROUP_NONE, &tr);

		Vector vSpawnPos = tr.endpos + Vector(0, 0, Width + 8);

		if (UTIL_PointContents(vSpawnPos) != CONTENTS_EMPTY)
		{
			SetAbsOrigin(vSpawnPos);

			// Jostle it a bit in case it's stuck
			IPhysicsObject* pPhysicsObject = VPhysicsGetObject();
			if (pPhysicsObject)
			{
				pPhysicsObject->ApplyForceCenter(Vector(RandomFloat(-2, 2), RandomFloat(-2, 2), RandomFloat(-2, 2)));
			}
			break;
		}

		TargetPos += Vector(RandomFloat(-20, 20), RandomFloat(-20, 20), RandomFloat(-5, 5));
	}
}
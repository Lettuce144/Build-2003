//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef PREDICTED_VIEWMODEL_H
#define PREDICTED_VIEWMODEL_H
#ifdef _WIN32
#pragma once
#endif

#include "predictable_entity.h"
#include "utlvector.h"
#include "baseplayer_shared.h"
#include "shared_classnames.h"

#if defined( CLIENT_DLL )
#define CPredictedViewModel C_PredictedViewModel
#endif

enum
{
	VMTYPE_NONE = -1,	// Hasn't been set yet. We should never have this.
	VMTYPE_HL2 = 0,		// HL2-Type vmodels. Hands, weapon and anims are in the same model. (Used in HL1, HL2, CS:S, DOD:S, pretty much any old-gen Valve game)
	VMTYPE_L4D			// L4D-Type vmodels. Hands are a separate model, anims are in the weapon model. (Used in L4D, L4D2, Portal 2, CS:GO)
};

enum
{
	VM_WEAPON = 0,
	VM_LEGS,
	VM_STIM
};



class CPredictedViewModel : public CBaseViewModel
{
	DECLARE_CLASS( CPredictedViewModel, CBaseViewModel );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

public:
	

	CPredictedViewModel( void );
	

	virtual void SetWeaponModel(const char* pszModelname, CBaseCombatWeapon* weapon);

	
	void RemoveViewmodelAddon(void);

	int GetViewModelType(void) { return m_iViewModelType; }
	void SetViewModelType(int iType) { m_iViewModelType = iType; }

							
	

#if defined( CLIENT_DLL )

	virtual void ProcessMuzzleFlashEvent(void);
	virtual void OnDataChanged(DataUpdateType_t updateType);
	virtual void SetDormant(bool bDormant);
	virtual void UpdateOnRemove(void);



	virtual bool ShouldPredict(void)
	{
		if (GetOwner() && GetOwner() == C_BasePlayer::GetLocalPlayer())
			return true;

		return BaseClass::ShouldPredict();
	}

	virtual int DrawModel(int flags);
	virtual int	InternalDrawModel(int flags);
	virtual bool OnInternalDrawModel(ClientModelRenderInfo_t* pInfo);


	virtual void AddViewModelBob(CBasePlayer* owner, Vector& eyePosition, QAngle& eyeAngles);
	virtual void CalcViewModelLag(Vector& origin, QAngle& angles, QAngle& original_angles);

	virtual void CalcViewModelCollision(Vector& origin, QAngle& eyeAngles, CBasePlayer* pOwner);
	virtual void CalcViewModelView(CBasePlayer* owner, const Vector& eyePosition,
		const QAngle& eyeAngles);


	//CHandle<C_ViewmodelAttachmentModel> m_hViewmodelAddon;

	virtual void			FireEvent(const Vector& origin, const QAngle& angles, int event, const char* options);

	virtual RenderGroup_t	GetRenderGroup(void) { return RENDER_GROUP_VIEW_MODEL_OPAQUE; }

	virtual CBaseCombatWeapon* GetWeapon() { return static_cast<CBaseCombatWeapon*>(BaseClass::GetWeapon()); }



#endif

	virtual bool			ShouldReceiveProjectedTextures(int flags) { return true; }


private:
	
#if defined( CLIENT_DLL )

	CPredictedViewModel( const CPredictedViewModel & ); // not defined, not accessible

#endif

	// View-bobbing and swaying.
	float m_flSideTiltResult;
	float m_flSideTiltDifference;
	float m_flForwardOffsetResult;
	float m_flForwardOffsetDifference;

	// Wall collision thingy like in tarkov and stuff
	float m_flCurrentDistance;
	float m_flDistanceDifference;

	QAngle m_angEyeAngles;
	QAngle m_angViewPunch;
	QAngle m_angOldFacing;
	QAngle m_angDelta;
	QAngle m_angMotion;
	QAngle m_angCounterMotion;
	QAngle m_angCompensation;

	CNetworkVar(int, m_iViewModelType);
	CNetworkVar(int, m_iViewModelAddonModelIndex);



};

#endif // PREDICTED_VIEWMODEL_H

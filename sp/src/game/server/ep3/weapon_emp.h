#include "cbase.h"
#include "basehlcombatweapon.h"
#include "basecombatcharacter.h"
#include "prop_electricbegin.h"

//-----------------------------------------------------------------------------
// CWeaponProto1
//-----------------------------------------------------------------------------
class CWeapon_Emp : public CBaseHLCombatWeapon
{
	DECLARE_CLASS(CWeapon_Emp, CBaseHLCombatWeapon);
public:

	CWeapon_Emp(void);

	void	Precache(void);
	void	ItemPostFrame(void);
	void	PrimaryAttack(void);
	void	SecondaryAttack(void);

	bool Holster(CBaseCombatWeapon* pSwitchingTo);

	bool	CanSelectObject(CBaseEntity* pObject);
	void    EnableSelect(CBaseEntity* pObject);

	//void EnableLink(void);

	bool	HasAnyAmmo(void) { return true; }

	CBaseEntity* DetectObject();

	CPropElectricBegin* m_pPropElectricBegin;

	Vector m_vecCurrentObjectOrigin;

	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

private:
	bool	m_bIsInselectMode;

};
#include "cbase.h"
#include "baseentity.h"

class CPropElectricBegin : public CBaseAnimating
{
public:
	DECLARE_CLASS(CPropElectricBegin, CBaseAnimating);
	DECLARE_DATADESC();

	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void Think(void);

	void EnableLink(Vector pObject);


	//void	InputDisable(inputdata_t& inputdata);
	//void	InputEnable(inputdata_t& inputdata);

	//void	HandleState(void);
	bool m_bEnabled;
	bool m_bLinkEnabled;



	COutputEvent m_OnLinked;

};


//The following include files are necessary to allow your MyPanel.cpp to compile.
#include "cbase.h"
#include "IMyPanel.h"
using namespace vgui;
#include <vgui/IVGui.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/CheckButton.h>

//CMyPanel class: Tutorial example class
class CMyPanel : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CMyPanel, vgui::Frame);
	//CMyPanel : This Class / vgui::Frame : BaseClass

	CMyPanel(vgui::VPANEL parent); 	// Constructor
	~CMyPanel() {};				// Destructor

protected:
	//VGUI overrides:
	virtual void OnTick();
	virtual void OnCommand(const char* pcCommand);
	virtual void OnApplyData();
private:
	//Other used VGUI control Elements:
	Button* m_pCloseButton;

};

// Constuctor: Initializes the Panel
CMyPanel::CMyPanel(vgui::VPANEL parent)
	: BaseClass(NULL, "MyPanel")
{
	SetParent(parent);

	SetKeyBoardInputEnabled(true);
	SetMouseInputEnabled(true);

	SetProportional(false);
	SetTitleBarVisible(true);
	SetMinimizeButtonVisible(false);
	SetMaximizeButtonVisible(false);
	SetCloseButtonVisible(false);
	//SetDeleteSelfOnClose(true);
	SetSizeable(true);
	SetMoveable(true);
	SetVisible(true);


	SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));

	LoadControlSettings("resource/UI/playermodelsel.res");

	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);

	DevMsg("MyPanel has been constructed\n");



	//Need to put it here AND NOT BEFORE THIS SECTION
	// Credit: Milkwaym16 for providing reselection fix (Not documented on vdc wiki >:C)

	//Make our self selceted after an restart
	ConVarRef r_oldflashlight("r_oldflashlight");
	if (r_oldflashlight.IsValid())
	{
		SetControlInt("CheckButton1", r_oldflashlight.GetInt() ? 1 : 0);
	}

	//Make our self selceted after an restart
	ConVarRef sv_rollangle("sv_rollangle");
	if (sv_rollangle.IsValid())
	{
		SetControlInt("CheckButton2", sv_rollangle.GetInt() ? 1 : 0);
	}
}

//Class: CMyPanelInterface Class. Used for construction.
class CMyPanelInterface : public MyPanel
{
private:
	CMyPanel* MyPanel;
public:
	CMyPanelInterface()
	{
		MyPanel = NULL;
	}
	void Create(vgui::VPANEL parent)
	{
		MyPanel = new CMyPanel(parent);
	}
	void Destroy()
	{
		if (MyPanel)
		{
			MyPanel->SetParent((vgui::Panel*)NULL);
			delete MyPanel;
		}
	}
	void Activate(void)
	{
		if (MyPanel)
		{
			MyPanel->Activate();
		}
	}
};
static CMyPanelInterface g_MyPanel;
MyPanel* mypanel = (MyPanel*)&g_MyPanel;

ConVar cl_showmypanel("cl_showmypanel", "0", FCVAR_CLIENTDLL, "Sets the state of myPanel <state>");

void CMyPanel::OnTick()
{
	BaseClass::OnTick();
	SetVisible(cl_showmypanel.GetBool());
}

CON_COMMAND(OpenTestPanelFenix, "Toggles testpanelfenix on or off")
{
	cl_showmypanel.SetValue(!cl_showmypanel.GetBool());
	mypanel->Activate();
};

void CMyPanel::OnApplyData()
{
	//Real magic happens here
	ConVarRef r_oldflashlight("r_oldflashlight");
	r_oldflashlight.SetValue(GetControlInt("CheckButton1", 0));

	//Real magic happens here
	ConVarRef sv_rollangle("sv_rollangle");
	sv_rollangle.SetValue(GetControlInt("CheckButton2", 0));
}

void CMyPanel::OnCommand(const char* pcCommand)
{
	BaseClass::OnCommand(pcCommand);

	if (!Q_stricmp(pcCommand, "turnoff"))
		cl_showmypanel.SetValue(0);
		OnApplyData();
}
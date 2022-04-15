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
	CheckButton* CheckButton_1;

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

	LoadControlSettings("resource/UI/ModOptions.res");

	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);

	DevMsg("MyPanel has been constructed\n");

	CheckButton_1 = new CheckButton(this, "Checkbutton1", "Enables the old flashlight");
	CheckButton_1->SetPos(26, 335);
	CheckButton_1->SetSize(256, 24);

	//Need to put it here AND NOT BEFORE THIS SECTION
	// Credit: Milkwaym16 for providing reselection fix (Not documented on vdc wiki >:C)

	//Make our self selceted after an restart

	//Make our self selceted after an restart
	ConVarRef sv_rollangle("sv_rollangle");
	if (sv_rollangle.IsValid())
	{
		SetControlInt("CheckButton2", sv_rollangle.GetInt() ? 1 : 0);
	}
	ConVarRef sv_bhop_enabled("sv_bhop_enabled");
	if (sv_bhop_enabled.IsValid())
	{
		SetControlInt("CheckButton4", sv_bhop_enabled.GetInt() ? 1 : 0); //Bhop
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
			ConVarRef r_oldflashlight("r_oldflashlight");
			if (r_oldflashlight.IsValid())
			{
				MyPanel->SetControlInt("Checkbutton1", r_oldflashlight.GetInt() ? 1 : 0);
			}

			ConVarRef sv_zoomnosuit("sv_zoomnosuit");
			if (sv_zoomnosuit.IsValid())
			{
				MyPanel->SetControlInt("Checkbutton3", r_oldflashlight.GetInt() ? 1 : 0);
			}

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

	ConVarRef sv_zoomnosuit("sv_zoomnosuit");
	sv_zoomnosuit.SetValue(GetControlInt("CheckButton3", 0));

	ConVarRef sv_bhop_enabled("sv_bhop_enabled");
	sv_bhop_enabled.SetValue(GetControlInt("CheckButton4", 0));


}

void CMyPanel::OnCommand(const char* pcCommand)
{
	BaseClass::OnCommand(pcCommand);

	if (!Q_stricmp(pcCommand, "turnoff"))
		cl_showmypanel.SetValue(0);
		OnApplyData();
}
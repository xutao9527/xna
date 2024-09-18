
#include <windows.h>
#include "xna_app.h"
#include "ui/ui_dbg_window.h"


int APIENTRY WinMain(HINSTANCE instance_handle, HINSTANCE previous_instance_handle, char * command_line,int command_show)
{
	XnaApp& xnaApp = XnaApp::Instance();

	std::shared_ptr<UiDbgWindow> uiDbgWindow = xnaApp.CreateXnaWindow<UiDbgWindow>("调试窗口",  600, 500, true);
	if(uiDbgWindow){
		xnaApp.ShowXnaWindow(std::move(uiDbgWindow));
		uiDbgWindow.reset();
	}

	XnaApp::Instance().Run();
	return 0;
}

#include "common.h"
#include "globals.h"
#include "ui/ui.h"

int __stdcall WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
	g::init();

	HANDLE mutex = CreateMutex(NULL, FALSE, LSWAP_MUTEX_NAME);

	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		CloseHandle(mutex);
		MessageBox(NULL, LSWAP_APPLICATION_NAME, "Is already running.", MB_OK);
		return 0;
	}

	if (ui::init() == UI_OK)
	{
		ui::handler();
	}
	ui::undo();

	return 0;
}
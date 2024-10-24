#pragma once

#include "imgui_custom.h"

enum ui_stat {
	UI_FAIL = -1,
	UI_OK
};

namespace ui
{
	ui_stat init();
	void handler();
	void undo();
}
#include "globals.h"
#include "common.h"
#include "utils/utils.h"

void g::init()
{
	g::cfg            = config::init();
	g::monitor_width  = GetSystemMetrics(SM_CXSCREEN);
	g::monitor_height = GetSystemMetrics(SM_CYSCREEN);
	g::window_width   = 370;
	g::window_height  = 365;
	g::in_autorun     = utils::is_in_autorun(LSWAP_APPLICATION_NAME);
}
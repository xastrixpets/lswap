#pragma once

#include "config/config.h"

namespace g
{
	void init();

	inline cfg_t cfg;
	inline int   monitor_width, monitor_height;
	inline int   window_width, window_height;
	inline bool  in_autorun;
}
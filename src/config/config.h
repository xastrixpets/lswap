#pragma once

#include <string>

struct cfg_t {
	std::string source_lang;
	std::string target_lang;
};

namespace config
{
	/**
	 * Load configuration file
	 * @return cfg_t struct
	*/
	cfg_t init();

	/**
	 * Change values in configuration file
	 * @param source language
	 * @param target language
	 * @return boolean of successful
	*/
	bool change_cfg_values(const std::string& source_lang, const std::string& target_lang);
}
#include "config.h"
#include "../common.h"

#include <fstream>
#include <sstream>
#include <filesystem>

static std::string path;

static cfg_t read_values()
{
	cfg_t cfg;

	std::ifstream f(path);

	if (!f.is_open()) {
		return cfg;
	}

	std::string line, content;
	while (std::getline(f, line)) {
		content += line + "\n";
	}

	f.close();

	std::stringstream ss(content);
	while (std::getline(ss, line)) {
		if (line.empty() || line[0] == '#') {
			continue;
		}

		size_t equal_pos = line.find('=');
		if (equal_pos == std::string::npos) {
			continue;
		}

		std::string key = line.substr(0, equal_pos);
		std::string value = line.substr(equal_pos + 1);

		value = value.substr(1, value.length() - 2);

		if (key == "Source") {
			cfg.source_lang = value;
		}

		else if (key == "Target") {
			cfg.target_lang = value;
		}
	}

	return cfg;
}

static bool set_config_values(const std::string& source_lang, const std::string& target_lang)
{
	std::ofstream f(path);

	if (!f.is_open())
		return false;

	f << "# The configuration file\n";
	f << "# " LSWAP_APPLICATION_NAME " version " LSWAP_VERSION_STRING "\n\n";
	f << "Source=\"" << source_lang << "\"\n";
	f << "Target=\"" << target_lang << "\"";
	
	f.close();

	return true;
}

cfg_t config::init()
{
	cfg_t cfg;

	std::string user_directory = getenv("USERPROFILE");
	path = user_directory + "\\" LSWAP_CONFIGURATION_FILENAME;

	cfg.source_lang = "ru";
	cfg.target_lang = "en";

	if (!std::filesystem::exists(path))
		set_config_values(cfg.source_lang, cfg.target_lang);

	return cfg = read_values();
}

bool config::change_cfg_values(const std::string& source_lang, const std::string& target_lang)
{
	return set_config_values(source_lang, target_lang);
}
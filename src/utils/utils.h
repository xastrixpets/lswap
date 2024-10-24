#pragma once

#include <windows.h>
#include <string>

/*
* String of forbidden characters that will be deleted
* @description this is needed for curl_easy_escape() function to avoid some bugs
*/
#define FORBIDDEN_CHARS L"@#$%^&*\"\\/:\r\n\t<>(){}"

namespace utils
{
	/*
	* Remove characters from wstring
	* @param wstring
	* @param chars to remove
	* @return result in std::wstring
	*/
	std::wstring remove_chars(const std::wstring& str, const std::wstring& chars);

	/*
	* Checks if a given registry entry is present in the system's autorun configuration.
	*
	* @param The name of the registry entry to check for autorun status.
	* @return True if the registry entry is found in autorun, false otherwise.
	*/
	bool is_in_autorun(const std::string& reg_name);

	/*
	* Adds or removes a registry entry from the system's autorun configuration.
	*
	* @param The name of the registry entry to be added or removed.
	* @param A boolean value indicating whether to add (true) or remove (false)
	*          the registry entry from autorun.
	* @return True if the operation was successful, false otherwise.
	*/
	bool add_reg_to_autorun(const std::string& reg_name, bool v);

	/*
	* Get clipboard data
	* @param hwnd
	* @return result in std::wstring
	*/
	std::wstring get_current_clipboard(HWND hwnd);
	
	/*
	* Write data to clipboard
	* @param hwnd
	* @param data to write
	*/
	void put_in_clipboard(HWND hwnd, const std::wstring& data);
	
	/*
	* Parse json
	* @param json data
	* @return result in std::wstring
	*/
	std::wstring parse_json(const std::wstring& json);

	/*
	* Write callback function (Using in cURL)
	* @param pointer to the data to be written
	* @param size of the data to be written
	* @param number of elements in the data to be written
	* @param user pointer passed to the cURL call
	* @return number of bytes written
	*/
	size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp);

	/*
	* Converts a wstring to a UTF-8 string
	* @param the wstring to be converted
	* @return result in std::string
	*/
	std::string to_utf8(const std::wstring& str);

	/*
	* Class X is a utility for managing a mutable string buffer with a predefined maximum length.
	* It holds a reference to a std::string and automatically resizes it to fit within the limits
	* specified by the length parameter during construction and destruction.
	*
	* @param the std::string reference to be managed, which will be resized upon construction
	*          and destruction.
	* @note The maximum length of the string is defined by len_, and includes space for a null
	*       terminator.
	*
	* @return char* conversion operator enables easy usage as a C-style string.
	*/
	class X {
	public:
		X(std::string& s) : s_{ s } { s.resize(len_); }
		~X() { s_.resize(strlen(s_.c_str())); }
		operator char*() { return s_.data(); }
		static constexpr auto len() { return len_ - 1; }
	private:
		std::string& s_;
		static constexpr auto len_ = 255;
	};
}
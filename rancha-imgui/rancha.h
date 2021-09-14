#pragma once

#include "Account.h"
#include <fstream>
#include <filesystem>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>


bool SaveChar(Account acct, Character character);
bool SaveAcct(Account acct);
bool SaveTeam(Team newteam);
int LaunchDaoc(int server, const wchar_t* acct, const wchar_t* password, const wchar_t* character, int realm);
void FetchAccounts(std::vector<Account>& acctList);
void FetchTeams(std::vector<Team>& teamList);
const wchar_t* charToWChar(const char* text);
const char* WstringToChar(std::wstring text);
wchar_t* WstringToWchar(std::wstring text);
bool PathExists(std::wstring path);


// convert UTF-8 string to wstring
// https://gitlab.com/macroquest/next/mqnext/-/blob/master/include/mq/base/String.h
inline std::wstring utf8_to_wstring(std::string_view s)
{
	if (s.empty()) return {};

	int slength = (int)s.length();
	int len = ::MultiByteToWideChar(CP_UTF8, 0, s.data(), slength, nullptr, 0);
	std::wstring r;
	r.resize(len);
	::MultiByteToWideChar(CP_UTF8, 0, s.data(), slength, r.data(), len);
	return r;
}

// convert UTF16 wstring to string
// https://gitlab.com/macroquest/next/mqnext/-/blob/master/include/mq/base/String.h
inline std::string wstring_to_utf8(const std::wstring& s)
{
	if (s.empty())
		return {};

	int sizeNeeded = ::WideCharToMultiByte(CP_UTF8, 0, &s[0], (int)s.size(), nullptr, 0, nullptr, nullptr);
	std::string r;
	r.resize(sizeNeeded, 0);
	::WideCharToMultiByte(CP_UTF8, 0, s.c_str(), (int)s.size(), &r[0], sizeNeeded, nullptr, nullptr);
	return r;
}

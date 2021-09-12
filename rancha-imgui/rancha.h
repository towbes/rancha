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
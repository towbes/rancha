#pragma once

#include "Resource.h"
#include "Account.h"
#include <string>
#include <CommCtrl.h>
#include <iostream>
#include <fstream>
#include <filesystem>


//Index of menu for server, account name, password
int LaunchDaoc(int server, const wchar_t* acct, const wchar_t* password, const wchar_t* character, int realm);
wchar_t* GetText(HWND hWnd);
void DrawLogin(HWND hWnd);
void DrawProfiles(HWND hWnd);
bool SaveAcct(Account acct);
bool SaveChar(Account acct, Character character);
void FetchAccounts(std::vector<Account>& acctList);

//https://devblogs.microsoft.com/oldnewthing/20041025-00/?p=37483
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

struct profileIndex {
	int acctIndex;
	int charIndex;
};

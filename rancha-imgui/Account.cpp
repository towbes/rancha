#include "Account.h"

Account::Account() {
	acctName = L"";
	acctPassword = L"";
	charList;
}

Account::Account(std::wstring name, std::wstring pass) {
		acctName = name;
		acctPassword = pass;
		charList;
}
std::wstring Account::GetAcctName() {
	return acctName;
}
std::wstring Account::GetAcctPassword() {
	return acctPassword;
}
void Account::SetAcctPassword(std::wstring pass) {
	acctPassword = pass;
}

std::vector<Character> Account::GetChars() {
	return charList;
}

void Account::AddCharacter(Character character) {
	charList.push_back(character);
}

Character::Character() {
	charName = L"";
	server = 0;
	realm = 0;
}

Character::Character(std::wstring name, int serv, int rlm) {
		charName = name;
		server = serv;
		realm = rlm;
	}

std::wstring Character::GetName() {
	return charName;
}
int Character::GetServer() {
	return server;
}
int Character::GetRealm() {
	return realm;
}


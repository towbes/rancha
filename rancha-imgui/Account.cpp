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

TeamMember::TeamMember() {
	acct;
	charIndex = 0;

}

TeamMember::TeamMember(Account acctname, int charlistIndex) {
	acct = acctname;
	charIndex = charlistIndex;
}

Account TeamMember::GetAccount() {
	return acct;
}

int TeamMember::GetCharIndex() {
	return charIndex;
}

Team::Team() {
	teamName = L"";
	members;
}

Team::Team(std::vector<TeamMember> memberlist, std::wstring name) {
	teamName = name;
	members = memberlist;
}

std::wstring Team::GetName() {
	return teamName;
}

void Team::ChangeName(std::wstring newname) {
	teamName = newname;
}

std::vector<TeamMember> Team::GetMembers() {
	return members;
}

void Team::AddMember(TeamMember newmember) {
	members.push_back(newmember);
}
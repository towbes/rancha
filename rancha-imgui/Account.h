#pragma once

#include <string>
#include <vector>
#include "magic_enum.h"
#include "cereal/archives/binary.hpp"
#include "cereal/access.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/vector.hpp"

/*https://stackoverflow.com/questions/28828957/enum-to-string-in-modern-c11-c14-c17-and-future-c20


Went with this library:
https://github.com/Neargye/magic_enum


*/
//Server name enum

enum ServerName {
	Ywain1 = 0,
	Ywain2 = 1,
	Ywain3 = 2,
	Ywain4 = 3,
	Ywain5 = 4,
	Ywain6 = 5,
	Ywain7 = 6,
	Ywain8 = 7,
	Ywain9 = 8,
	Ywain10 = 9,
	Gaheris = 10
};

//Cereal needs header for certain types http://uscilab.github.io/cereal/stl_support.html

class Character {
private:
	std::wstring charName;
	int server;
	int realm;

	friend class cereal::access;

	template <class Archive>
	void serialize(Archive& ar) {
		ar(this->charName, this->server, this->realm);
	}

public:
	Character();
	Character(std::wstring name, int serv, int rlm);
	std::wstring GetName();
	int GetServer();
	int GetRealm();

};

class Account {
private:
	std::wstring acctName;
	std::wstring acctPassword;
	std::vector<Character> charList;

	friend class cereal::access;

	template <class Archive>
	void serialize(Archive& ar) {
		ar(this->acctName, this->acctPassword, this->charList);
	}

public:
	Account();
	Account(std::wstring name, std::wstring pass);
	std::wstring GetAcctName();
	std::wstring GetAcctPassword();
	void SetAcctPassword(std::wstring pass);

	std::vector<Character> GetChars();
	void AddCharacter(Character character);
};

class TeamMember {
private:
	Account acct;
	int charIndex;

	friend class cereal::access;

	template <class Archive>
	void serialize(Archive& ar) {
		ar(this->acct, this->charIndex);
	}

public:
	TeamMember();
	TeamMember(Account acctname, int charlistIndex);
	Account GetAccount();
	int GetCharIndex();
};

class Team {
private:
	std::wstring teamName;
	std::vector<TeamMember> members;

	friend class cereal::access;

	template <class Archive>
	void serialize(Archive& ar) {
		ar(this->teamName, this->members);
	}

public:
	Team();
	Team(std::vector<TeamMember> memberlist, std::wstring name);
	std::wstring GetName();
	void ChangeName(std::wstring newname);
	std::vector<TeamMember> GetMembers();
	void AddMember(TeamMember newmember);
};
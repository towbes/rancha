#pragma once

#include <string>
#include <vector>
#include "cereal/archives/binary.hpp"
#include "cereal/access.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/vector.hpp"

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


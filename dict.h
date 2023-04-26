#ifndef _KARIN_DICT_H
#define _KARIN_DICT_H

#include <unordered_map>
#include <vector>
#include <iostream>

#include "str.h"

class idVec3;
class idMat3;

class idDict
{
	private:
	using hash_table = std::unordered_map<idStr, idStr, std::hash<std::string>>;

	public:
		hash_table hash;
		std::vector<idStr> sequence;

		void Set(const char *name, const char *value);
		void SetFloat(const char *name, float value);
		void SetInteger(const char *name, int value);
		void SetBool(const char *name, bool value);
		void SetVec3(const char *name, const idVec3 &value);
		void SetMat3(const char *name, const idMat3 &value);
		void Clear(void) {
			sequence.clear();
			hash.clear();
		}

		const char * GetString(const char *name, const char *def = "") const;
		int Num() const {
			return sequence.size();
		}
		bool IsEmpty() const {
			return sequence.empty();
		}
		std::vector<idStr>::iterator begin() {
			return sequence.begin();
		}
		std::vector<idStr>::iterator end() {
			return sequence.end();
		}
		std::vector<idStr>::const_iterator begin() const {
			return sequence.cbegin();
		}
		std::vector<idStr>::const_iterator end() const {
			return sequence.cend();
		}
		const char * operator[](const char *name) const {
			return GetString(name);
		}
		bool Contains(const char *name) const;
		const char * operator[](int i) const {
			return hash.at(sequence[i]);
		}
		friend std::ostream & operator<<(std::ostream &o, const idDict &v);
};

#endif

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
	public:
		void Set(const char *name, const char *value);
		void SetFloat(const char *name, float value);
		void SetInteger(const char *name, int value);
		void SetBool(const char *name, bool value);
		void SetVec3(const char *name, const idVec3 &value);
		void SetMat3(const char *name, const idMat3 &value);
		void Clear(void);

		int Num() const;
		bool IsEmpty() const;
		std::vector<idStr>::iterator begin();
		std::vector<idStr>::iterator end();
		std::vector<idStr>::const_iterator begin() const;
		std::vector<idStr>::const_iterator end() const;
		const char * operator[](const char *name) const;
		const char * operator[](int i) const;

		bool Contains(const char *name) const;
		const char * GetString(const char *name, const char *def = "") const;
		friend std::ostream & operator<<(std::ostream &o, const idDict &v);

	private:
		using hash_table = std::unordered_map<idStr, idStr, std::hash<std::string>>;
		hash_table hash;
		std::vector<idStr> sequence;
};

inline int idDict::Num() const
{
	return sequence.size();
}

inline bool idDict::IsEmpty() const
{
	return sequence.empty();
}

inline std::vector<idStr>::iterator idDict::begin()
{
	return sequence.begin();
}

inline std::vector<idStr>::iterator idDict::end()
{
	return sequence.end();
}

inline std::vector<idStr>::const_iterator idDict::begin() const
{
	return sequence.cbegin();
}

inline std::vector<idStr>::const_iterator idDict::end() const
{
	return sequence.cend();
}

inline const char * idDict::operator[](const char *name) const
{
	return GetString(name);
}

inline const char * idDict::operator[](int i) const
{
	return hash.at(sequence[i]);
}

#endif

#ifndef _KARIN_LIST_H
#define _KARIN_LIST_H

#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>

template <class T>
class idList : public std::vector<T>
{
	public:
		using std::vector<T>::vector;
		idList<T> & operator<<(const T &t);
		idList<T> & operator<<(T &&t);
		int Append(const T &t);
		int Append(T &&t);
		void DeleteContents(const std::default_delete<T> &deleteFunc, bool clear = true);
		void DeleteContents(bool clear = true);
		operator const T *(void) const;
		operator T *(void);
		int FindIndex(const T &t) const;
		bool Contains(const T &t) const;
		template <class U>
		friend std::ostream & operator<<(std::ostream &o, const idList<U> &list);
		void Sort(bool(*cmp)(const T&, const T&));
};

template <class T>
inline idList<T> & idList<T>::operator<<(const T &t)
{
	push_back(t);
	return *this;
}

template <class T>
idList<T> & idList<T>::operator<<(T &&t)
{
	emplace_back(t);
	return *this;
}

template <class T>
int idList<T>::Append(const T &t)
{
	this->push_back(t);
	return this->size() - 1;
}

template <class T>
int idList<T>::Append(T &&t)
{
	this->emplace_back(t);
	return this->size() - 1;
}

template <class T>
void idList<T>::DeleteContents(const std::default_delete<T> &deleteFunc, bool clear)
{
	for(auto &p : *this)
	{
		deleteFunc(p);
		p = nullptr;
	}
	if(clear)
		this->clear();
}

template <class T>
void idList<T>::DeleteContents(bool clear)
{
	for(auto &p : *this)
	{
		delete p;
		p = nullptr;
	}
	if(clear)
		this->clear();
}

template <class T>
inline idList<T>::operator const T *(void) const
{
	return this->data();
}

template <class T>
inline idList<T>::operator T *(void)
{
	return this->data();
}

template <class T>
int idList<T>::FindIndex(const T &t) const
{
	int res = 0;
	for(const auto &i : *this)
	{
		if(i == t)
			break;
		res++;
	}
	return res >= this->size() ? -1 : res;
}

template <class T>
bool idList<T>::Contains(const T &t) const
{
	return std::find(this->cbegin(), this->cend(), t) != this->cend();
}

template <class T>
void idList<T>::Sort(bool(*cmp)(const T&, const T&))
{
	std::stable_sort(this->begin(), this->end(), cmp);
}

template <class T>
std::ostream & operator<<(std::ostream &o, const idList<T> &list)
{
	for(const auto &i : list)
		o << i;
	return o;
}

#endif

#pragma once
#include "stdafx.h"

class utils
{
	utils();
	utils(const utils&);
	void operator=(utils&);
	static std::mutex m_mutex;
	static utils* m_pInstance;
public:
	~utils();
	static utils* GetInstance();
	template<class T> T read(const DWORD& dwNodeBase);
	void log(char* _format, ...);
	bool Ustrstr(char* str1, char *str2);
	MODULEINFO GetModuleInformationEx(std::string moduleName);
};

template<class T>
T utils::read(const DWORD& _dwNodeBase)
{
	T value;
	__try
	{
		value = *(T*)(_dwNodeBase);
	}
	__except (1)
	{
		memset(&value, 0, sizeof(T));
	}
	return value;
}


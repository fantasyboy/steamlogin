#include "stdafx.h"

#include "utils.h"


std::mutex utils::m_mutex;
utils* utils::m_pInstance = nullptr;
utils::utils()
{
}


utils::utils(const utils&)
{

}

void utils::operator=(utils&)
{

}

utils::~utils()
{
}

utils* utils::GetInstance()
{
	if (nullptr == m_pInstance)
	{
		m_mutex.lock();
		if (nullptr == m_pInstance)
		{
			m_pInstance = new utils;
		}
		m_mutex.unlock();
	}
	return m_pInstance;
}

void utils::log(char* _format, ...)
{
#ifdef _USER_DBG
	std::string temp;
	va_list marker = { 0 };
	va_start(marker, _format);
	size_t num_of_chars = _vscprintf(_format, marker);
	if (num_of_chars > temp.capacity())
	{
		temp.reserve(num_of_chars + 1);
	}
	vsprintf_s(const_cast<char*>(temp.c_str()), num_of_chars + 1, _format, marker);
	OutputDebugString(temp.c_str());
#endif
}

bool utils::Ustrstr(char* str1, char *str2)
{
	__try {
		return strstr(str1, str2) != NULL;
	}
	__except (1) {
		return false;
	}

	return false;
}


MODULEINFO utils::GetModuleInformationEx(std::string moduleName)
{
	MODULEINFO module = { 0 };
	auto hModule = GetModuleHandle(moduleName.c_str());
	if (hModule)
	{
		GetModuleInformation(GetCurrentProcess(), hModule, &module, sizeof(MODULEINFO));
	}
	return module;
}

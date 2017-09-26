// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <string>
#include <process.h>
#include "utils.h"
void login(std::string username, std::string password, bool bRember);
unsigned __stdcall ThreadFunc(void* pArguments);

void travseButton(DWORD addr);
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: {
		DisableThreadLibraryCalls(hModule);
		_beginthreadex(NULL, NULL, &ThreadFunc, NULL, NULL, NULL);
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

void login(std::string username, std::string password, bool bRember)
{
	__asm {
		MOV EAX, DWORD PTR DS : [0x39FE17A8];
		MOV ESI, EAX;
		MOV EDI, DWORD PTR DS : [ESI];
		PUSH 0;
		LEA EAX, password;
		PUSH EAX;
		LEA EAX, username;
		PUSH EAX;
		MOV ECX, ESI;
		MOV EAX , DWORD PTR DS : [EDI + 0xC0];
		CALL EAX;
	}
}

unsigned __stdcall ThreadFunc(void * pArguments)
{

	const DWORD Base_ButtonTravseAddr = 0x3ED949CC;
	auto Base = utils::GetInstance()->read<DWORD>(Base_ButtonTravseAddr);
	if (!Base) {
#ifdef _USER_DBG
		utils::GetInstance()->log("HXL: 没有找到Base_ButtonTravseAddr ！");
#endif 
		return 1;
	}

	auto Base2 = utils::GetInstance()->read<DWORD>(Base + 0x38);
	if (!Base2) {
#ifdef _USER_DBG
		utils::GetInstance()->log("HXL: 没有找到Base2！");
#endif 
		return 1;
	}
	//Base2 就是 链表的基地址

	travseButton(Base2);
	return 0;
}

void travseButton(DWORD addr)
{
	auto StartAddr = utils::GetInstance()->read<DWORD>(addr + 0x4);
	auto ButtonNum = utils::GetInstance()->read<DWORD>(addr + 0x10);

	if (!StartAddr || !ButtonNum) {
		//如果链表起始地址 || 控件数量不存在
		return;
	}

	for (DWORD i = 0; i < ButtonNum; i+=4){
		auto temp = utils::GetInstance()->read<DWORD>(StartAddr + i);
		if (temp){
			auto objTemp = utils::GetInstance()->read<DWORD>(temp + 0x20);
			if (objTemp) {
				//读取需要函数地址
				auto objTemp2 = utils::GetInstance()->read<DWORD>(objTemp);
				if (objTemp2) {
					//MOV EDX, DWORD PTR DS : [EAX]
					//读取虚表函数+4的位置
					auto objTemp4 = utils::GetInstance()->read<DWORD>(objTemp2+ 0x4);
					if (objTemp4) {
						//找到了虚表函数地址

						//58384275    2B49 FC         SUB ECX, DWORD PTR DS : [ECX - 4]
						//58384278    83E9 44         SUB ECX, 44
						// E9
						auto ecx_1 = objTemp4 + 0x5;
						//从 objTemp4 + 0x5 开始读，一直读到下一个E9位置
						auto index = 0;
						do {
							auto data = utils::GetInstance()->read<BYTE>(ecx_1 + index);
							if (data == 0xe9) {
								break;
							}
							index++;
						} while (1);
						DWORD ecx_2 = 0;
						memcpy(&ecx_2, (void*)(ecx_1), index);

#ifdef _USER_DBG
						utils::GetInstance()->log("HXL: objTemp4 = %x ecx_1 = %x", objTemp4, ecx_2);
#endif // _USER_DBG
						auto ecx_sub_4 = utils::GetInstance()->read<DWORD>(objTemp - 4);
						//ecx_ret 就是需要的 对象地址
						DWORD ecx_ret = (objTemp - ecx_sub_4) - ecx_2;
#ifdef _USER_DBG
						utils::GetInstance()->log("HXL: ecx_ret = %x name = %s", ecx_ret, (char*)(*(DWORD*)(ecx_ret - 0x70)));
#endif // _USER_DBG
						
					}
				}
			}
			//递归遍历链表
			travseButton(temp);
		}
	}
}

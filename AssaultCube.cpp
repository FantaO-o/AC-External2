#include <iostream>
#include <windows.h>
#include <vector>
#include "mem.h"
#include "proc.h"

// DO NOT USING NAMESPACE STD!!!!!!!!!!!!!!!!!!!!!!!!!
int main() {
	HANDLE hProcess = 0;
	uintptr_t moduleBase = 0, localPlayerPtr = 0, healthAddr = 0, ammoAddr = 0;
	bool bHealth = false, bAmmo = false, bRecoil = false;

	int newHealth = 1337;

	// ========================== get process ID of target process==========================
	DWORD procId = GetProcId(L"ac_client.exe"); // LETTER 'L' Transforms our unicode "ac_client.exe" to char array

	if (procId) {
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, procId);

		moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");

		localPlayerPtr = moduleBase + 0x10f4f4;

		healthAddr = FindDMMAAddy(hProcess, localPlayerPtr, { 0xf8 });

		ammoAddr = FindDMMAAddy(hProcess, localPlayerPtr, { 0x150 });

	} else {
		std::printf("Process not found\n");
		getchar();
		return 0;
	}

	DWORD dwExit = 0;
	while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE) {

		if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
			bHealth = !bHealth;
		}

		if (GetAsyncKeyState(VK_NUMPAD2) & 1) {
			bAmmo = !bAmmo;

		}

		if (GetAsyncKeyState(VK_NUMPAD3) & 1) {
			bRecoil = !bRecoil;
			if (bRecoil) {
				mem::NopEx((BYTE*)(moduleBase + 0x63786), 10, hProcess);
			} else {
				mem::PatchEx((BYTE*)(moduleBase + 0x63786), (BYTE*)"x50\x8d\x4c\x24\x1c\x51\x8b\xce\xff\xd2", 10, hProcess);
			}
		}

		if (GetAsyncKeyState(VK_INSERT) & 1) {
			return 0;
		}

		// continous loop
		if (bHealth) {
			mem::PatchEx((BYTE*)healthAddr, (BYTE*)&newHealth, sizeof(newHealth), hProcess);
		}

		if (bAmmo) {
			mem::PatchEx((BYTE*)ammoAddr, (BYTE*)&newHealth, sizeof(newHealth), hProcess);
		}

		Sleep(10);
	}

	std::printf("EXITING\n");
	getchar();
	return 0;

}

#include <Windows.h>

unsigned char* hook_location = (unsigned char*)0x0040AD9D;
// this is what the overwritten location was calling
DWORD original_call_address = 0x4607C0;
// this is the next address to be executed after we are done with our shenanigans
DWORD original_next_address = 0x0040ADA2;
DWORD edi_value = 0;
INPUT input = { 0 };

// the naked spec prevents C++ from setting up a new stack frame and complicating our plans
__declspec(naked) void codecave()
{
	__asm {
		call original_call_address
		pushad
		mov edi_value, eax
	};

	if (edi_value != 0)
	{
		// looking at player: emulate left click down
		input.type = INPUT_MOUSE;
		input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		SendInput(1, &input, sizeof(INPUT));
	}
	else
	{
		// not looking at player: emulate left click up
		input.type = INPUT_MOUSE;
		input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		SendInput(1, &input, sizeof(INPUT));
	}

	// restore registers and jump back to the location after overwritten command
	__asm {
		popad
		jmp original_next_address
	};
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		DWORD old_protect;
		// Change protections so we can write to hook location
		VirtualProtect((void*)hook_location, 5, PAGE_EXECUTE_READWRITE, &old_protect);
		*hook_location = 0xE9; // Overwrite hook location with opcode for jump
		// Calculate relative distance to pass as parameter to jmp
		*(DWORD*)(hook_location + 1) = (DWORD)&codecave - ((DWORD)hook_location + 5);
	}

	return true;
}
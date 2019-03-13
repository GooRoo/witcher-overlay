#include <cstddef>
#include <iostream>
#include <string>
#include <Windows.h>

#include "dx9_overlay/api.h"

int main()
{
	SetParam("process", "witcher.exe");
	CHAR *currentDir = new CHAR[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, currentDir);
	SetOverlayAssetsPath(currentDir);

	return EXIT_SUCCESS;
}

#include "stdafx.h"

using namespace std;

HANDLE hstdout;
char listBuffer[0xFFFF];

bool extractFile(HANDLE _archive, const char* filename)
{
	__asm
	{
		push filename
		push filename
		mov ecx, _archive
		call SFileExtractFile
	}
}

vector<string> parseListFile(char* buffer, int length)
{
	auto entries = explode(string(buffer, length), "\n");

	for(int i = 0; i < entries.size();i++)
	{
		entries[i] = fixString(entries[i]);
	}

	return entries;
}

void cleanListFile(HANDLE _archive, vector<string>* list)
{
	HANDLE hFile = 0;
	vector<int> toClean;

	for(int i = 0; i < list->size(); i++, hFile = 0)
	{
		// Check if listFile itself
		if((isRootFile((*list)[i].c_str()) && hasEnding((*list)[i].c_str(), ".lst")))
		{
			toClean.push_back(i);
			continue;
		}

		// Could use SFileHasFile instead, but meh...
		NIFSOpenFileEx(_archive, (*list)[i].c_str(), 1, &hFile, 0);

		if(hFile)
		{
			if(*(DWORD *)((char*)hFile + 40) != 0x46494C45) // Check for invalid handle
			{
				toClean.push_back(i);
			}
			else
			{
				__asm mov esi, hFile
				SFileCloseFile();
			}
		}
		else
		{
			toClean.push_back(i);
		}
	}

	for(int i = toClean.size() - 1; i >= 0; i--)
	{
		list->erase(list->begin() + toClean[i]);
	}
}

void handleFile(const char* archive)
{
	// Open archive for reading
	printf("Opening archive '%s'...\n", archive);
	HANDLE _archive = SFileOpenArchive(archive, 0);

	if(_archive)
	{
		// Read listFile
		DWORD length;
		HANDLE hListFile;
		NIFSOpenFileEx(_archive, "(listfile)", 1, &hListFile, 0);

		if(!hListFile)
		{
			SetConsoleTextAttribute( hstdout, 0x0C );
			printf("Failed to open listfile!\n");
			SetConsoleTextAttribute( hstdout, 0x07 );
		}
		else
		{
			__asm mov edx, offset listBuffer
			__asm mov ecx, hListFile
			SFileReadFile(sizeof(listBuffer), &length, 1);

			if(!length)
			{
				SetConsoleTextAttribute( hstdout, 0x0C );
				printf("Failed to open listfile!\n");
				SetConsoleTextAttribute( hstdout, 0x07 );
			}
			else
			{
				__asm mov esi, hListFile
				SFileCloseFile();
				auto files = parseListFile(listBuffer, length);
				cleanListFile(_archive, &files);

				// Extract files from archive
				printf("Extracting %d files...\n", files.size());

				for(int i = 0;i<files.size();i++)
				{
					printf("%s\n", files[i].c_str());
					if(!extractFile(_archive, files[i].c_str()))
					{
						SetConsoleTextAttribute( hstdout, 0x0C );
						printf("Failed to extract %s!\n", files[i].c_str());
						SetConsoleTextAttribute( hstdout, 0x07 );
					}
				}
			}
		}


		// Dispose archive
		__asm mov esi, _archive // Pass archive handle to SFileCloseFile
		SFileCloseFile();
	}
	else
	{
		SetConsoleTextAttribute( hstdout, 0x0C );
		printf("Failed to open archive %s\n", archive);
		SetConsoleTextAttribute( hstdout, 0x07 );
	}
}

void printTitle(byte color)
{
	                                                                                                SetConsoleTextAttribute( hstdout, (color << 4) );
	printf("                                                                                 ");    SetConsoleTextAttribute( hstdout, color );
	printf("                                                                              ");       SetConsoleTextAttribute( hstdout, (color << 4) );
	printf("  ");                                                                                   SetConsoleTextAttribute( hstdout, color );
	printf("                         React's IFS Tool by momo5502                         ");       SetConsoleTextAttribute( hstdout, (color << 4) );
	printf("  ");                                                                                   SetConsoleTextAttribute( hstdout, color );
	printf("                                                                              ");       SetConsoleTextAttribute( hstdout, (color << 4) );
	printf("                                                                                 \n");  SetConsoleTextAttribute( hstdout, 0x07 );
}

DWORD debugFuncPtr_1;
DWORD debugFuncPtr_2;

void enableDebugOutput(HANDLE lib)
{
	// Allow debug output
	if(DO_DEBUG) DebugEnable();

	// Simulate ifsdebug.dll's 'CreateDebugInterface'
	debugFuncPtr_1 = (DWORD)&OutputDebugStringA;
	debugFuncPtr_2 = (DWORD)&debugFuncPtr_1;
	*(DWORD*)((DWORD)lib + 0x88AC0) = (DWORD)&debugFuncPtr_2; 
}

int _tmain(int argc, _TCHAR* argv[])
{
	hstdout = GetStdHandle( STD_OUTPUT_HANDLE );
	SetConsoleTitle("IFS Tool");
	printTitle(11);

	HANDLE ifs2_lib = LoadLibrary("IFS2.dll");

	if(!ifs2_lib)
	{
		SetConsoleTextAttribute( hstdout, 0x0C );
		printf("Unable to load IFS2.dll!\n\n");
		SetConsoleTextAttribute( hstdout, 0x07 );
	}
	else
	{
		assignFunctions(ifs2_lib);
		enableDebugOutput(ifs2_lib);

		auto archives = GetPassedArchives();

		if(!archives.size())
		{
			SetConsoleTextAttribute( hstdout, 0x0C );
			printf("No archive given!\n\n");
			SetConsoleTextAttribute( hstdout, 0x07 );
		}
		else
		{
			for(int i = 0;i<archives.size();i++)
			{
				handleFile(archives[i].c_str());
				printf("\n");
			}
		}
	}

	printf("Press any key to exit...");
	_getch();
	return 0;
}

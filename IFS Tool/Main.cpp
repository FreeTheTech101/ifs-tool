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

string getListFile(HANDLE _archive)
{
	DWORD length;
	HANDLE hListFile;
	NIFSOpenFileEx(_archive, "(listfile)", 1, &hListFile, 0);

	if(!hListFile)
		return "";

	__asm mov edx, offset listBuffer
	__asm mov ecx, hListFile
	SFileReadFile(sizeof(listBuffer), &length, 1);

	if(!length)
		return "";

	auto files = parseListFile(listBuffer, length);

	string listName;

	for(int i = 0;i<files.size();i++)
	{
		if(hasEnding(files[i], ".lst"))
		{
			listName = files[i];
			break;
		}
	}

	return listName;
}

void handleFile(const char* archive)
{
	// Open archive for reading
	printf("Opening archive '%s'...\n", archive);
	HANDLE _archive = SFileOpenArchive(archive, 0);

	if(_archive)
	{
		//Read listFile name
		string listFile = getListFile(_archive);

		// Read listFile
		DWORD length;
		HANDLE hListFile;
		NIFSOpenFileEx(_archive, listFile.c_str(), 1, &hListFile, 0);

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
				printf("ListFile: %s (%d)\n", listFile.c_str(), length);
				auto files = parseListFile(listBuffer, length);
				files.erase(files.begin());

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

int _tmain(int argc, _TCHAR* argv[])
{
	hstdout = GetStdHandle( STD_OUTPUT_HANDLE );
	SetConsoleTextAttribute( hstdout, 0x0B );

	printf("=======================\n");
	printf("| React IFS extractor |\n");
	printf("=======================\n\n");

	SetConsoleTextAttribute( hstdout, 0x07 );

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

		auto archives = GetPassedArchives();

		if(!archives.size())
		{
			SetConsoleTextAttribute( hstdout, 0x0C );
			printf("No archives given!\n\n");
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

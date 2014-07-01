#include "stdafx.h"

using namespace std;

HANDLE hstdout;

bool __stdcall extractFile(HANDLE _archive, const char* filename)
{
	__asm mov ecx, _archive // Pass archive handle to SFileExtractFile
	if(!SFileExtractFile(filename, TEMP_EXTRACT)) // Weirdness is that you have to pass a char* directly, doing so as variable will crash. IDK why, probably I'm too dumb...
	{
		return false;
	}
	else
	{
		// Create directory
		char* path = (char*)malloc_n(strlen(filename) + 1);

		for(int i = 0;i<strlen(filename);i++)
		{
			if(filename[i] == '\\' || filename[i] == '/')
			{
				memcpy(path, filename, i);
				path[i + 1] = 0;
				_mkdir(path);
			}
		}

		free(path);

		MoveFile(TEMP_EXTRACT, filename);
		return true;
	}
}

vector<string> parseListFile(char* buffer, int length)
{
	auto entries = explode(buffer, "\n");

	// Remove list itself
	entries.erase(entries.begin());

	for(int i = 0; i < entries.size();i++)
	{
		entries[i] = fixString(entries[i]);
	}

	return entries;
}

// Probably there is a better way to do this, but that's ok for now
string getListFile(const char* _archive)
{
	FILE* file = fopen(_archive, "rb");

	if(!file)
		return "";

	string list = "";
	IFS archiveHeader;

	fread(&archiveHeader, sizeof(IFS), 1, file);
	fseek(file, archiveHeader.HeaderSize, SEEK_SET);

	char buffer;

	while (!hasEnding(list, ".lst"))
	{
		fread(&buffer, 1, 1, file);
		list += buffer;
	}

	fclose(file);

	return list;
}

void handleFile(const char* archive)
{
	// Open archive for reading
	printf("Opening archive '%s'...\n", archive);
	HANDLE _archive = SFileOpenArchive(archive, 0);

	if(_archive)
	{
		//Read listFile name
		string listFile = getListFile(archive);

		// Read listFile
		DWORD length;
		HANDLE hListFile;
		NIFSOpenFileEx(_archive, listFile.c_str(), 1, &hListFile, 0);
		char* buffer = (char*)malloc_n(0x1000);

		__asm mov edx, buffer
		__asm mov ecx, hListFile
		SFileReadFile(0x1000, &length, 1);

		__asm mov esi, hListFile
		SFileCloseFile();
		printf("ListFile: %s (%d)\n", listFile.c_str(), length);

		// Extract files from archive
		auto files = parseListFile(buffer, length);
		free(buffer);

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

		// Close archive handle
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
		printf("Unable to load IFS2.dll!\n");
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
	remove(TEMP_EXTRACT);
	return 0;
}

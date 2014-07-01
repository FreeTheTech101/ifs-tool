#include "stdafx.h"

using namespace std;

HANDLE ifs2_lib;

// Functions are actually wrappers, as Tencent is unable to handle function params
typedef HANDLE (__stdcall * SFileOpenArchive_w_t)(const char* filename, int unk); // Probably file mode?
SFileOpenArchive_w_t SFileOpenArchive;

typedef bool (__stdcall * SFileExtractFile_w_t)(const char* inFile, const char* outFile);
SFileExtractFile_w_t SFileExtractFile;

typedef bool (__stdcall * SFileCloseFile_t)();
SFileCloseFile_t SFileCloseFile;

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

vector<string> parseListFile(HANDLE _archive, const char* listfile)
{
	__asm mov ecx, _archive // Pass archive handle to SFileExtractFile
	if(!SFileExtractFile(listfile, TEMP_LIST))
		return explode("", "fail");

	FILE* listF = fopen(TEMP_LIST, "r");

	if(!listF)
		return explode("", "fail");

	fseek (listF, 0, SEEK_END);
	long length = ftell(listF);
	rewind (listF);

	char* buffer = (char*)malloc_n(length);

	fread (buffer, 1, length, listF);
	fclose(listF);

	remove(TEMP_LIST);

	auto entries = explode(buffer, "\n");

	free(buffer);

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

	int headerSize;
	string list = "";

	fseek(file, 4, SEEK_SET);
	fread(&headerSize, 4, 1, file);
	fseek(file, headerSize, SEEK_SET);

	char buffer;

	while (!hasEnding(list, ".lst"))
	{
		fread(&buffer, 1, 1, file);
		list += buffer;
	}

	fclose(file);

	return list;
}

void handleFile()
{
	SFileOpenArchive = (SFileOpenArchive_w_t)((DWORD)ifs2_lib + 0x16230);
	SFileExtractFile = (SFileExtractFile_w_t)((DWORD)ifs2_lib + 0x24BA0);
	SFileCloseFile   = (SFileCloseFile_t)((DWORD)ifs2_lib + 0x1FFB0);

	string archive = GetCommandLine();
	
	const size_t last_slash_idx = archive.find_last_of("\\/");
	if (string::npos != last_slash_idx)
	{
		archive.erase(0, last_slash_idx + 1);
	}

	if(hasEnding(archive, "\""))
	{
		archive = archive.substr(0, archive.size() - 1);
	}

	if(!hasEnding(archive, ".ifs"))
	{
		archive = "You're an idiot!";
	}

	printf("Opening archive '%s'...\n", archive.c_str());

	HANDLE _archive = SFileOpenArchive(archive.c_str(), 0);

	if(_archive)
	{
		auto listFile = getListFile(archive.c_str());
		char* buffer = (char*)malloc_n(0x400);

		printf("ListFile: %s\n", listFile.c_str());

		auto files = parseListFile(_archive, listFile.c_str());

		printf("Extracting %d files...\n", files.size());

		for(int i = 0;i<files.size();i++)
		{
			printf("%s\n", files[i].c_str());
			if(!extractFile(_archive, files[i].c_str()))
			{
				printf("Failed to extract %s!\n", files[i].c_str());
			}
		}

		__asm mov esi, _archive // Pass archive handle to SFileCloseFile
		SFileCloseFile();
	}
	else
	{
		printf("Failed to open archive!\n");
	}
}

void cleanUp()
{
	remove(TEMP_LIST);
	remove(TEMP_EXTRACT);
}

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hstdout = GetStdHandle( STD_OUTPUT_HANDLE );
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo( hstdout, &csbi );
	SetConsoleTextAttribute( hstdout, 0x0B );

	printf("=======================\n");
	printf("= React IFS extractor =\n");
	printf("=======================\n");

	SetConsoleTextAttribute( hstdout, csbi.wAttributes );

	ifs2_lib = LoadLibrary("IFS2.dll");

	if(!ifs2_lib)
	{
		printf("Unable to load IFS2.dll!\n");
	}
	else
	{
		handleFile();
	}

	printf("Press any key to exit...");
	_getch();
	cleanUp();
	return 0;
}


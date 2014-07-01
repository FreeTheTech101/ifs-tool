// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Various generic utility functions.
//
// Initial author: NTAuthority
// Started: 2010-09-10
// ==========================================================

#include "stdafx.h"

#define VA_BUFFER_COUNT		4
#define VA_BUFFER_SIZE		32768

static char g_vaBuffer[VA_BUFFER_COUNT][VA_BUFFER_SIZE];
static int g_vaNextBufferIndex = 0;

const char *va( const char *fmt, ... )
{
	va_list ap;
	char *dest = &g_vaBuffer[g_vaNextBufferIndex][0];
	g_vaNextBufferIndex = (g_vaNextBufferIndex + 1) % VA_BUFFER_COUNT;
	va_start(ap, fmt);
	int res = _vsnprintf( dest, VA_BUFFER_SIZE, fmt, ap );
	dest[VA_BUFFER_SIZE - 1] = '\0';
	va_end(ap);

	return dest;
}

char tempFile[MAX_PATH];

std::vector<std::string> GetPassedArchives()
{
	int numArgs;
	std::vector<std::string> archives;
	LPCWSTR commandLine = GetCommandLineW();
	LPWSTR* argv = CommandLineToArgvW(commandLine, &numArgs);

	for (int i = 0; i < numArgs; i++)
	{
		WideCharToMultiByte(CP_ACP, 0, argv[i], -1, tempFile, MAX_PATH, "?", NULL);

		if(hasEnding(tempFile, ".ifs"))
		{
			archives.push_back(tempFile);
		}
	}

	LocalFree(argv);
	return archives;
}

void* malloc_n(size_t length)
{
	void* memPtr = malloc(length);
	memset(memPtr, 0, length);
	return memPtr;
}

void free_n(void* memPtr)
{
	if(memPtr)
	{
		free(memPtr);
	}
}

bool hasEnding (std::string const &fullString, std::string const &ending)
{
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
	} else {
		return false;
	}
}

std::string fixString(std::string str)
{
	while (hasEnding(str, "\x0D") || hasEnding(str, "\x0A"))
	{
		str = str.substr(0, str.size() - 1);
	}

	return str;
}

std::vector<std::string> explode(const std::string& str, const std::string& delimiters)
{
	std::vector<std::string> tokens;

	auto subStrBeginPos = str.find_first_not_of(delimiters, 0);
	auto subStrEndPos = str.find_first_of(delimiters, subStrBeginPos);

	while(std::string::npos != subStrBeginPos || std::string::npos != subStrEndPos)
	{
		tokens.push_back(str.substr(subStrBeginPos, subStrEndPos-subStrBeginPos));

		subStrBeginPos = str.find_first_not_of(delimiters, subStrEndPos);
		subStrEndPos = str.find_first_of(delimiters, subStrBeginPos);
	}

	return tokens;
}

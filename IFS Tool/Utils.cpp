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

// determine which patchset to use
unsigned int _gameFlags;

typedef struct  
{
	const wchar_t* argument;
	unsigned int flag;
} flagDef_t;

flagDef_t flags[] =
{
	{ L"dump", GAME_FLAG_DUMPDATA },
	{ 0, 0 }
};

bool hasLicenseKey = false;
char licenseKey[48];

const char* GetLicenseKey()
{
	return (hasLicenseKey) ? &licenseKey[1] : NULL;
}

void DetermineGameFlags()
{
	int numArgs;
	LPCWSTR commandLine = GetCommandLineW();
	LPWSTR* argv = CommandLineToArgvW(commandLine, &numArgs);

	for (int i = 0; i < numArgs; i++)
	{
		if (argv[i][0] == L'#' || argv[i][0] == L'@')
		{
			WideCharToMultiByte(CP_ACP, 0, argv[i], -1, licenseKey, sizeof(licenseKey), "?", NULL);

			hasLicenseKey = true;
		}

		if (argv[i][0] != L'-') continue;

		for (wchar_t* c = argv[i]; *c != L'\0'; c++)
		{
			if (*c != L'-')
			{
				for (flagDef_t* flag = flags; flag->argument; flag++)
				{
					if (!wcscmp(c, flag->argument))
					{
						_gameFlags |= flag->flag;
						break;
					}
				}
				break;
			}
		}
	}

	LocalFree(argv);
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
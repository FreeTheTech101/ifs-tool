extern unsigned int _gameFlags;
void DetermineGameFlags();

// flag settings
#define GAME_FLAG_DUMPDATA		(1 << 0)

//#ifndef PRE_RELEASE_DEMO
#define GAME_FLAG(x)			((_gameFlags & x) == x)

const char *va( const char *fmt, ... );
void* malloc_n(size_t length);
void free_n(void* memPtr);

bool hasEnding (std::string const &fullString, std::string const &ending);
std::string fixString(std::string str);
std::vector<std::string> explode(const std::string& str, const std::string& delimiters);
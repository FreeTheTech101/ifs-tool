// Pretty obsolete now...
struct IFS
{
	DWORD dwSignature;          // 0:3
	DWORD HeaderSize;           // 4:7
	USHORT wFormatVersion;      // 8:9
	USHORT wSectorSize;         // A:B
	DWORD dwArchiveSize;        // C:13
	ULONGLONG BetTablePos64;    // 14:1B
	ULONGLONG HetTablePos64;    // 1C:23
	ULONGLONG MD5TablePos64;    // 24:2B
	ULONGLONG BitmapPos64;      // 2C:33
	ULONGLONG HetTableSize;     // 34:3B
	ULONGLONG BetTableSize;     // 3C:43
	ULONGLONG MD5TableSize;     // 44:4B
	ULONGLONG BitmapSize;       // 4C:53
	DWORD dwMD5PieceSize;       // 54:57
	DWORD dwRawChunkSize;       // 58:5C

	// Copied from StormLib. Not sure if equal to Tencent's stuff
	BYTE MD5_BlockTable[0x10];      // MD5 of the block table before decryption
	BYTE MD5_HashTable[0x10];       // MD5 of the hash table before decryption
	BYTE MD5_HiBlockTable[0x10];    // MD5 of the hi-block table
	BYTE MD5_BetTable[0x10];        // MD5 of the BET table before decryption
	BYTE MD5_HetTable[0x10];        // MD5 of the HET table before decryption
	BYTE MD5_MpqHeader[0x10];       // MD5 of the MPQ header from signature to (including) MD5_HetTable
};


// Functions are actually wrappers, as Tencent is unable to handle function params
typedef HANDLE (__stdcall * SFileOpenArchive_w_t)(const char* filename, int unk); // Probably file mode?
extern SFileOpenArchive_w_t SFileOpenArchive;

typedef bool (__stdcall * SFileExtractFile_w_t)(const char* inFile, const char* outFile);
extern SFileExtractFile_w_t SFileExtractFile;

typedef bool (__stdcall * SFileCloseFile_t)();
extern SFileCloseFile_t SFileCloseFile;

typedef void (__stdcall * SFileReadFile_w_t)(HANDLE file, DWORD length, LPDWORD pdwRead, int unk);
extern SFileReadFile_w_t SFileReadFile_w;

typedef void (__stdcall * SFileReadFile_t)(DWORD length, LPDWORD pdwRead, int unk);
extern SFileReadFile_t SFileReadFile;

typedef bool (__stdcall * NIFSOpenFileEx_t)(HANDLE archive, const char * szFileName, DWORD dwSearchScope, HANDLE * phFile, DWORD unk);
extern NIFSOpenFileEx_t NIFSOpenFileEx;


void assignFunctions(HANDLE lib);

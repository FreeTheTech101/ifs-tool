#include "stdafx.h"

SFileOpenArchive_w_t SFileOpenArchive;
SFileExtractFile_w_t SFileExtractFile;
SFileCloseFile_t SFileCloseFile;
SFileReadFile_w_t SFileReadFile_w;
SFileReadFile_t SFileReadFile;
NIFSOpenFileEx_t NIFSOpenFileEx;

void assignFunctions(HANDLE lib)
{
	SFileOpenArchive = (SFileOpenArchive_w_t)((DWORD)lib + 0x16230);
	SFileExtractFile = (SFileExtractFile_w_t)((DWORD)lib + 0x24BA0);
	SFileCloseFile   = (SFileCloseFile_t)((DWORD)lib + 0x1FFB0);
	SFileReadFile_w  = (SFileReadFile_w_t)((DWORD)lib + 0x24880);
	SFileReadFile    = (SFileReadFile_t)((DWORD)lib + 0x215D0);
	NIFSOpenFileEx   = (NIFSOpenFileEx_t)((DWORD)lib + 0x1EDE0);
}

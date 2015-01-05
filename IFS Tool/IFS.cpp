#include "stdafx.h"

SFileOpenArchive_w_t SFileOpenArchive;
SFileExtractFile_w_t SFileExtractFile;
SFileCloseFile_t SFileCloseFile;
SFileReadFile_t SFileReadFile;
NIFSOpenFileEx_t NIFSOpenFileEx;

DebugEnable_t DebugEnable;

void assignFunctions(HANDLE lib)
{
	SFileOpenArchive = (SFileOpenArchive_w_t)((DWORD)lib + 0x163E0);
	SFileExtractFile = (SFileExtractFile_w_t)((DWORD)lib + 0x25DF0);
	SFileCloseFile   = (SFileCloseFile_t)((DWORD)lib + 0x210A0);
	SFileReadFile    = (SFileReadFile_t)((DWORD)lib + 0x22570);
	NIFSOpenFileEx   = (NIFSOpenFileEx_t)((DWORD)lib + 0x1FED0);

	DebugEnable      = (DebugEnable_t)((DWORD)lib + 0x16210);
}

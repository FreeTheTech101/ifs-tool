struct IFS
{
	char  dwSignature[4];	// 0:3
	int   HeaderSize;		// 4:7
	short wFormatVersion;	// 8:9
	short wSectorSize;		// A:B
	long  ArchiveSize;		// C:13
	long  BetTablePos64;	// 14:1B
	long  HetTablePos64;	// 1C:23
	long  MD5TablePos64;	// 24:2B
	long  BitmapPos64;		// 2C:33
	long  HetTableSize;		// 34:3B
	long  BetTableSize;		// 3C:43
	long  MD5TableSize; 	// 44:4B
	long  BitmapSize;		// 4C:53
	int   dwMD5PieceSize;	// 54:57
	int   dwRawChunkSize;	// 58:5C

	// Copied from StormLib. Not sure if equal to Tencent's stuff
	unsigned char MD5_BlockTable[0x10];      // MD5 of the block table before decryption
	unsigned char MD5_HashTable[0x10];       // MD5 of the hash table before decryption
	unsigned char MD5_HiBlockTable[0x10];    // MD5 of the hi-block table
	unsigned char MD5_BetTable[0x10];        // MD5 of the BET table before decryption
	unsigned char MD5_HetTable[0x10];        // MD5 of the HET table before decryption
	unsigned char MD5_MpqHeader[0x10];       // MD5 of the MPQ header from signature to (including) MD5_HetTable
};
#include "sEncryption.h"

void __cdecl zeroBuffer(char* inputBuffer, int bufferSize)
{

	std::memset(inputBuffer, NULL, bufferSize);
};
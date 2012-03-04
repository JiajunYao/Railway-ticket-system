/**
 * This file contains common functions in client and server
 */

#include "client_server.h"

// this function is a wrapper function of c library function fgets, the purpose is to handle the error caused by eof
char* Fgets(char* s, int size, FILE* stream)
{
	char* fgets_result = fgets(s, size, stream);
	if(fgets_result == NULL && !feof(stream))
	{
		return NULL;
	}
	else if(fgets_result == NULL && feof(stream))
	{
		return Fgets(s, size, stream);
	}
	else
	{
		return fgets_result;
	}
}

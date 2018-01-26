#include<windows.h>
#include<string>
#include<stdexcept>

#define BUF_LENGTH 1000000

using namespace std;
void usage()
{
	printf("Extends a file to a given length.\n");
	printf("\n");
	printf("  extend_file [file_name] [new_size]\n");
}
void printerr(const char* s)
{
	fprintf(stderr, "%s", s);
}
int main(int argc, char *argv[])
{
    if (argc != 3)
	{
		usage();
		return 1;
	}
	static_assert(sizeof(void *) == 8, "Assumes 64-bit");
	long long int curSize, newSize;
	LARGE_INTEGER li;
	HANDLE hFile;
	try
	{
		newSize = stoull(string(argv[2]));
	}
	catch(const invalid_argument& e)
	{
		printerr("Invalid parameter for newSize");
		return 1;
	}
	catch(const out_of_range& e)
	{
		printerr("newSize out of range");
		return 1;
	}
	hFile = CreateFile(
		argv[1],
		GENERIC_READ |GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printerr("CreateFile failed");
		return 1;
	}
    if (GetFileSizeEx(hFile, &li) == FALSE)
	{
		printerr("GetFileSizeEx failed");
		return 1;
	}
	curSize = li.QuadPart;
	if (curSize > newSize)
	{
		printerr("The given file is already bigger than the new size");
		return 1;
	}
	if (SetFilePointer(hFile, 0, NULL, FILE_END)==INVALID_SET_FILE_POINTER)
	{
		printerr("SetFilePointer failed");
		return 1;
	}
	DWORD written;
	BYTE* buf = new BYTE[BUF_LENGTH];
	memset(buf, 0, BUF_LENGTH);
	while (newSize - curSize >= BUF_LENGTH)
	{
		if ((WriteFile(hFile, buf, BUF_LENGTH, &written, NULL)==FALSE) ||
			(written != BUF_LENGTH))
		{
			printerr("WriteFile failed");
			return 1;
		}
		curSize += BUF_LENGTH;
	}
	while (curSize < newSize)
	{
		BYTE b = 0;
		if ((WriteFile(hFile, &b, 1, &written, NULL)==FALSE) ||
			(written != 1))
		{
			printerr("WriteFile failed");
			return 1;
		}
		curSize += 1;
	}
	return 0;
}

/*
 * NOTE
 * My changes to this code are under the MIT License. As I do not know who made this script initially, and there was no license with it, I am going to assume that the initial code wasn't licensed at all.
 * Contributions will all be considered to be a part of the MIT License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _MSC_VER
#include <sys/utime.h>
#else
#include <utime.h>
#endif

#define PE_SIGNATURE "00004550"

int readFourBytesHex(char *str, FILE *src);

int main(int argc, char *argv[])
{
	FILE *fp;
	int c;
	int i;
	int idx;
	char temp[9];
	unsigned long n;
	time_t tm;
	struct utimbuf new_times;
	char *cTimeString;
	if (argc != 2)
	{
		fprintf(stderr, "usage: %s <DLL/EXE>", argv[0]);
		return 1;
	}
	fp = fopen(argv[1], "r");
	if (fp == NULL)
	{
		fprintf(stderr, "error: failed to open the file.");
		return 1;
	}
	for (i = 1; i <= 60; i++)
	{
		c = getc(fp);
		if (c == EOF)
		{
			fprintf(stderr, "error: the file is not a windows object file.");
			return -1;
		}
	}
	temp[8] = '\0';
	readFourBytesHex(temp, fp);
	i += 4;
	n = strtoul(temp, NULL, 16);
	for (; i < n; i++)
	{
		c = getc(fp);
		if (c == EOF)
		{
			fprintf(stderr, "error: the file is not a windows object file.");
			return -1;
		}
	}
	if (readFourBytesHex(temp, fp) != 0)
	{
		fprintf(stderr, "error: the file is not a windows object file.");
		return -1;
	}
	i += 4;
	if (strcmp(temp, PE_SIGNATURE) != 0)
	{
		fprintf(stderr, "File is not a PE or its header is corrupted. Either way, bubye!");
		return -1;
	}
	i = 0;
	for (; i < 4; i++)
	{
		c = getc(fp);
		if (c == EOF)
		{
			fprintf(stderr, "error: the file is not a windows object file.");
			return -1;
		}
	}
	if (readFourBytesHex(temp, fp) != 0)
	{
		fprintf(stderr, "error: the file is not a windows object file.");
		return -1;
	}
	n = strtoul(temp, NULL, 16);
	tm = n;
	cTimeString = ctime(&tm);
	printf("Compile Date Time Stamp: %s", cTimeString);
	new_times.modtime = n;
	utime(argv[1], &new_times);
	fclose(fp);
	return 0;
}

// Reads four bytes in Hex in Little Endian format.
int readFourBytesHex(char *str, FILE *src)
{
	char byteStr[2];
	int i;
	int c;
	str += 6;
	for (i = 0; i < 4; i++)
	{
		c = getc(src);
		if (c == EOF)
		{
			return -1;
		}
		sprintf(byteStr, "%002x ", c);
		strncpy(str, byteStr, 2);
		str -= 2;
	}
	return 0;
}

#include "syscall.h"

int main(int argc, char **argv)
{
	char str[256];
	int len;
	int i;

	while (1) {
		len = ReadFile(str, 255, 0);
		if (len <= 0)
			break;
		PrintString(str);
		PrintString("\n");
		for (i = 0; i < 256; i++)
			str[i] = 0;
	}
	return 0;
}

#include "syscall.h"

void print_hex(char chr)
{
	switch (chr) {
	case 0:
		PrintString("0");
		break;
	case 1:
		PrintString("1");
		break;
	case 2:
		PrintString("2");
		break;
	case 3:
		PrintString("3");
		break;
	case 4:
		PrintString("4");
		break;
	case 5:
		PrintString("5");
		break;
	case 6:
		PrintString("6");
		break;
	case 7:
		PrintString("7");
		break;
	case 8:
		PrintString("8");
		break;
	case 9:
		PrintString("9");
		break;
	case 10:
		PrintString("A");
		break;
	case 11:
		PrintString("B");
		break;
	case 12:
		PrintString("C");
		break;
	case 13:
		PrintString("D");
		break;
	case 14:
		PrintString("E");
		break;
	case 15:
		PrintString("F");
		break;
	}
}

int main(int argc, char **argv)
{
	OpenFileID fd;
	char filename[256];
	char userinp[256];
	char buffer[256];
	int bytes_read;
	int i;

	PrintString("Nhap ten file can dump: ");
	ReadFile(filename, 256, 0);
	fd = OpenFileFunc(filename, 1);
	if (fd == -1) {
		PrintString("Khong the mo file.\n");
		return 1;
	}

	do {
		bytes_read = ReadFile(buffer, 255, fd);
		if (bytes_read <= 0)
			break;

		for (i = 0; i < bytes_read; i++) {
			print_hex(buffer[i] / 16);
			print_hex(buffer[i] % 16);
			PrintString(" ");
			if (i % 16 == 15)
				PrintString("\n");
		}
	}
	while (1);
	PrintString("\n");

	return 0;
}

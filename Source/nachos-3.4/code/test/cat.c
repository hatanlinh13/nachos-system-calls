#include "syscall.h"

int main(int argc, char **argv)
{
	OpenFileID open;
	char filename[256];
	PrintString("Nhap ten file: ");
	ReadFile(filename, 255, 0);
	open = OpenFileFunc(filename, 1);
	if (open == -1) {
		PrintString("Ten file nhap vao khong hop le.\n");
		return 1;
	}
	else {
		char buffer[255];
		int bytes_read;
		while (1) {
			bytes_read = ReadFile(buffer, 255, open);
			if (bytes_read >= 0) {
				//if (WriteFile(buffer, bytes_read, 1) == -1) {
				//	PrintString("Co loi khi ghi ra man hinh console.\n");
				//}
				WriteFile(buffer, bytes_read, 1);
			}
			else {
				CloseFile(open);
				return 0;
			}
		}
	}
	return 0;
}

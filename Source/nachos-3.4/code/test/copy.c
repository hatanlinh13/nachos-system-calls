#include <syscall.h>

int main(int argc, char **argv)
{
	OpenFileID sfd;
	OpenFileID dfd;
	char srcfilename[256];
	char dstfilename[256];
	char buffer[256];
	int bytes_read;

	PrintString("Nhap ten file nguon: ");
	ReadFile(srcfilename, 255, 0);
	sfd = OpenFileFunc(srcfilename, 1);
	if (sfd == -1) {
		PrintString("Khong the mo file nguon.\n");
		return 1;
	}
	PrintString("Nhap ten file dich: ");
	ReadFile(dstfilename, 255, 0);
	CreateFile(dstfilename);
	dfd = OpenFileFunc(dstfilename, 0);
	if (dfd == -1) {
		PrintString("Khong the mo file dich.\n");
		CloseFile(sfd);
		return 1;
	}

	while ((bytes_read = ReadFile(buffer, 255, sfd)) >= 0) {
		WriteFile(buffer, bytes_read - 1, dfd);
	}

	CloseFile(sfd);
	CloseFile(dfd);

	return 0;
}

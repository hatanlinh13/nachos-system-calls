#include "syscall.h"
#include "copyright.h"
int main(int argc, char* argv[])
{
	char filename[256];
	PrintString("Nhap ten file: ");
	ReadFile(filename, 256, 0);
	if (CreateFile(filename))
	{
		PrintString("Can not create file\n");
	} 
	else
	{
		PrintString("Successfully create file\n");
	}
	
	return 0;
}

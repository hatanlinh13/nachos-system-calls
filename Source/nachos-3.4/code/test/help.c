#include <syscall.h>

int
main()
{
	PrintString("He Dieu Hanh\n");
	PrintString("Do An 1 - Exceptions va System Calls\n");
	PrintString("***\n");
	PrintString("Nhom 0xff - thanh vien:\n");
	PrintString(">>> Nguyen Thanh Tan - 1512491\n");
	PrintString(">>> Ha Tan Linh - 1512284\n");
	PrintString("***\n");
	PrintString("Chuong trinh ascii:\n");
	PrintString(">>> In ra man hinh 128 ki tu cua bo ma ASCII chuan.\n");
	PrintString("Chuong trinh sort:\n");
	PrintString(">>> Nguoi dung nhap vao N va sau do la N so nguyen,\n");
	PrintString(">>> neu khong hop le thi yeu cau nguoi dung nhap lai.\n");
	PrintString(">>> Sau do sap xep tang dan va in lai ra man hinh.\n");
	return 0;
}

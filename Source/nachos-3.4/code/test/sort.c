#include <syscall.h>

#define N_MAX 100

int
main()
{
	int n;
	int arr[N_MAX];
	int i, j;

	PrintString("Nhap N (N <= 100): ");
	n = ReadInt();
	while (n == 0 || n > 100) {
		PrintString("So nhap khong hop le, vui long nhap lai: ");
		n = ReadInt();
	}

	/* Input N integers */
	for (i = 0; i < n; i++) {
		PrintString("Nhap vao so nguyen thu ");
		PrintInt(i+1);
		PrintString(": ");
		arr[i] = ReadInt();
		while (arr[i] == 0) {
			PrintString("Nhap khong hop le, vui long nhap lai: ");
			arr[i] = ReadInt();
		}
	}

	/* Bubble sort */
	for (i = 0; i < n - 1; i++) {
		/* Last i integers are already sorted */
		for (j = 0; j < n - i - 1; j++) {
			if (arr[j] > arr[j + 1]) {
				/* Swap two integers */
				arr[j]     ^= arr[j + 1];
				arr[j + 1] ^= arr[j];
				arr[j]     ^= arr[j + 1];
			}
		}
	}

	/* Print N sorted integers */
	PrintString("Day so da sap xep:\n");
	for (i = 0; i < n - 1; i++) {
		PrintInt(arr[i]);
		PrintString(", ");
	}
	/* Last integer */
	PrintInt(arr[n - 1]);
	PrintString(".\n");

	return 0;
}

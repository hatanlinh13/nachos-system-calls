#include <syscall.h>

/*
 * actions todo for each turn:
 *
 * ->first, *voinuoc* opens and reads file *require.txt*
 * to get the water amount required by *sinhvien* this turn.
 *
 * ->then, *voinuoc* opens and reads two file *first.txt* and *second.txt*
 * to get the amount of water served by each tap
 *
 * ->next, *voinuoc* chooses a tap with less served water
 * use this tap to server *sinhvien* of this turn
 * and update the value of either *first* or *second*
 *
 * ->finally, *voinuoc* opens file *tap.txt* and writes 1 or 2 respectively
 * based on which tap it used this turn.
 */
int main()
{
	OpenFileID require;
	OpenFileID tap;
	char buf[1];
	int first;
	int second;
	int required;

	while (1) {
		/* wait for other process signal to begin serving */
		Wait("sem_voinuoc");

		required = 0;
		require = OpenFileFunc("require.txt", 1);
		if (require == -1)
			break;
		while (1) {
			if (ReadFile(buf, 1, require) <= 0)
				break;
			if (buf[0] >= '0' && buf[0] <= '9')
				required = 10*required + (buf[0] - '0');
			else
				break;
		}
		CloseFile(require);

		if (required == 0)
			break;

		first = 0;
		require = OpenFileFunc("first.txt", 1);
		if (require == -1)
			break;
		while (1) {
			if (ReadFile(buf, 1, require) <= 0)
				break;
			if (buf[0] >= '0' && buf[0] <= '9')
				first = 10*first + (buf[0] - '0');
			else
				break;
		}
		CloseFile(require);

		second = 0;
		require = OpenFileFunc("second.txt", 0);
		if (require == -1)
			break;
		while (1) {
			if (ReadFile(buf, 1, require) <= 0)
				break;
			if (buf[0] >= '0' && buf[0] <= '9')
				second = 10*second + (buf[0] - '0');
			else
				break;
		}
		CloseFile(require);

		CreateFile("tap.txt");
		tap = OpenFileFunc("tap.txt", 0);
		if (first <= second) {
			first += required;
			WriteFile("1", 1, tap);
			require = OpenFileFunc("first.txt", 0);
			if (first == 0) {
				buf[0] = '0';
				WriteFile(buf, 1, require);
			}
			else {
				int f = first;
				int d = 1;
				while (f > 9) {
					d *= 10;
					f /= 10;
				}
				while (first > 0) {
					buf[0] = '0' + (first / d);
					WriteFile(buf, 1, require);
					first = first % d;
					d /= 10;
				}
			}
			CloseFile(require);
		}
		else {
			second += required;
			WriteFile("2", 1, tap);
			require = OpenFileFunc("second.txt", 0);
			if (second == 0) {
				buf[0] = '0';
				WriteFile(buf, 1, require);
			}
			else {
				int f = second;
				int d = 1;
				while (f > 9) {
					d *= 10;
					f /= 10;
				}
				while (second > 0) {
					buf[0] = '0' + (second / d);
					WriteFile(buf, 1, require);
					second = second % d;
					d /= 10;
				}
			}
			CloseFile(require);
		}
		CloseFile(tap);

		/* served one student, notice *sinhvien* to continue its execution */
		Signal("sem_sinhvien");
	}
}

#include <syscall.h>

/*
 * ->first, *sinhvien* opens file *turn.txt* to read data of this turn
 * and opens file *rslt.txt* to write result of this turn
 *
 * ->then, *sinhvien* executes one *voinuoc* process
 *
 * ->finally, for earch number in *turn.txt*
 * *sinhvien* create a file *require.txt* contains this number
 * call *Signal* to pass control to *voinuoc*
 * and append returned value in *tap.txt* to *rslt.txt*
 */
int main(int argc, char **argv)
{
	OpenFileID turn;
	OpenFileID rslt;
	OpenFileID require;
	OpenFileID tap;
	OpenFileID fs;
	SpaceId    tapp;
	char buf[1];
	int endof;

	turn = OpenFileFunc("turn.txt", 1);
	if (turn == -1)
		ExitProcess(-1);

	CreateFile("rslt.txt");
	rslt = OpenFileFunc("rslt.txt", 0);
	if (rslt == -1)
		ExitProcess(-1);

	tapp = ExecProcess("./test/voinuoc");
	if (tapp == -1)
		ExitProcess(-1);

	CreateFile("first.txt");
	fs = OpenFileFunc("first.txt", 0);
	buf[0] = '0';
	WriteFile(buf, 1, fs);
	CloseFile(fs);
	CreateFile("second.txt");
	fs = OpenFileFunc("second.txt", 0);
	buf[0] = '0';
	WriteFile(buf, 1, fs);
	CloseFile(fs);

	endof = 0;
	while (!endof) {
		CreateFile("require.txt");
		require = OpenFileFunc("require.txt", 0);
		while (1) {
			if (ReadFile(buf, 1, turn) <= 0) {
				endof = 1;
				break;
			}
			if (buf[0] >= '0' && buf[0] <= '9') {
				WriteFile(buf, 1, require);
			}
			else {
				break;
			}
		}
		CloseFile(require);

		/* notice *voinuoc* to start its execution */
		Signal("sem_voinuoc");
		/* wait for notice from other process to continue */
		Wait("sem_sinhvien");

		/* append result to turn's result file */
		tap = OpenFileFunc("tap.txt", 1);
		ReadFile(buf, 1, tap);
		WriteFile(buf, 1, rslt);
		buf[0] = ' ';
		WriteFile(buf, 1, rslt);
		CloseFile(tap);
	}

	CloseFile(turn);
	CloseFile(rslt);
	ExitProcess(0);
}

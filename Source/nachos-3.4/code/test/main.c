#include <syscall.h>

/*
 * ->first, *main* create 2 semaphores to control *sinhvien* and *voinuoc*
 *
 * ->next, *main* opens *input.txt* and *output.txt*
 *
 * ->then, *main* reads N - which is the number of turns
 *
 * ->finally, for each turn, *main* copies data to *turn.txt*
 * creates a *sinhvien* process, joins to this process
 * waits for it to return
 * and copies result from *rslt.txt* to *output.txt*
 */
int main(int argc, char **argv)
{
	OpenFileID inp;
	OpenFileID otp;
	OpenFileID turn;
	OpenFileID rslt;
	char buf[1];
	int n = 0;
	int i;
	int pid;
	int rv;

	CreateSemaphore("sem_sinhvien", 0);
	CreateSemaphore("sem_voinuoc" , 0);

	inp = OpenFileFunc("input.txt", 1);
	if (inp == -1)
		return -1;
	CreateFile("output.txt");
	otp = OpenFileFunc("output.txt", 0);
	if (otp == -1)
		return -1;

	while (1) {
		if (ReadFile(buf, 1, inp) <= 0)
			break;
		if (buf[0] >= '0' && buf[0] <= '9')
			n = 10*n + (buf[0] - '0');
		else if (buf[0] == '\n')
			break;
	}

	for (i = 0; i < n; i++) {
		/* copy data of a turn from input to turn */
		CreateFile("turn.txt");
		turn = OpenFileFunc("turn.txt", 0);
		while (1) {
			if (ReadFile(buf, 1, inp) <= 0)
				break;
			if (buf[0] == '\n')
				break;
			else
				WriteFile(buf, 1, turn);
		}
		CloseFile(turn);

		/* execute and join *sinhvien* process */
		pid = ExecProcess("./test/sinhvien");
		if (pid == -1)
			break;
		rv  = JoinProcess(pid);
		if (rv == -1)
			break;

		/* append result to output */
		rslt = OpenFileFunc("rslt.txt", 1);
		if (rslt == -1)
			break;
		while (ReadFile(buf, 1, rslt) > 0)
			WriteFile(buf, 1, otp);
		buf[0] = '\n';
		WriteFile(buf, 1, otp);
		CloseFile(rslt);
	}

	PrintString("Program exited successfully. See output.txt for result!\n");
	CloseFile(inp);
	CloseFile(otp);
	return 0;
}

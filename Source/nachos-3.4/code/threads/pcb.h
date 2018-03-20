#ifndef __PCB_H__
#define __PCB_H__

#include "synch.h"

class PCB {
private:
	char process_name[50];
	int  process_id;
	int  parent_id;
	int  numwait;
	int  exitcode;
	Thread    *thread;
	Semaphore *joinsem;
	Semaphore *exitsem;
	Semaphore *multex;
public:
	PCB();
	~PCB();

	int  Exec(char *name, int pid, int ppid);
	void JoinWait();
	void ExitWait();
	void JoinRelease();
	void ExitRelease();

	char *GetName();
	int   GetID();
	int   GetParentID();
	int   GetNumWait();
	int   GetExitCode();
	void  SetName(char *name);
	void  SetExitCode(int ec);
	Thread * GetThread();
};

#endif

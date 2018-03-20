#ifndef __PTABLE_H__
#define __PTABLE_H__

#include "bitmap.h"
#include "synch.h"
#include "pcb.h"

#define MAX_PROCESS 10

class Ptable {
private:
	PCB       *pcb[MAX_PROCESS];
	BitMap    *bm;
	Semaphore *bmsem;
public:
	Ptable();
	~Ptable();

	int ExecUpdate(char *name);
	int ExitUpdate(int ecd);
	int JoinUpdate(int pid);

	bool isExist(int pid);
	void Remove(int pid);

	int   GetFreeSlot();
	char *GetName(int pid);
};

#endif

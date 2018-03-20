#ifndef __STABLE_H__
#define __STABLE_H__

#include "bitmap.h"
#include "sem.h"

#define MAX_SEMAPHORE 10

class Stable {
private:
	BitMap *bm;
	Sem    *semtab[MAX_SEMAPHORE];
public:
	Stable();
	~Stable();

	int Create(char *name, int init);
	int Wait(char *name);
	int Signal(char *name);
	int FindFreeSlot();
};

#endif

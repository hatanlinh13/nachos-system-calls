#ifndef __SEM_H__
#define __SEM_H__

#include "synch.h"

class Sem {
private:
	char       sem_name[50];
	Semaphore *sem;
public:
	Sem(char *name, int id);
	~Sem();

	void Wait();
	void Signal();

	char *GetName();
};

#endif

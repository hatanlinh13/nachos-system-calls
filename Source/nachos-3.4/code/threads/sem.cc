#include <cstring>

#include "sem.h"

Sem::Sem(char *name, int id)
{
	strncpy(sem_name, name, 50);
	sem = new Semaphore(name, id);
}

Sem::~Sem()
{
	delete sem;
}

char *Sem::GetName()
{
	return sem_name;
}

void Sem::Wait()
{
	sem->P();
}

void Sem::Signal()
{
	sem->V();
}

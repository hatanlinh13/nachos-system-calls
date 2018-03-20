#include <cstring>

#include "stable.h"

Stable::Stable()
{
	bm = new BitMap(MAX_SEMAPHORE);
	for (int i = 0; i < MAX_SEMAPHORE; i++)
		semtab[i] = NULL;
}

Stable::~Stable()
{
	for (int i = 0; i < MAX_SEMAPHORE; i++)
		if (semtab[i]) {
			delete semtab[i];
			semtab[i] = NULL;
		}
	delete bm;
}

int Stable::FindFreeSlot()
{
	/* as a side effect
	 * mark the bit in bitmap
	 * therefore this function should be called
	 * if and only if neccessary
	 */
	return bm->Find();
}

int Stable::Create(char *name, int init)
{
	if (!name) return -1;
	if (init < 0)     return -1;

	for (int i = 0; i < MAX_SEMAPHORE; i++) {
		if (semtab[i]
		&&  strncmp(semtab[i]->GetName(), name, 49) == 0)
		/* name already existed in semaphore list */
			return -1;
	}
	
	int slot = FindFreeSlot();
	if (slot == -1) /* cannot find free slot */
		return -1;
	semtab[slot] = new Sem(name, init);
	return 0;
}

int Stable::Wait(char *name)
{
	if (!name) return -1;

	for (int i = 0; i < MAX_SEMAPHORE; i++)
		if (semtab[i]
		&&  strncmp(semtab[i]->GetName(), name, 49) == 0) {
		/* found */
			semtab[i]->Wait();
			return 0;
		}

	/* no semaphore with *name* found */
	return -1;
}

int Stable::Signal(char *name)
{
	if (!name) return -1;

	for (int i = 0; i < MAX_SEMAPHORE; i++)
		if (semtab[i]
		&&  strncmp(semtab[i]->GetName(), name, 49) == 0) {
		/* found */
			semtab[i]->Signal();
			return 0;
		}

	/* no semaphore with *name* found */
	return -1;
}

#include "ptable.h"
#include "system.h"

Ptable::Ptable()
{
	bm    = new BitMap(MAX_PROCESS);
	bmsem = new Semaphore("bmsem", 1);
	for (int i = 0; i < MAX_PROCESS; i++)
		pcb[i] = NULL;
}

Ptable::~Ptable()
{
	delete bm;
	delete bmsem;
	for (int i = 0; i < MAX_PROCESS; i++)
		if (pcb[i]) {
			delete pcb[i];
			pcb[i] = NULL;
		}
}

char *Ptable::GetName(int pid)
{
	if (pid < 0) return NULL;

	for (int i = 0; i < MAX_PROCESS; i++)
		if (pcb[i] && pcb[i]->GetID() == pid)
			return pcb[i]->GetName();

	return NULL;
}

int Ptable::GetFreeSlot()
{
	/* the position is marked in bitmap
	 * after Find returned
	 */
	return bm->Find();
}

bool Ptable::isExist(int pid)
{
	if (pid < 0) return false;

	for (int i = 0; i < MAX_PROCESS; i++)
		if (pcb[i] && pcb[i]->GetID() == pid)
			return true;

	return false;
}

void Ptable::Remove(int pid)
{
	if (pid < 0) return;

	for (int i = 0; i < MAX_PROCESS; i++)
		if (pcb[i] && pcb[i]->GetID() == pid) {
			delete pcb[i];
			pcb[i] = NULL;
			bm->Clear(i);
			return;
		}
}

int Ptable::ExecUpdate(char *name)
{
	bmsem->P();

	if (!name) { bmsem->V(); return -1; }

	int pid  = -1;
	int ppid = -1;
	if (bm->NumClear() == MAX_PROCESS) { /* first process to exec */
		pid = 0;
		bm->Mark(pid);
	}
	else {
		if (strncmp(currentThread->getName(), name, 49) == 0) {
		/* a thread cannot exec itself */
			bmsem->V();
			return -1;
		}

		/* find current thread in ptable as parent of new thread */
		for (int i = 0; i < MAX_PROCESS; i++)
			if (pcb[i] && pcb[i]->GetThread() == currentThread) {
				ppid = i;
				break;
			}
		if (ppid == -1) { /* cannot find current thread in ptable */
			bmsem->V();
			return -1;
		}

		pid = this->GetFreeSlot();
		if (pid == -1) { /* cannot find free slot */
			bmsem->V();
			return -1;
		}
	}

	/* create ptable entry for new thread and call exec */
	pcb[pid] = new PCB();
	if (pcb[pid]->Exec(name, pid, ppid) == -1) {
	/* exec failed, delete ptable entry and return */
		bm->Clear(pid);
		delete pcb[pid];
		pcb[pid] = NULL;

		bmsem->V();
		return -1;
	}

	bmsem->V();
	return pid;
}

int Ptable::JoinUpdate(int pid)
{
	/* range check */
	if (pid < 0 || pid >= 10) return -1;

	/* find current thread */
	int which = -1;
	for (int i = 0; i < MAX_PROCESS; i++)
		if (pcb[i] && pcb[i]->GetThread() == currentThread) {
			which = i;
			break;
		}

	if (which == -1) return -1; /* cannot find current thread in ptable */

	/* current thread is not parent of join target */
	if (!pcb[pid] || pcb[pid]->GetParentID() != which)
		return -1;

	/* current process joins and
	 * waits until join target finished
	 */
	pcb[which]->JoinWait();

	/* get exit code and return */
	return pcb[which]->GetExitCode();
}

int Ptable::ExitUpdate(int ecd)
{
	/* find pcb of current thread in ptable */
	int which = -1;
	for (int i = 0; i < MAX_PROCESS; i++)
		if (pcb[i] && pcb[i]->GetThread() == currentThread) {
			which = i;
			break;
		}

	if (which == -1) /* cannot find current thread in ptable */
		return -1;

	int ppid = pcb[which]->GetParentID();
	if (ppid == -1) { /* current thread is init thread */
		interrupt->Halt();
	}
	else {
		/* parent release join of child */
		pcb[ppid]->SetExitCode(ecd);
		pcb[ppid]->JoinRelease();

		/* free exited process address space */
		delete currentThread->space;
		currentThread->Finish();
		/* delete current pcb entry */
		delete pcb[which];
		pcb[which] = NULL;
		bm->Clear(which);
	}

	return 0;
}

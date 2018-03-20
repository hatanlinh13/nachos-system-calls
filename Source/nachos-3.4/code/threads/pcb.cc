#include <cstring>

#include "pcb.h"
#include "system.h"

PCB::PCB()
{
	/* Set initial values */
	process_name[0] = '\0'; /* empty name */
	process_id = -1;
	parent_id  = -1;
	numwait    =  0;
	exitcode   =  0;
	thread     =  NULL;
	joinsem    = new Semaphore("joinsem", 0);
	exitsem    = new Semaphore("exitsem", 0);
	multex     = new Semaphore("multex" , 1);
}

PCB::~PCB()
{
	if (thread)  delete thread;
	if (joinsem) delete joinsem;
	if (exitsem) delete exitsem;
	if (multex)  delete multex;
}

void init_func(int)
{
	currentThread->space->InitRegisters();
	currentThread->space->RestoreState();
	machine->Run();
}

int PCB::Exec(char *name, int pid, int ppid)
{
	multex->P();

	if (!name || pid < 0) {
		multex->V();
		return -1;
	}

	if (thread) {
		multex->V();
		return -1;
	}

	process_id = pid;
	parent_id  = ppid;
	strncpy(process_name, name, 50);

	OpenFile *obj;
	if ((obj = fileSystem->Open(name)) == NULL) {
		multex->V();
		return -1;
	}

	thread = new Thread(process_name);
	thread->space = new AddrSpace(obj);

	if (ppid == -1) { /* init process, no parent */
		Thread *old_thread = currentThread;
		currentThread = thread;
		currentThread->setStatus(RUNNING);
		delete old_thread;

		thread->space->InitRegisters();
		thread->space->RestoreState();
	}
	else { /* has parent */
		thread->Fork(init_func, 0);
	}

	delete obj;
	multex->V();
	return 0;
}

void PCB::JoinWait()
{
	numwait++;
	joinsem->P();
}

void PCB::ExitWait()
{
	numwait++;
	exitsem->P();
}

void PCB::JoinRelease()
{
	numwait--;
	joinsem->V();
}

void PCB::ExitRelease()
{
	numwait--;
	exitsem->V();
}

char *PCB::GetName()
{
	return process_name;
}

int PCB::GetID()
{
	return process_id;
}

int PCB::GetParentID()
{
	return parent_id;
}

int PCB::GetNumWait()
{
	return numwait;
}

int PCB::GetExitCode()
{
	return exitcode;
}

void PCB::SetName(char *name)
{
	strncpy(process_name, name, 50);
}

void PCB::SetExitCode(int ec)
{
	exitcode = ec;
}

Thread *PCB::GetThread()
{
	return thread;
}

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#define MaxFileLength 32
char* User2System(int virtAddr, int limit)
{
    int i; // index
    int oneChar;
    char* kernelBuf = NULL;
    kernelBuf = new char[limit + 1]; //need for terminal string
    if (kernelBuf == NULL)
        return kernelBuf;
    memset(kernelBuf, 0, limit + 1);
    //printf("\n Filename u2s:");
    for (i = 0; i < limit; i++) {
        machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;
        //printf("%c",kernelBuf[i]);
        if (oneChar == 0)
            break;
    }
    return kernelBuf;
}
// Input: - User space address (int)
// - Limit of buffer (int)
// - Buffer (char[])
// Output:- Number of bytes copied (int)
// Purpose: Copy buffer from System memory space to User memory space
int System2User(int virtAddr, int len, char* buffer)
{
    if (len < 0)
        return -1;
    if (len == 0)
        return len;
    int i = 0;
    int oneChar = 0;
    do {
        oneChar = (int)buffer[i];
        machine->WriteMem(virtAddr + i, 1, oneChar);
        i++;
    } while (i < len && oneChar != 0);
    return i;
}
void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    switch (which) {
    case NoException:
        machine->registers[PrevPCReg] = machine->registers[PCReg];
        machine->registers[PCReg] = machine->registers[NextPCReg];
        machine->registers[NextPCReg] += 4;
        return;
    case SyscallException:
        switch (type) {
        case SC_Halt:
            DEBUG('a', "Shutdown, initiated by user program.\n");
            interrupt->Halt();
            break;

        case SC_Create: {
            int virtAddr;
            char* filename;
            DEBUG('a', "\n SC_Create call ...");
            DEBUG('a', "\n Reading virtual address of filename");
            //lay tham so ten tap tin tu thanh ghi r4
            virtAddr = machine->ReadRegister(4);
            DEBUG('a', "\n Reading filename.");
            filename = User2System(virtAddr, MaxFileLength + 1);
            if (filename == NULL) {
                printf("\n Not enough memory in system");
                DEBUG('a', "\n Not enough memory in system");
                machine->WriteRegister(2, -1); //tra ve loi cho chuong trinh
                //nguoi dung
                delete filename;
                return;
            }
            DEBUG('a', "\n Finish reading filename");

            if (!fileSystem->Create(filename, 0)) {
                printf("\n Error create file '%s'", filename);
                machine->WriteRegister(2, -1);
                delete filename;
                return;
            }
            machine->WriteRegister(2, 0);
            delete filename;
            //interrupt->Halt();
            break;
	}

        case SC_ReadInt: {
            DEBUG('a', "\n Read interger from console");
            int number = 0;
            int nDigit = 0;
            int i;
	    int not_a_number = 0;
	    int negative = 0;
            char* buffer = new char[MAX_INT_LENGTH];
            nDigit = gSynchConsole->Read(buffer, MAX_INT_LENGTH);
	    i = 0;
	    while (buffer[i] == ' ' || buffer[i] == '\t') i++;
	    if (buffer[i] == '-') {
	    	negative = 1;
		i++;
	    }
            for (; i < nDigit; ++i)
		if (buffer[i] >= '0' && buffer[i] <= '9') {
			number = number * 10 + (int)(buffer[i] & MASK_GET_NUM);
		}
		else {
			not_a_number = 1;
			break;
		}
	    if (not_a_number) {
		    machine->WriteRegister(2, 0);
	    }
	    else {
		    if (negative) number *= -1;
		    machine->WriteRegister(2, number);
	    }
            delete buffer;
            //interrupt->Halt();
            break;
	}

        case SC_PrintInt: {
            char s[MAX_INT_LENGTH], neg, tmp;
            neg = '-';
            int i, n, mid, sz;
            i = n = 0;
            DEBUG('a', "Read argument value at r4");
            n = machine->ReadRegister(4);
            if (n < 0) {
                gSynchConsole->Write(&neg, 1);
                n = -n;
            }
            do {
                s[i++] = n % 10 + '0';
            } while ((n /= 10) > 0);
            sz = i;
            s[sz] = '\n';
            mid = i / 2;
            while (i-- > mid) {
                tmp = s[sz - i - 1];
                s[sz - i - 1] = s[i];
                s[i] = tmp;
            }
            gSynchConsole->Write(s, sz);
            //interrupt->Halt();
            break;
	}

	case SC_ReadChar: {
            int sz;
            char buf[MAX_INT_LENGTH];
            sz = gSynchConsole->Read(buf, MAX_INT_LENGTH);
            machine->WriteRegister(2, buf[sz - 1]);
            //interrupt->Halt();
            break;
	}

        case SC_PrintChar: {
            char ch;
            ch = (char)machine->ReadRegister(4);
            gSynchConsole->Write(&ch, 1);
            //interrupt->Halt();
            break;
	}

        case SC_ReadString: {
            char* buf = new char[LIMIT];
            if (buf == 0)
                break;
            int bufAddrUser = machine->ReadRegister(4);
            int length = machine->ReadRegister(5);
            int sz = gSynchConsole->Read(buf, length);
            System2User(bufAddrUser, sz, buf);
            delete[] buf;
            //interrupt->Halt();
            break;
	}

        case SC_PrintString: {
            int bufAddr = machine->ReadRegister(4);
            int i = 0;
            char* buf;
            buf = User2System(bufAddr, LIMIT);
            while (buf[i] != 0) {
                gSynchConsole->Write(buf + i, 1);
                ++i;
            }
            delete[] buf;
            //interrupt->Halt();
            break;
	}
	case SC_CreateFile:
	{
		int bufAddr = machine->ReadRegister(4); // read string pointer from user
		char *buf = new char[LIMIT];
		buf = User2System(bufAddr, LIMIT);
		// create a new file 
		if (fileSystem->Create(buf, 0) == false)
		{
			DEBUG('f',"can not create file");
			machine->WriteRegister(2, -1);
		} else 
		{
			DEBUG('f',"create file successfully");
			machine->WriteRegister(2, 0);
		};
		delete [] buf;
		break;
	}
	case SC_OpenFileID:
	{
		int bufAddr = machine->ReadRegister(4); // read string pointer from user
		int type = machine->ReadRegister(5);
		char *buf = new char[LIMIT];
		int freeSlot = fileSystem->findFreeSlot();
		if (freeSlot >= 10)
		{
			machine->WriteRegister(2, -1);
			delete[] buf;
			break;
		}
		buf = User2System(bufAddr, LIMIT);
		if (strcmp(buf,"stdin") == 0)
		{
			printf("stdin mode\n");
			machine->WriteRegister(2, 0);
			break;
		}
		if (strcmp(buf,"stdout") == 0)
		{
			printf("stdout mode\n");
			machine->WriteRegister(2, 1);
			break;
		}
		if ((fileSystem->openf[freeSlot] = fileSystem->Open(buf, type)) != NULL)
		{
			DEBUG('f',"open file successfully");
			machine->WriteRegister(2, freeSlot);
		} else 
		{
			DEBUG('f',"can not open file");
			machine->WriteRegister(2, -1);
		};
		delete [] buf;
		break;
	}
	case SC_SeekFile:
	{
		int pos = machine->ReadRegister(4);
		int m_index = machine->ReadRegister(5);
		if (m_index < 0 || m_index >= 10)
		{
			machine->WriteRegister(2, -1);
			break;
		}
		// check openf[m_index]
		if (fileSystem->openf[m_index] == NULL)
		{
			printf("seek fail \n");
			machine->WriteRegister(2, -1);
			break;
		}
			pos = (pos == -1) ? fileSystem->openf[m_index]->Length() : pos;
		if (pos > fileSystem->openf[m_index]->Length() || pos < 0) {
			machine->WriteRegister(2, -1);
		} else 
		{
			fileSystem->openf[m_index]->Seek(pos);
			machine->WriteRegister(2, pos);
		}
	break;
	}
	case SC_ReadFile:
	{
		int bufAddr = machine->ReadRegister(4);
		int NumBuf = machine->ReadRegister(5);
		int m_index = machine->ReadRegister(6);	
		int OldPos;
		int NewPos;
		char *buf = new char[NumBuf];
		// Check m_index
		if (m_index < 0 || m_index >= 10)
		{
			machine->WriteRegister(2, -1);
			delete[] buf;
			break;
		}
		// check openf[m_index]
		if (fileSystem->openf[m_index] == NULL)
		{
			machine->WriteRegister(2, -1);
			delete[] buf;
			break;
		}
		OldPos = fileSystem->openf[m_index]->GetCurrentPos();
		buf = User2System(bufAddr, NumBuf);
		if (fileSystem->openf[m_index]->type == 2)
		{
			/*  printf("NumBuf = %d\n", NumBuf);*/
			int sz = gSynchConsole->Read(buf, NumBuf);
			/*  System2User(bufAddr, sz, buf);*/
		
			System2User(bufAddr, sz, buf);
			machine->WriteRegister(2, sz);
			break;
		}
		
		if ((fileSystem->openf[m_index]->Read(buf, NumBuf) ) > 0)
		{
			// Copy data from kernel to user space
		  NewPos = fileSystem->openf[m_index]->GetCurrentPos();
			System2User(bufAddr, NewPos - OldPos +1, buf);
			machine->WriteRegister(2, NewPos - OldPos + 1);
		}
		else
		{
			machine->WriteRegister(2, -1);
			delete[] buf;
			break;
		}
		// read data from console 
		
		/*  
		if (fileOpen.type == 2)
		{
			int sz = gSynchConsole->Read(buf, NumBuf);
			System2User(bufAddr, sz, buf);
			machine->WriteRegister(2, sz);
		}*/
		delete[] buf;
		break;
	}
	case SC_WriteFile:
	{
		int bufAddr = machine->ReadRegister(4);
		int NumBuf = machine->ReadRegister(5);
		int m_index =  machine->ReadRegister(6);
		int OldPos;
		int NewPos;
		char *buf = new char[NumBuf];
		// Check m_index
		if (m_index < 0 || m_index >= 10)
		{
			machine->WriteRegister(2, -1);
			delete[] buf;
			break;
		}
		// check openf[m_index]
		if (fileSystem->openf[m_index] == NULL)
		{
			machine->WriteRegister(2, -1);
			delete[] buf;
			break;
		}
		OldPos = fileSystem->openf[m_index]->GetCurrentPos();
	
		// type must equals '0'
		buf = User2System(bufAddr, NumBuf);
		if (fileSystem->openf[m_index]->type  == 0 || fileSystem->openf[m_index]->type == 3)
		{	
		if ((fileSystem->openf[m_index]->Write(buf, NumBuf)) > 0) 
		{
			// Copy data from kernel to user space
			NewPos = fileSystem->openf[m_index]->GetCurrentPos();
			machine->WriteRegister(2, NewPos - OldPos + 1);
		}
		else if (fileSystem->openf[m_index]->type == 1)
		{
			machine->WriteRegister(2, -1);
			delete[] buf;
			break;
		}
		}
		// Write data to console
		if (fileSystem->openf[m_index]->type == 3)
		{
			int i = 0;
			while (buf[i] != 0)
			{
				gSynchConsole->Write(buf+i, 1);
				i++;
			}
			machine->WriteRegister(2, i-1);
		}
		delete[] buf;
		break;
	}
	case SC_CloseFile:
	{
		int m_index = machine->ReadRegister(4);
		if (fileSystem->openf[m_index] == NULL) break;
		delete fileSystem->openf[m_index];
		fileSystem->openf[m_index] = NULL;
		break;
	}

	case SC_ExecProcess:
	{
		int name = machine->ReadRegister(4);
		if (!name) {
			machine->WriteRegister(2, -1);
			break;
		}

		char *namebuf;
		namebuf = User2System(name, 49);

		int rslt = ptab->ExecUpdate(namebuf);

		delete[] namebuf;
		machine->WriteRegister(2, rslt);
		break;
	}

	case SC_JoinProcess:
	{
		int pid  = machine->ReadRegister(4);
		int rslt = ptab->JoinUpdate(pid);
		machine->WriteRegister(2, rslt);
		break;
	}

	case SC_ExitProcess:
	{
		int pid  = machine->ReadRegister(4);
		int rslt = ptab->ExitUpdate(pid);
		machine->WriteRegister(2, rslt);
		break;
	}

	case SC_CreateSem:
	{
		int name = machine->ReadRegister(4);
		int sval = machine->ReadRegister(5);
		if (!name || sval < 0) {
			machine->WriteRegister(2, -1);
			break;
		}

		char *namebuf = User2System(name, 49);

		int rslt = stab->Create(namebuf, sval);
		delete[] namebuf;
		machine->WriteRegister(2, rslt);
		break;
	}

	case SC_Wait:
	{
		int name = machine->ReadRegister(4);
		if (!name) {
			machine->WriteRegister(2, -1);
			break;
		}

		char *namebuf = User2System(name, 49);

		int rslt = stab->Wait(namebuf);
		delete[] namebuf;
		machine->WriteRegister(2, rslt);
		break;
	}

	case SC_Signal:
	{
		int name = machine->ReadRegister(4);
		if (!name) {
			machine->WriteRegister(2, -1);
			break;
		}

		char *namebuf = User2System(name, 49);

		int rslt = stab->Signal(namebuf);
		delete[] namebuf;
		machine->WriteRegister(2, rslt);
		break;
		break;
	}

	case SC_Exit:
	    interrupt->Halt();
	    break;

        default:
            printf("\nUnexpected user mode exception (%d, %d)\n", which, type);
            interrupt->Halt();
            break;
        }
        machine->registers[PrevPCReg] = machine->registers[PCReg];
        machine->registers[PCReg] = machine->registers[NextPCReg];
        machine->registers[NextPCReg] += 4;
        break;
    case PageFaultException:
        printf("\n No valid translation found");
        interrupt->Halt();
        break;
    case ReadOnlyException:
        printf("\n Write to read-only");
        interrupt->Halt();
        break;
    case BusErrorException:
        printf("\n Translation result in an invalid physical address");
        interrupt->Halt();
        break;
    case AddressErrorException:
        printf("\n Unaligned reference or one that was beyond the end of the address space");
        interrupt->Halt();
        break;
    case OverflowException:
        printf("\n Integer overflow");
        interrupt->Halt();
        break;
    case IllegalInstrException:
        printf("\n Unimplemented or reserved instr");
        interrupt->Halt();
        break;
    case NumExceptionTypes:
        printf("\n Num exception types");
        interrupt->Halt();
        break;
    }
}

#include "syscall.h"

int main(){
    SpaceId a, b;
    CreateSemaphore("a", 1);
    CreateSemaphore("b", 0);
    a = ExecProcess("./test/printa");
    b = ExecProcess("./test/printb");
    JoinProcess(a);
    JoinProcess(b);

    Exit(0);
}

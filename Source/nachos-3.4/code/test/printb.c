#include "syscall.h"

int main(){
    int i = 0;
    for (; i < 50; ++i){
        Wait("b");
        PrintString("B");
        Signal("a");
    }
    Exit(0);
}

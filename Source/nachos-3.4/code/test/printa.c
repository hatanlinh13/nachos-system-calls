#include "syscall.h"

int main(){
    int i = 0;
    for (; i < 50; ++i){
        Wait("a");
        PrintString("A");
        Signal("b");
    }
    Exit(0);
}

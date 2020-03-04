#include <iostream>
#include "threadpool.h"

void fun(int number){
    printf("0x%lx",pthread_self());
    std::cout <<  "is doing this work" << number << std::endl;
    /* sleep(5); */
}

int main()
{
    Threadpool threadpool;
    threadpool.init(4);
    for(int i = 0;i<100;i++){
        threadpool.addTask(fun,1024);
        threadpool.addTask(fun,1025);
    }
    while(true);
    /* threadpool.destroy(); */
    return 0;
}


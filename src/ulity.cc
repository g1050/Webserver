/*
 * @Author: sky
 * @Date: 2020-12-26 22:21:01
 * @LastEditors: sky
 * @LastEditTime: 2020-12-30 08:38:15
 * @Description: 
 */
#include "../include/ulity.h"

void setnonBlocking(int listenfd){

    int flag = fcntl(listenfd,F_GETFL,0);
    check_exit(flag != -1,"setNonBlocking error");
    flag |= SOCK_NONBLOCK;
    check_exit(fcntl(listenfd,F_SETFL,flag) != -1,"setNonBlocking error");

}



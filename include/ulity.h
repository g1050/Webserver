#ifndef ULITY_H
#define ULITY_H
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <sys/epoll.h>
#include <signal.h>
#include <memory>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <functional>
#include <memory>
#include <pthread.h>
#include "debug.h"

#define MAXSIZE 4096
//#define LISTENQ 1024 //Now useless.
//

void setnonBlocking(int listenfd);

#endif

#include <map>
#include <time.h>
#include <iostream>
#include <chrono>
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <condition_variable>

#include "manager.h"
#include "payload.pb.h"
#include "user.h"

using namespace std;
using namespace google::protobuf;

// TODO: define all functions of manager.h header...
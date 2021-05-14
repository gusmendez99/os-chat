#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ifaddrs.h>
#include <map>
#include <vector>
#include <algorithm>
#include "payload.pb.h"
#include "user_message.h"

#define DELAY 30000
#define BUFSIZE 4096

using namespace std;
using namespace google::protobuf;

map<string, vector<UserMessage *>> private_messages;
#include <time.h>
#include <iostream>
#include <string>

using namespace std;

enum UserStatus { ACTIVE, BUSY, INACTIVE };

class User
{
public:
  string username; // unique
  UserStatus status;
  string ip_address; // unique
  int socket;
  string to_string();
  string get_status();
  
};

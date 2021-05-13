#include <time.h>
#include <iostream>
#include <string>
#include <netinet/in.h>

using namespace std;

enum UserStatus { ACTIVE, BUSY, INACTIVE };

class User
{
public:
  User();
  string username; // unique
  UserStatus status;
  char ip_address[INET_ADDRSTRLEN]; // unique
  int socket;
  string to_string();
  string get_status();
  void set_status(string new_status);
  
};

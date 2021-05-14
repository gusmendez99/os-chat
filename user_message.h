#include <time.h>
#include <iostream>
#include <string>

using namespace std;

class UserMessage
{
public:
  UserMessage(string username, string message);
  string username;
  string message;
  int seen;
  string to_string();
};
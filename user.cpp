#include <iostream>
#include <string>
#include <time.h>
#include "user.h"

using namespace std;

User::User()
{
	username = "";
	ip_address = "0.0.0.0";
	socket = 0;
	status = ACTIVE;
}
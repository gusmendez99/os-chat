#include <iostream>
#include <string>
#include <string.h>
#include <time.h>
#include "user.h"

using namespace std;

User::User()
{
	username = "";
	socket = 0;
	status = ACTIVE;
}

string User::to_string() 
{
	string data;
	data = "\t" + username + "\t" + ip_address + "\t" + User::get_status();
	return data;
}

string User::get_status()
{
	if(status==ACTIVE)
		return "Activo";
	else if (status==BUSY)
	 	return "Ocupado";
	else
	 	return "Inactivo";
}

void User::set_status(string new_status)
{
	if(strcmp(new_status.c_str(), "Activo") == 0)
		status = ACTIVE;
	else if (strcmp(new_status.c_str(), "Ocupado") == 0)
	 	status = BUSY;
	else
	 	status = INACTIVE;
}
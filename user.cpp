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

string User::to_string() 
{
	string data;
	data = "User: " + username + "\tIP: " + ip_address + "\tStatus: " + User::get_status();
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
#include <iostream>
#include <string>
#include <string.h>
#include "user_message.h"

using namespace std;

UserMessage::UserMessage(string username, string message)
{
    username = username;
    message = message;
    seen = 0;
}

string UserMessage::to_string()
{
    string data;
    data = "\t" + username + "\t" + message;
    return data;
}

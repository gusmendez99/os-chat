#include <map>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <netinet/in.h>
#include <pthread.h>
#include "user.h"

using namespace std;

#define BUFFER_SIZE 1024

class Manager
{
public:
    Manager(int socket, int client_id, struct sockaddr_in address);

    char buffer[BUFFER_SIZE];
    bool is_connected;
    int client_id, socket;
    time_t execution_time;
    struct sockaddr_in address;
    socklen_t length;
    User user;

    // Connection
    void handle_client();
    int add_client(User user);
    void terminate();
    // Utils
    void raise_error(const char *msg);
    void handle_received_error(int code);
    void read_map();
    void add_user_to_map(User user);
    bool exists_user(string username);
    bool exists_ip(string ip);
    int get_user_id_by_username(string username);
    void update_user_status();
    // Messages
    void send_broadcast(string message);
    void send_private(string message, int user_id, string username);
    void send_error(int user_id, string error);
    void confirm_user_register(int user_id);
    // Chat state
    void handle_chat_options();
    void sync_chat(string username, string ip);
    void send_connected_users(int user_id, string username);
    void change_user_status(string status);

private:
    static map<int, User> users;
};
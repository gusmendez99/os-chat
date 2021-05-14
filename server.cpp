#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <condition_variable>
#include <map>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "user.h"
#include "payload.pb.h"

#define IP "127.0.0.1"
#define BUFFER_SIZE 4096
#define MAX_CLIENTS 20
#define DEFAULT_SENDER "Server"
#define HTTP_OK 200
#define HTTP_INTERNAL_ERROR 500

using namespace std;

pthread_t thread_pool[MAX_CLIENTS];
void *retvals[MAX_CLIENTS];

map<string, User *> users = {};

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void send_response(int socket, string sender, string message, int code, char *buffer)
{
    string binary;
    Payload *response = new Payload();
    response->set_sender(sender);
    response->set_message(message);
    response->set_code(code);

    response->SerializeToString(&binary);

    strcpy(buffer, binary.c_str());
    send(socket, buffer, binary.size() + 1, 0);
}

void send_error(int socket, string message)
{
    string binary;
    Payload *error_payload = new Payload();
    error_payload->set_sender("server");
    error_payload->set_message(message);
    error_payload->set_code(500);
    error_payload->SerializeToString(&binary);

    char buffer[binary.size() + 1];
    strcpy(buffer, binary.c_str());
    int sent = send(socket, buffer, sizeof buffer, 0);
    if (sent == 0)
    {
        fprintf(stderr, "ERROR sending response to current client\n");
    }
}

void *handle_client_connected(void *params)
{
    char buffer[BUFFER_SIZE];
    User current_user;
    User *new_user = (User *)params;
    int own_socket = new_user->socket;

    // Received payload
    Payload payload;

    while (1)
    {
        int received = recv(own_socket, buffer, BUFFER_SIZE, 0);
        if (received > 0)
        {
            if (*buffer == '#')
            {
                printf("SERVER - Lost connection with the client %s...\n", current_user.username);
            }
            else
            {
                // Handle Proto Options
                payload.ParseFromString(buffer);

                // USER REGISTER
                if (payload.flag() == Payload_PayloadFlag_register_)
                {
                    printf("New server registration request with username: %s\n", new_user->username);

                    if (users.count(payload.sender()) == 0)
                    {
                        string response_message = "User registered successfully";
                        send_response(own_socket, DEFAULT_SENDER, response_message, HTTP_OK, buffer);
                        printf("User %s entered to chat\n", new_user->username);

                        // Guardar informacion de nuevo cliente
                        current_user.username = payload.sender();
                        current_user.socket = own_socket;
                        current_user.status = ACTIVE;

                        strcpy(current_user.ip_address, new_user->ip_address);
                        users[current_user.username] = &current_user;
                    }
                    else
                    {
                        send_error(own_socket, "Username has already been taken");
                    }
                }

                // CONNECTED USERS
                else if (payload.flag() == Payload_PayloadFlag_user_list)
                {
                    printf("Listing all users for user: %s\n", new_user->username);

                    Payload *response = new Payload();
                    string response_message = "\tUSERNAME\tIP\tESTADO\n";
                    map<string, User *>::iterator itr;

                    for (itr = users.begin(); itr != users.end(); ++itr)
                    {
                        User *u = itr->second;
                        response_message = response_message + u->to_string() + "\n";
                    }
                    send_response(own_socket, DEFAULT_SENDER, response_message, HTTP_OK, buffer);
                }

                // USER INFO
                else if (payload.flag() == Payload_PayloadFlag_user_info)
                {
                    if (users.count(payload.extra()) > 0)
                    {
                        printf("Retrieving info of user '%s', action performed by user: %s\n", payload.extra(), new_user->username);
                        string response_message = "\tUSERNAME\tIP\tESTADO\n";
                        User *user_retrieved = users[payload.extra()];

                        response_message = response_message + user_retrieved->to_string() + "\n";
                        send_response(own_socket, DEFAULT_SENDER, response_message, HTTP_OK, buffer);
                    }
                    else
                    {
                        printf("User does not exists on server");
                        send_error(own_socket, "User does not exists on server");
                    }
                }

                // CHANGE USER STATUS
                else if (payload.flag() == Payload_PayloadFlag_update_status)
                {
                    printf("Status change request for user: %s (%s)\n", new_user->username, payload.extra());
                    current_user.set_status(payload.extra());

                    string response_message = "Status updated to " + current_user.get_status();
                    send_response(own_socket, DEFAULT_SENDER, response_message, HTTP_OK, buffer);
                }

                // BROADCAST
                else if (payload.flag() == Payload_PayloadFlag_general_chat)
                {
                    printf("Broadcast message received from user: %s\n", new_user->username);
                    string response_message = "Broadcast message was sent successfully";
                    send_response(own_socket, DEFAULT_SENDER, response_message, HTTP_OK, buffer);

                    // Real broadcast message
                    Payload *broadcast = new Payload();
                    broadcast->set_sender(DEFAULT_SENDER);
                    broadcast->set_message("BM (" + payload.sender() + "): " + payload.message() + "\n");
                    broadcast->set_code(HTTP_OK);

                    string binary;
                    broadcast->SerializeToString(&binary);
                    strcpy(buffer, binary.c_str());

                    map<string, User *>::iterator itr;
                    for (itr = users.begin(); itr != users.end(); ++itr)
                    {
                        User *u = itr->second;
                        send(u->socket, buffer, binary.size() + 1, 0);
                    }
                }

                // PRIVATE MESSAGE
                else if (payload.flag() == Payload_PayloadFlag_private_chat)
                {
                    if (users.count(payload.extra()) > 0)
                    {
                        printf("Private message received from user: %s, to: %s\n", new_user->username, payload.extra());

                        string response_message = "Private message was sent successfully to: " + payload.extra();
                        send_response(own_socket, DEFAULT_SENDER, response_message, HTTP_OK, buffer);

                        User *recipient = users[payload.extra()];
                        // Real private message
                        Payload *private_msg = new Payload();
                        private_msg->set_sender(current_user.username);
                        private_msg->set_message("PM (" + payload.sender() + "): " + payload.message() + "\n");
                        private_msg->set_code(HTTP_OK);

                        string binary;
                        private_msg->SerializeToString(&binary);
                        strcpy(buffer, binary.c_str());
                        send(recipient->socket, buffer, binary.size() + 1, 0);
                    }
                    else
                    {
                        printf("User for PM does not exists on server");
                        send_error(own_socket, "User for PM does not exists on server");
                    }
                }

                // OPTION ERROR
                else
                {
                    send_error(own_socket, "Server couldn't match selected option...");
                }
            }
        }
    }

    // Disconnnect user from socket
    printf("Disconnecting user %s from server...", new_user->username);
    users.erase(new_user->username);
    close(own_socket);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);

    sockaddr_in server_addr;
    sockaddr_in client_addr;
    socklen_t client_size;
    int socket_fd = 0, conn_fd = 0;
    char *ip = IP;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0)
    {
        fprintf(stderr, "Error creating socket...\n");
        return EXIT_FAILURE;
    }

    if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        fprintf(stderr, "Error on binding IP...\n");
        return EXIT_FAILURE;
    }

    if (listen(socket_fd, 5) < 0)
    {
        fprintf(stderr, "Socket listening failed...\n");
        return EXIT_FAILURE;
    }

    printf("SERVER RUNNING ON PORT %d\n", port);

    while (1)
    {
        client_size = sizeof client_addr;
        conn_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_size);

        // TODO: Manage a limited number of clients connected...
        User new_user;
        new_user.socket = conn_fd;
        inet_ntop(AF_INET, &(client_addr.sin_addr), new_user.ip_address, INET_ADDRSTRLEN);

        // Threads
        pthread_t thread_id;
        pthread_attr_t attrs;

        pthread_attr_init(&attrs);
        pthread_create(&thread_id, &attrs, handle_client_connected, (void *)&new_user);

        // Reduce CPU usage
        sleep(1);
    }

    close(socket_fd);
    return EXIT_SUCCESS;
}

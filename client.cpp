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
#include <queue>
#include "payload.pb.h"
#include "user_message.h"

#define IP "127.0.0.1"
#define DELAY 30000
#define BUFFER_SIZE 4096

using namespace std;
using namespace google::protobuf;

map<string, vector<UserMessage *>> private_messages;
bool has_server_response, has_blocked_gui_interaction, is_connected;
queue<std::string> messages_queue;

// Ncurses Config
void make_line(WINDOW *w,int x, int y)
{
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    attron(COLOR_PAIR(1));

    wmove(w, y, 0);
    whline(w,' ', x);

    attroff(COLOR_PAIR(1));
    wmove(w, y+1, 0);
}

void make_main_menu(int option)
{
    mvprintw(0, 0,"Main Menu");

    if(option==1)
        {

            attron(COLOR_PAIR(1));
            mvprintw(2, 0,"Show is_connected users");
            attroff(COLOR_PAIR(1));//se apaga el color
            mvprintw(3, 0,"Change status");
            mvprintw(4, 0,"Send broadcast message");
            mvprintw(5, 0,"Send private message");
            mvprintw(6, 0,"Help");
            mvprintw(7, 0,"Exit");

        }
        else if(option==2)
        {
            mvprintw(2, 0,"Show is_connected users");
            attron(COLOR_PAIR(1));
            mvprintw(3, 0,"Change status");
            attroff(COLOR_PAIR(1));//se apaga el color
            mvprintw(4, 0,"Send broadcast message");
            mvprintw(5, 0,"Send private message");
            mvprintw(6, 0,"Help");
            mvprintw(7, 0,"Exit");
        }
        else if(option==3)
        {
            mvprintw(2, 0,"Show is_connected users");
            mvprintw(3, 0,"Change status");
            attron(COLOR_PAIR(1));
            mvprintw(4, 0,"Send broadcast message");
            attroff(COLOR_PAIR(1));//se apaga el color
            mvprintw(5, 0,"Send private message");
            mvprintw(6, 0,"Help");
            mvprintw(7, 0,"Exit");
        }
        else if(option==4)
        {
            mvprintw(2, 0,"Show is_connected users");
            mvprintw(3, 0,"Change status");
            mvprintw(4, 0,"Send broadcast message");
            attron(COLOR_PAIR(1));
            mvprintw(5, 0,"Send private message");
            attroff(COLOR_PAIR(1));//se apaga el color
            mvprintw(6, 0,"Help");
            mvprintw(7, 0,"Exit");
        }
        else if(option==5)
        {
            mvprintw(2, 0,"Show is_connected users");
            mvprintw(3, 0,"Change status");
            mvprintw(4, 0,"Send broadcast message");
            mvprintw(5, 0,"Send private message");
            attron(COLOR_PAIR(1));
            mvprintw(6, 0,"Help");
            attroff(COLOR_PAIR(1));//se apaga el color
            mvprintw(7, 0,"Exit");
        }
        else if(option==6)
        {
            mvprintw(2, 0,"Show is_connected users");
            mvprintw(3, 0,"Change status");
            mvprintw(4, 0,"Send broadcast message");
            mvprintw(5, 0,"Send private message");
            mvprintw(6, 0,"Help");
            attron(COLOR_PAIR(1));
            mvprintw(7, 0,"Exit");
            attroff(COLOR_PAIR(1));//se apaga el color
        }
        

}



void print_status_options(int position)
{
    mvprintw(0, 0,"Status change");

    if(position==0)
    {
        attron(COLOR_PAIR(1));
        mvprintw(2, 0,"Active"); 
        attroff(COLOR_PAIR(1));
        mvprintw(3, 0,"Busy"); 
        mvprintw(4, 0,"Inactive");
    }
    else if(position==1)
    {
        mvprintw(2, 0,"Active"); 
        attron(COLOR_PAIR(1));
        mvprintw(3, 0,"Busy"); 
        attroff(COLOR_PAIR(1));
        mvprintw(4, 0,"Inactive");
    }
    else if(position==2)
    {
        mvprintw(2, 0,"Active"); 
        mvprintw(3, 0,"Busy"); 
        attron(COLOR_PAIR(1));
        mvprintw(4, 0,"Inactive");
        attroff(COLOR_PAIR(1));
    }

      

}

void print_help()
{
    mvprintw(0, 0,"Help");

    mvprintw(2, 0,  "--------------------------------------------");
    mvprintw(3, 0,  "To navigate you use the keyboard arrows, to select an option you use the Enter key and return you use the ESC key.");

    mvprintw(6, 0,  "MENU OPTIONS:");
    
    mvprintw(8, 0,  "Show is_connected users:");
    mvprintw(9, 0,  "The information of the users who are registered in the server is displayed, to see extra information, select a user.");

    mvprintw(12, 0,  "Change status:");
    mvprintw(13, 0,  "This option allows you to change the status of the user (Active, Busy, Inactive).");

    mvprintw(16, 0,  "Broadcast message:");
    mvprintw(17, 0,  "Chat with all is_connected users.");

    mvprintw(19, 0,  "Private message:");
    mvprintw(20, 0,  "Private chat with a single user.");    
}


void *listen(void *args){
    while (1)
	{
		char buffer[BUFFER_SIZE];
		int received_message = recv(*(int *)args, buffer, BUFFER_SIZE, 0);
        
		Payload payload;
		payload.ParseFromString(buffer);
        string message = payload.message();


		if (payload.code() != 500 || payload.code() != 200) {
            cout << "SERVER ERROR WHILE LISTENING SERVER..." << endl;
        }
		
        if (payload.code() == 500){
			cout << "SERVER ERROR: " << message << endl;
		}

		while (!has_blocked_gui_interaction && !messages_queue.empty())
		{
			printf("%s\n", messages_queue.front().c_str());
			messages_queue.pop();
		}

		has_server_response = false;
		if (!is_connected) pthread_exit(0);
	}
}


int main(int argc, char *argv[]) 
{
    /*
        The gethostbyname() and gethostbyaddr() functions are deprecated on 
        most platforms, and they don't implement support for IPv6. 
        So, we need a collection of hints for the IP field
    */
    int socket_fd, err;
	struct addrinfo hints = {}, *addrs, *addr;
	char host_ip[INET_ADDRSTRLEN];
	char buffer[BUFFER_SIZE];
	char *username, *server, *port;
    
    GOOGLE_PROTOBUF_VERIFY_VERSION;

	if (argc != 4)
	{
		printf("Usage: %s <username> <server_ip> <port>\n", argv[0]);
		return EXIT_FAILURE;
	}

    username = argv[1];
    server = argv[2];
    port = argv[3];

    // Socket connection
    memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET; // Since your original code was using sockaddr_in and
                               // PF_INET, I'm using AF_INET here to match.  Use
                               // AF_UNSPEC instead if you want to allow getaddrinfo()
                               // to find both IPv4 and IPv6 addresses for the hostname.
                               // Just make sure the rest of your code is equally family-
                               // agnostic when dealing with the IP addresses associated
                               // with this connection. For instance, make sure any uses
                               // of sockaddr_in are changed to sockaddr_storage,
                               // and pay attention to its ss_family field, etc...
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    err = getaddrinfo(server, port, &hints, &addrs);
	if (err != 0)
	{
		fprintf(stderr, "%s: %s\n", server, gai_strerror(err));
		return EXIT_FAILURE;
	}

	for (addr = addrs; addr != NULL; addr = addr->ai_next)
	{
		if ((socket_fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) == -1)
		{
			continue;
		}

		if (connect(socket_fd, addr->ai_addr, addr->ai_addrlen) == -1)
		{
			close(socket_fd);
			continue;
		}
		break;
	}

	if (addr == NULL)
	{
		printf("ERROR ON GET HOST IP\n");
		return EXIT_FAILURE;
	}

	inet_ntop(
        addr->ai_family, 
        &((struct sockaddr_in *) addr->ai_addr)->sin_addr, 
        host_ip, 
        sizeof host_ip
    );
	freeaddrinfo(addrs);

	printf("STARTING HANDSHAKE WITH OWN IP: %s\n", host_ip);
    // Server handshake
    string message_serialized;
    Payload *handshake_payload = new Payload;

	handshake_payload->set_sender(username);
	handshake_payload->set_flag(Payload_PayloadFlag_register_);
	handshake_payload->set_ip(host_ip);
	handshake_payload->SerializeToString(&message_serialized);

	strcpy(buffer, message_serialized.c_str());
	send(socket_fd, buffer, message_serialized.size() + 1, 0);
	recv(socket_fd, buffer, BUFFER_SIZE, 0);

	Payload server_payload;
	server_payload.ParseFromString(buffer);
    string server_message = server_payload.message();

	if(server_payload.code() != 200) {
		printf("ERROR: SERVER RESPONSE WAS: %s\n", server_message.c_str());
        return EXIT_FAILURE;
    }
	is_connected = true;

	pthread_t client_thread_id;
	pthread_attr_t attrs;

	pthread_attr_init(&attrs);
	pthread_create(&client_thread_id, &attrs, listen, (void *)&socket_fd);

    printf("Finally connected...\n");

	
	// Close connection
	pthread_cancel(client_thread_id);
	is_connected = 0;
	close(socket_fd);
	return EXIT_SUCCESS;
}

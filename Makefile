all: payload.proto server client

server: server.cpp payload.pb.cc
	g++ -o server server.cpp payload.pb.cc user.cpp -lpthread -lprotobuf

# TODO: complete client-make option
client: client.cpp payload.pb.cc
	g++ -o client client.cpp payload.pb.cc user_message.cpp -lprotobuf -lncurses -pthread

payload.proto:
	protoc -I=. --cpp_out=. payload.protoc

make:
	protoc -I=. --cpp_out=. payload.proto
	# TODO: complete server & client files
	# g++ -o SERVER_FILES -lpthread -lprotobuf
	# g++ CLIENT_FILES -lprotobuf -lncurses -pthread
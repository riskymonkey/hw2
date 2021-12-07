#include "chap03.h"

#if defined(_WIN32)
#include <conio.h>
#endif

void de_n(char *ptr){
    char *qtr = strchr(ptr, '\n');
    if(qtr){
        *qtr = '\0';
    }
}

//设置服务器监听端口号
const unsigned long port = 8080;

int main(int argc, char *argv[]) {

#if defined(_WIN32)
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d)) {
        fprintf(stderr, "Failed to initialize.\n");
        return 1;
    }
#endif

    //创建服务器socket，地址族为AF_INET(IPv4)，传输方式为TCP
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//初始化IP为服务器127.0.0.1，端口为已设置的port
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//客户端连接服务器
	connect(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("Connected.\n");
    printf("To send data, enter text followed by enter.\n");

    while(1) {

        fd_set reads;
        FD_ZERO(&reads);
        FD_SET(server_socket, &reads);
#if !defined(_WIN32)
        FD_SET(0, &reads);
#endif

        struct timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 100000;

        if (select(server_socket+1, &reads, 0, 0, &timeout) < 0) {
            fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
            return 1;
        }
        //server response
        if (FD_ISSET(server_socket, &reads)) {
            char read[4096];
            char *logout = "Logout...\n";
            memset(read, '\0', sizeof(read));
            int bytes_received = recv(server_socket, read, 4096, 0);
            if (bytes_received < 1) {
                printf("Connection closed by peer.\n");
                break;
            }
            printf("\n%s\n", read);
            if( strcmp( logout, read ) == 0 ) break;
        }

#if defined(_WIN32)
        if(_kbhit()) {
#else
        if(FD_ISSET(0, &reads)) {
#endif
            //user input
            char read_buffer[4096];
            if (!fgets(read_buffer, 4096, stdin)) break;
            de_n(read_buffer);
            //if( read[strlen(read)-1] == '\n' ) read[strlen(read)-1] = '\0';
            int bytes_sent = send(server_socket, read_buffer, strlen(read_buffer), 0);

        }
    } //end while(1)

    printf("Closing socket...\n");
    CLOSESOCKET(server_socket);

#if defined(_WIN32)
    WSACleanup();
#endif

    printf("Finished.\n");
    return 0;
}


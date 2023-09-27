#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void setupAddressStruct(struct sockaddr_in* address, int PORT){
 
    // clear address struct
    memset((char*) address, '\0', sizeof(*address)); 

    address->sin_family = AF_INET;
    address->sin_port = htons(PORT);
    address->sin_addr.s_addr = INADDR_ANY;
}

int main(int argc, char *argv[]) {
    
    int conn_socket, ch_read;
    char* buffer;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_size = sizeof(client_addr);
    int PORT = atoi(argv[1]);

    // Check usage & args
    if (argc < 2) { 
        fprintf(stderr,"USAGE: %s port\n", argv[0]); 
        exit(1);
    } 

    // create socket that listens
    int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket < 0) {
        error("enc_server: error opening socket");
    }

    // address struct for server socket
    setupAddressStruct(&server_addr, PORT);

    // bind socket and address struct
    if (bind(listen_socket, (const struct sockaddr *)& server_addr, sizeof(server_addr)) < 0){
        error("enc_server: error on binding");
    }

    // listen with queue of 5
    listen(listen_socket, 5);

    // continue to listen
    while(1){
        // Accept the connection request which creates a connection socket
        conn_socket = accept(listen_socket, (struct sockaddr *)& client_addr, &client_size); 
        if (conn_socket < 0){
            close(conn_socket);
            exit(1);
        }

        int fork_result = fork();
        if (fork_result == 0) {
            
            // get identity
            buffer = malloc(19);
            memset(buffer, '\0', 19);
            ch_read = recv(conn_socket, buffer, 19, 0);

            // characters read error
            if (ch_read < 19){
                close(conn_socket);
                exit(1);
            }

            // not connected to proper server
            if (strncmp(buffer, "enc_client:", 11) != 0) {
                // send error
                ch_read = send(conn_socket, "errorerror", 10, 0);
                close(conn_socket);
                exit(1);
            } else {
                // send identity
                ch_read = send(conn_socket, "enc_server", 10, 0);
                if (ch_read < 10){
                    close(conn_socket);
                    exit(1);
                }
            }

            ////////////////////////////////////////////

            // get message size
            int message_len;
            sscanf(buffer+11, "%X", &message_len);
            
            free(buffer);
            buffer = malloc(message_len);
            memset(buffer, '\0', message_len);

            printf("%d\n", message_len);

            // get plaintext and key
            ch_read = recv(conn_socket, buffer, message_len, 0);
            if (ch_read < message_len){
                close(conn_socket);
                exit(1);
            }
        
            // send back message
            ch_read = send(conn_socket, buffer, strlen(buffer), 0);
            if (ch_read < strlen(buffer)){
                close(conn_socket);
                exit(1);
            }
            
        } else {
            printf("enc_server: client w/host %d and port %d on child %d\n", ntohs(client_addr.sin_addr.s_addr), ntohs(client_addr.sin_port), fork_result);
        }
    }

    // Close the connection socket for this client
    close(conn_socket);

    // Close the listening socket
    close(listen_socket); 
    return 0;
}
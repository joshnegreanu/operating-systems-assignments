#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// Error function used for reporting issues
void error(const char *msg) { 
    perror(msg); 
    exit(0);
} 

// Set up the address struct
void setupAddressStruct(struct sockaddr_in* address, int portNumber, char* hostname){

    // Clear out the address struct
    memset((char*) address, '\0', sizeof(*address)); 

    // The address should be network capable
    address->sin_family = AF_INET;
    // Store the port number
    address->sin_port = htons(portNumber);

    // Get the DNS entry for this host name
    struct hostent* hostInfo = gethostbyname(hostname); 
    if (hostInfo == NULL) { 
        fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
        exit(1);
    }
    
    // Copy the first IP address from the DNS entry to sin_addr.s_addr
    memcpy((char*) &address->sin_addr.s_addr, hostInfo->h_addr_list[0], hostInfo->h_length);
}

int main(int argc, char *argv[]) {

    int socket_fd, PORT, ch_written, ch_read;
    struct sockaddr_in server_addr;
    char* buffer;

    FILE* fd;
    long length;
    char* plaintext;
    char* key;

    char* valid_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ \n";
    
    // plaintext
    fd = fopen(argv[1], "r");
    if (fd == NULL) {
        exit(1);
    } else {
        fseek(fd, 0, SEEK_END);
        length = ftell(fd);
        fseek(fd, 0, SEEK_SET);
        plaintext = malloc (length);
        if (plaintext) {
            fread (plaintext, 1, length, fd);
        }
        fclose (fd);
        
        if (plaintext[strspn(plaintext, valid_chars)] != '\0') {
            exit(1);
        }
    }

    // key
    fd = fopen(argv[2], "r");
    if (fd == NULL) {
        exit(1);
    } else {
        fseek(fd, 0, SEEK_END);
        length = ftell(fd);
        fseek(fd, 0, SEEK_SET);
        key = malloc (length);
        if (key) {
            fread (key, 1, length, fd);
        }
        fclose (fd);

        if (key[strspn(key, valid_chars)] != '\0') {
            exit(1);
        }
    }

    // check if plaintext and key are valid
    /*
    if (strlen(argv[1] > strlen(argv[2]))) {
        exit(1);
    }
    */

    // Create a socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0){
        close(socket_fd);
        exit(1);
    }

    // Set up the server address struct
    setupAddressStruct(&server_addr, atoi(argv[3]), "localhost");

    // Connect to server
    if (connect(socket_fd, (struct sockaddr*)& server_addr, sizeof(server_addr)) < 0){
        close(socket_fd);
        exit(1);
    }

    ////////////////////////////////////////////

    // clear out the buffer array
    buffer = malloc(19);
    memset(buffer, '\0', 19);

    sprintf(buffer, "enc_client:%.8X", (strlen(plaintext)+strlen(key)+1));

    // send identity to server
    ch_written = send(socket_fd, buffer, 19, 0);
    
    // characters written error
    if (ch_written < 19){
        printf("enc_client: not all enc_client written to socket\n");
    }

    // get return message from server
    free(buffer);
    buffer = malloc(10);
    memset(buffer, '\0', 10);
    ch_read = recv(socket_fd, buffer, 10, 0);

    // characters read error
    if (ch_read < 10){
        close(socket_fd);
        exit(1);
    }

    // not connected to proper server
    if (strncmp(buffer, "enc_server", 10) != 0) {
        close(socket_fd);
        exit(1);
    }

    ////////////////////////////////////////////

    // clear out the buffer array
    free(buffer);
    buffer = malloc(strlen(plaintext)+strlen(key)+2);
    memset(buffer, '\0', strlen(plaintext)+strlen(key)+1);

    // set buffer plaintext
    sprintf(buffer, "%s;%s", plaintext, key);

    // send plaintext to server
    ch_written = send(socket_fd, buffer, strlen(plaintext)+strlen(key)+1, 0); 
    if (ch_written < strlen(plaintext)+strlen(key)+1){
        close(socket_fd);
        exit(1);
    }

    // get return message from server
    free(buffer);
    buffer = malloc(strlen(plaintext));
    memset(buffer, '\0', strlen(plaintext));
    ch_read = recv(socket_fd, buffer, strlen(plaintext), 0);

    // characters read error
    if (ch_read < strlen(plaintext)){
        close(socket_fd);
        exit(1);
    }

    printf("%s", buffer);
    
    // close the socket
    close(socket_fd); 

    return 0;
}
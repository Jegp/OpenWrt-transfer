/*
    A C 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main(void) {
    int socketfd, status;
    struct addrinfo hints, *servinfo, *current;
    struct sockaddr_storage remote_addr;
    char ipstr[INET6_ADDRSTRLEN];
    socklen_t remote_addr_size;

    // Init address
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Check for correct initialization of address
    if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0) {
        fprintf(stderr, "Server error at getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    // Get address, create socket and bind port
    for (current = servinfo; current != NULL; current = current->ai_next) {
        void * address; 
        char * ip_version;
        
        // Get address
        if (current->ai_family == AF_INET) {
            address = &(((struct sockaddr_in *) current->ai_addr)->sin_addr);
        } else {
            address = &(((struct sockaddr_in6 *) current->ai_addr)->sin6_addr);
        }

        // Create socket
        if ((socketfd = socket(current->ai_family, 
                             current->ai_socktype, 
                             current->ai_protocol)) == -1) {
            perror("Client: Failed to create socket");
            continue;
        }
        
        if (connect(socketfd, current->ai_addr, current->ai_addrlen) == -1) {
            close(socketfd);
            perror("Client: Failed to connect to server");
            continue;
        }
    
        break;
    }

    // Free memory
    freeaddrinfo(servinfo);

    char *msg = "Beej was here!";
    int len, bytes_sent;
    len = strlen(msg);
    bytes_sent = send(socketfd, msg, len, 0);
    printf("Sent %d bytes out of len 14", bytes_sent, len);
    close(socketfd);
        

    // Success
    return 0;
}
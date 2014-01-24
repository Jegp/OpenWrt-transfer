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
    int socketfd, status, new_fd;
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
            perror("Server: Failed to create socket");
            continue;
        }

        // Bind port
        if (bind(socketfd, current->ai_addr, current->ai_addrlen) == -1) {
            close(socketfd);
            perror("Server: Failed to bind to socket");
            exit(1);
        }
        
        
        inet_ntop(current->ai_family, address, ipstr, sizeof ipstr);
        printf("Starting server at %s...\n", ipstr);

        break;
    }
    
    // Free memory
    freeaddrinfo(servinfo);
    
    if (listen(socketfd, 10) == -1) {
        perror("Server: Failed to listen for new connections");
        exit(1);
    }
    
    while (1) {
        remote_addr_size = sizeof remote_addr;
        if ((new_fd = accept(socketfd, (struct sockaddr *)&remote_addr, &remote_addr_size)) == -1) {
            perror("Server: Error when accepting connection");
            exit(1);
        }
        char s[INET6_ADDRSTRLEN];

        inet_ntop(remote_addr.ss_family,
            &((struct sockaddr_in *)&remote_addr)->sin_addr,
            s, sizeof s);
        printf("server: got connection from %s\n", s);
        char buf[100];
        int rec;
        if ((rec = recv(new_fd, buf, 99, 0)) == -1) {
            perror("Server: Failure when receiving message");
            continue;
        }
        buf[rec] = '\0';
        printf("Received: '%s' with length %d\n", buf, rec);
        close (new_fd);
    }
    close(socketfd);

    // Success
    return 0;
}
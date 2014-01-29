/*
    A simple client
    Thanks to http://beej.us/guide/bgnet/output/html/multipage/index.html
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

int write_to_socket(int socketfd, void * buffer, int bytes_read) {
    int bytes_written;
    while (bytes_read > 0) {
        bytes_written = write(socketfd, buffer, bytes_read);
        if (bytes_written < 0) {
            fprintf(stderr, "Failed to write to server\n");
            return -1;
        }
        bytes_read -= bytes_written;
        buffer += bytes_written;
    }

    return 0;
}

int sendFile(char * file, int socketfd) {
    int filefd, bytes_read;
    char buffer[1000];

    if ((filefd = open(file, O_RDONLY)) < 0) {
        fprintf(stderr, "Could not open file '%s'\n", file);
        return -1;
    }
    
    // Read the file into a buffer
    while (1) {
        bytes_read = read(filefd, buffer, sizeof(buffer));
        if (bytes_read == 0) break;
        if (bytes_read < 0) {
            fprintf(stderr, "Failed to read from file\n");
            return -1;
        }
        
        if (write_to_socket(socketfd, (void *) buffer, bytes_read) != 0) {
            return -1;
        }
    }

    // Close the open file
    close(filefd);
    
    return 0;
}

int send_std_in(int socketfd) {
    int bytes_read;
    char * line = NULL;
    size_t size;
    
    // Loop forever
    while (1) {
        if ((bytes_read = getline(&line, &size, stdin)) == -1) {
            printf("Input ended\n");
            return 0;
        }

        if (write_to_socket(socketfd, (void *) line, bytes_read) != 0) {
            return -1;
        }
    }

    return 0;
}


int main(int argc, char * args[]) {
    int socketfd, status;
    struct addrinfo hints, *servinfo, *current;
    struct sockaddr_storage remote_addr;
    char ipstr[INET6_ADDRSTRLEN];
    socklen_t remote_addr_size;

    // Init params
    if (argc != 2 && argc != 3) {
        printf("Usage: client server-address [filename]\n");
        exit(1);
    }
    char * server   = args[1];
    char * filename = args[2];
    printf("Sending %s to server at '%s'\n", 
        (filename == NULL) ? "" : filename, server);

    // Init address
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Check for correct initialization of address
    if ((status = getaddrinfo(server, "3490", &hints, &servinfo)) != 0) {
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
            continue;
        }
    
        break;
    }

    // Check for error when connecting
    if (current == NULL)  {
        fprintf(stderr, "Client: Failed to bind\n");
        return 2;
    }
    
    // Free memory
    freeaddrinfo(servinfo);
    
    // Send the input
    if (filename == NULL) {
      if (send_std_in(socketfd) != 0) {
        fprintf(stderr, "Client: Error while sending data\n");
      }
    } else if (sendFile(filename, socketfd) != 0) {
        fprintf(stderr, "Client: Error while sending file\n");
        return -1;
    }

    // Close the socket
    close(socketfd);

    // Success
    return 0;
}
                                 
/*
    A simple server
    Thanks to http://beej.us/guide/bgnet/output/html/multipage/index.html
*/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

int write_file(char * filename, int connection_fd) {
    char buffer[100];
    int filefd, bytes_read, bytes_written;

    // Open outputfile
    if ((filefd = open(filename, O_CREAT | O_WRONLY, 0644)) < 0) {
        fprintf(stderr, "Failed to create file '%s'\n", filename);
        return -1;
    }
        
    while (1) {
        bytes_read = recv(connection_fd, buffer, sizeof(buffer), 0);
        if (bytes_read == 0) break;
        if (bytes_read < 0) {
            perror("Failure when receiving data");
            return -1;
        }
        
        void * p = buffer;
        while (bytes_read > 0) {
            bytes_written = write(filefd, p, bytes_read);
            if (bytes_written < 0) {
                fprintf(stderr, "Failed to write to file '%s'", filename);
                return -1;
            }
            bytes_read -= bytes_written;
            p += bytes_written;
        }
    }

    // Success
    return 0;
}

int main(int argc, char * args[]) {
    int socketfd, status, new_fd;
    struct addrinfo hints, *servinfo, *current;
    struct sockaddr_storage remote_addr;
    struct timeval tv;
    char ipstr[INET6_ADDRSTRLEN], filename[100];
    socklen_t remote_addr_size;
    double timestamp;
    pid_t childId;
    char * address_prefix;

    if (argc == 2) {
        struct stat file_status;
        address_prefix = args[1];
        if (stat(address_prefix, &file_status) == 0) {
            if (!(file_status.st_mode & S_IFDIR)) {
                fprintf(stderr, "Failure: Prefix '%s' must be a directory\n", address_prefix);
                return -1;
            }
        } else {
            fprintf(stderr, "Failure: Prefix '%s' could not be found\n", address_prefix);
            return -1;
        }
    } else if (argc > 2) {
        printf("Usage: server [path-prefix]\n");
    } else {
        address_prefix = "";
    }

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
        
        // Print success
        inet_ntop(current->ai_family, address, ipstr, sizeof ipstr);
        printf("Starting server at %s...\n", ipstr);

        break;
    }

    // Check to see if the connection could be made
    if (current == NULL) {
        fprintf(stderr, "Server: Failed to bind\n");
        return -1;
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
        
        gettimeofday(&tv, NULL);
        timestamp = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
    
        printf("Got connection from %s at %.0f\n", s, timestamp);
        
        snprintf(filename, sizeof(filename), "%s/%s_%.0f", address_prefix, s, timestamp);        
        
        if (write_file(filename, new_fd) != 0) {
            fprintf(stderr, "Error, shutting down\n");
            break;
        }
    
        printf("Successfully wrote file '%s'\n", filename);

        close (new_fd);
    }
    close(socketfd);

    // Success
    return 0;
}
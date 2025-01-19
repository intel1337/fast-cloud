#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <dirent.h>

#define IMGMAX 100000
#define STORAGE_DIR "./storage"
#define SERVER_PORT 8080

void handle_client(int client_fd) {
    char command[BUFSIZ];
    int command_length = recv(client_fd, command, BUFSIZ, 0);
    if (command_length == -1) {
        perror("Receive command :");
        return;
    }
    command[command_length] = '\0';

    if (strcmp(command, "stlist") == 0) {
        DIR *dir;
        struct dirent *entry;
        char *file_list = malloc(IMGMAX);
        memset(file_list, 0, IMGMAX);

        dir = opendir(STORAGE_DIR);
        if (dir == NULL) {
            free(file_list);
            return;
        }

        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_REG) {
                strcat(file_list, entry->d_name);
                strcat(file_list, "\n");
            }
        }
        closedir(dir);

        if (strlen(file_list) == 0) {
            free(file_list);
            return;
        }

        send(client_fd, file_list, strlen(file_list), 0);
        free(file_list);

    } else if (strncmp(command, "stupload ", 9) == 0) {
        char *file_name = command + 9;
        char file_path[BUFSIZ];
        snprintf(file_path, BUFSIZ, "%s/%s", STORAGE_DIR, file_name);

        FILE *file = fopen(file_path, "wb");
        if (file == NULL) {
            const char *error_msg = "error : Could not create file";
            send(client_fd, error_msg, strlen(error_msg), 0);
            return;
        }

        char file_data[IMGMAX];
        int file_size = recv(client_fd, file_data, IMGMAX, 0);
        if (file_size == -1) {
            perror("Receive file data :");
            fclose(file);
            return;
        }
        fwrite(file_data, 1, file_size, file);
        fclose(file);
        const char *success_msg = "File uploaded successfully.";
        send(client_fd, success_msg, strlen(success_msg), 0);
   
    } else if (strncmp(command, "stdl ", 5) == 0) {
        char *file_name = command + 5;
        char file_path[BUFSIZ];
        snprintf(file_path, BUFSIZ, "%s/%s", STORAGE_DIR, file_name);

        FILE *file = fopen(file_path, "rb");
        if (file == NULL) {
            const char *error_msg = "error : File not found";
            send(client_fd, error_msg, strlen(error_msg), 0);
            return;
        }

        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        rewind(file);

        char file_data[IMGMAX];
        fread(file_data, 1, file_size, file);
        fclose(file);

        send(client_fd, file_data, file_size, 0);
    } else {
        const char *error_msg = "error : bad command";
        send(client_fd, error_msg, strlen(error_msg), 0);
    }
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket :");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr = {
        .sin_addr.s_addr = INADDR_ANY,
        .sin_family = AF_INET,
        .sin_port = htons(SERVER_PORT)
    };

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind :");
        close(server_fd);
        return EXIT_FAILURE;
    }

    if (listen(server_fd, 3) == -1) {
        perror("Listen :");
        close(server_fd);
        return EXIT_FAILURE;
    }

    printf("Server listening on port: %d\n", SERVER_PORT);
    while(1){
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_fd == -1) {
            perror("Accept :");
            close(server_fd);
            return EXIT_FAILURE;
        }

        handle_client(client_fd);

        close(client_fd);
    }
    close(server_fd);
    return EXIT_SUCCESS;
}
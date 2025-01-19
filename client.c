#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define IMGMAX 100000
#define SERVER_PORT 8080

void upload_file(int server_fd, char *file_name) {
    FILE *file = fopen(file_name, "rb");
    if (file == NULL) {
        perror("File Open :");
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char file_data[IMGMAX];
    fread(file_data, 1, file_size, file);
    fclose(file);

    char command[BUFSIZ];
    snprintf(command, BUFSIZ, "stupload %s", file_name);
    send(server_fd, command, strlen(command), 0);
    send(server_fd, file_data, file_size, 0);

    char response[BUFSIZ];
    int response_length = recv(server_fd, response, BUFSIZ, 0);
    if (response_length == -1) {
        perror("Response :");
        return;
    }
    response[response_length] = '\0';
    printf("%s\n", response);
}

void download_file(int server_fd, char *file_name) {
    char command[BUFSIZ];
    snprintf(command, BUFSIZ, "stdl %s", file_name);
    send(server_fd, command, strlen(command), 0);

    char file_data[IMGMAX];
    int file_size = recv(server_fd, file_data, IMGMAX, 0);
    if (file_size == -1) {
        perror("File data :");
        return;
    }

    FILE *file = fopen(file_name, "wb");
    if (file == NULL) {
        perror("File Open :");
        return;
    }
    fwrite(file_data, 1, file_size, file);
    fclose(file);

    printf("File downloaded successfully.\n");
}

void list_files(int server_fd) {
    char command[BUFSIZ];
    snprintf(command, BUFSIZ, "stlist");
    send(server_fd, command, strlen(command), 0);

    char file_list[IMGMAX];
    int file_list_length = recv(server_fd, file_list, IMGMAX, 0);
    if (file_list_length == -1) {
        perror("File list :");
        return;
    }
    file_list[file_list_length] = '\0';
    printf("%s\n", file_list);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <command> [file_name]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket :");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr = {
        .sin_addr.s_addr = inet_addr("127.0.0.1"),
        .sin_family = AF_INET,
        .sin_port = htons(SERVER_PORT)
    };

    if (connect(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connect :");
        close(server_fd);
        return EXIT_FAILURE;
    }

    char *command = argv[1];
    if (strcmp(command, "stlist") == 0) {
        list_files(server_fd);
    } else if (strcmp(command, "stupload") == 0) {
        if (argc < 3) {
            printf("Error: File name is required for upload command\n");
            close(server_fd);
            return EXIT_FAILURE;
        }
        upload_file(server_fd, argv[2]);
    } else if (strcmp(command, "stdl") == 0) {
        if (argc < 3) {
            printf("Error: File name is required for download command\n");
            close(server_fd);
            return EXIT_FAILURE;
        }
        download_file(server_fd, argv[2]);
    } else {
        printf("Error: Unknown command\n");
        close(server_fd);
        return EXIT_FAILURE;
    }

    close(server_fd);
    return EXIT_SUCCESS;
}
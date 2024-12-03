#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "config.h"  // Contient le port du serveur
#define IMGMAX 100000

char buf[BUFSIZ];
char fname[BUFSIZ];
char input[BUFSIZ];
 char finalfile[IMGMAX];


int main(){
    system("clear");
    printf("Welcome to [🌨️ ] - FastCloud\n");
    printf("Version : 1.0\n");
    printf("Type connect for a quick start.. \n\n");
    printf("Fastcloud@🌨️ $");
    fgets(input,BUFSIZ,stdin);
    if (strcmp(input,"connect\n") == 0){
        system("clear");
        printf("[🌨️ ] - FastCloud\n");
        printf("Version : 1.0\n");
        printf("Fastcloud@🌨️ - File name :");
        fgets(fname,BUFSIZ,stdin);
        fname[strcspn(fname, "\n")] = 0;
        FILE* filename = fopen(fname,"r+");
        fseek(filename,0,SEEK_END);
        const long tmpsize = ftell(filename);
        rewind(filename);
        fread(finalfile,1,tmpsize,filename);

        printf("[🌨️ ] - FastCloud\n");
        printf("Press Enter to continue..");
        getchar();
        system("clear");
        fflush(stdout);
        printf("[🌨️ ] - FastCloud\n");
        printf("Version : 1.0\n");
        

        int server_fd = socket(AF_INET,SOCK_STREAM,0);
        if(server_fd == -1) return EXIT_FAILURE;
        struct sockaddr_in server_addr = {
            .sin_addr.s_addr = INADDR_ANY,
            .sin_family = AF_INET,
            .sin_port = htons(SERVER_PORT)
        };
        int error = bind(server_fd,(struct sockaddr*)&server_addr,sizeof server_addr);
        if(error == -1) { close(server_fd); return EXIT_FAILURE; }

        error = listen(server_fd,BUFSIZ);
        if(error == -1) { close(server_fd); return EXIT_FAILURE; }
        printf("Server listen on port : %d\n",SERVER_PORT);



        struct sockaddr_in client_addr;
        socklen_t len;
        int client_fd = accept(server_fd,(struct sockaddr*)&client_addr,&len);
        if(client_fd == -1){ close(server_fd); return EXIT_FAILURE; }


        fseek(filename,0,SEEK_SET);
        fseek(filename,0,SEEK_END);

        error = send(client_fd,finalfile,tmpsize,0);
        if(error == -1){ close(client_fd); close(server_fd); return EXIT_FAILURE; }



        int recv_length = recv(client_fd,buf,BUFSIZ,0); 
        if(recv_length == -1){
            close(client_fd);
            close(server_fd);
            return EXIT_FAILURE;
        }
        printf("Fastcloud@🌨️ > Logs : ");
        printf("%s\n",buf);
        close(client_fd);
        close(server_fd);
        return EXIT_SUCCESS;

    // printf("%d\n",time(NULL));

    }
    
    
}
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

int main() {
    int ctrl_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in ctrl_addr;
    ctrl_addr.sin_family = AF_INET;
    ctrl_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    ctrl_addr.sin_port = htons(21); 

    if (connect(ctrl_socket, (struct sockaddr *)&ctrl_addr, sizeof(ctrl_addr))) {
        perror("connect() failed");
        return 1;
    }

    char buf[2048];
    
    // Nhan xau chao tu server
    int len = recv(ctrl_socket, buf, sizeof(buf), 0);
    buf[len] = 0;
    // puts(buf);

    char username[64], password[64];

    while (1)
    {
        printf("Nhap username: ");
        scanf("%s", username);
        printf("Nhap password: ");
        scanf("%s", password);

        // Gui lenh USER
        sprintf(buf, "USER %s\r\n", username);
        send(ctrl_socket, buf, strlen(buf), 0);

        len = recv(ctrl_socket, buf, sizeof(buf), 0);
        buf[len] = 0;
        // puts(buf);

        // Gui lenh PASS
        sprintf(buf, "PASS %s\r\n", password);
        send(ctrl_socket, buf, strlen(buf), 0);

        len = recv(ctrl_socket, buf, sizeof(buf), 0);
        buf[len] = 0;
        // puts(buf);

        if (strncmp(buf, "230", 3) == 0)
        {
            printf("Dang nhap thanh cong.\n");
            break;
        }
        else
        {
            printf("Dang nhap that bai.\n");
        }
    }
    char cmd[32];

    while (1){

    scanf("%s", cmd);

    // Xu li PASV
    if (strncmp(cmd,"PASV\r\n",strlen(cmd)) == 0)
    {
        // Mo ket noi du lieu
        // Gui lenh PASV
        send(ctrl_socket, "PASV\r\n", 6, 0);
        len = recv(ctrl_socket, buf, sizeof(buf), 0);
        buf[len] = 0;
        puts(buf);

        // Xu ly ket qua lenh PASV
        char *pos1 = strchr(buf, '(') + 1;
        char *pos2 = strchr(pos1, ')');
        char temp[256];
        int n = pos2 - pos1;
        memcpy(temp, pos1, n);
        temp[n] = 0;

        char *p = strtok(temp, ",");
        int i1 = atoi(p);
        p = strtok(NULL, ",");
        int i2 = atoi(p);
        p = strtok(NULL, ",");
        int i3 = atoi(p);
        p = strtok(NULL, ",");
        int i4 = atoi(p);
        p = strtok(NULL, ",");
        int p1 = atoi(p);
        p = strtok(NULL, ",");
        int p2 = atoi(p);

        struct sockaddr_in data_addr;
        data_addr.sin_family = AF_INET;
        sprintf(temp, "%d.%d.%d.%d", i1, i2, i3, i4);
        data_addr.sin_addr.s_addr = inet_addr(temp);
        data_addr.sin_port = htons(p1 * 256 + p2);

        int data_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (connect(data_socket, (struct sockaddr *)&data_addr, sizeof(data_addr))) {
            perror("connect() failed");
            return 1;
        }

        getchar();
        fgets(cmd, sizeof(cmd),stdin);
        cmd[strlen(cmd)] = 0;
        puts(cmd);

        if (strncmp(cmd, "LIST",4)==0){

        // Gui lenh LIST
        send(ctrl_socket, "LIST\r\n", 6, 0);
        len = recv(ctrl_socket, buf, sizeof(buf), 0);
        buf[len] = 0;
        puts(buf);

        // Nhan du lieu
        while (1)
        {
            len = recv(data_socket, buf, sizeof(buf), 0);
            if (len <= 0)
                break;
            buf[len] = 0;
            puts(buf);
        }

        // Nhan phan hoi cua lenh LIST
        len = recv(ctrl_socket, buf, sizeof(buf), 0);
        buf[len] = 0;
        puts(buf);
        close(data_socket);
        }

        
        if (strncmp(cmd,"STOR",4)==0){

            char fileName[256];
            memcpy(fileName,cmd+5,strlen(cmd)-3);
            fileName[strlen(fileName)-1] = 0;

            FILE *fp = fopen(fileName,"rb");
            if (fp == NULL){
                printf("File not found\n");
            }

            char msg[512];
            sprintf(msg, "STOR %s\r\n", fileName);
            send(ctrl_socket,msg,strlen(msg),0);

            len = recv(ctrl_socket, buf, sizeof(buf), 0);
            buf[len] = 0;
            puts(buf);

            char data[512];

            while (!feof(fp))
            {
                /* code */ 
                int ret = fread(data,1,sizeof(data),fp);
                if (ret <= 0)
                    break;
                send(data_socket, data, ret, 0);
            }
            
        printf("Updated\n");
        fclose(fp);
        close(data_socket);

        // Nhan phan hoi lenh STOR
        len = recv(ctrl_socket, buf, sizeof(buf), 0);
        buf[len] = 0;
        puts(buf);

        }
    
    }

    //Xu li EPSV
    if (strncmp(cmd,"EPSV",4) == 0){
        
         // Gui lenh PASV
        send(ctrl_socket, "EPSV\r\n", 6, 0);

        //Nhan phan hoi lenh EPSV
        len = recv(ctrl_socket, buf, sizeof(buf), 0);
        buf[len] = 0;
        puts(buf);
        char *start = strstr(buf,"(|||");
        char *end = strstr(buf,"|)");
        char temp[256];
        memcpy(temp,start+4,end-start-4);
        int port = atoi(temp);

        struct sockaddr_in data_addr;
        data_addr.sin_family = AF_INET;
        data_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        data_addr.sin_port = htons(port);

        int data_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (connect(data_socket, (struct sockaddr *)&data_addr, sizeof(data_addr))) {
            perror("connect() failed");
            return 1;
        }

        getchar();
        fgets(cmd, sizeof(cmd),stdin);
        cmd[strlen(cmd)] = 0;
        puts(cmd);

        if (strncmp(cmd, "LIST",4)==0){

        // Gui lenh LIST
        send(ctrl_socket, "LIST\r\n", 6, 0);
        len = recv(ctrl_socket, buf, sizeof(buf), 0);
        buf[len] = 0;
        puts(buf);

        // Nhan du lieu
        while (1)
        {
            len = recv(data_socket, buf, sizeof(buf), 0);
            if (len <= 0)
                break;
            buf[len] = 0;
            puts(buf);
        }

        // Nhan phan hoi con lai cua lenh LIST
        len = recv(ctrl_socket, buf, sizeof(buf), 0);
        buf[len] = 0;
        puts(buf);
        close(data_socket);
        }

        
        if (strncmp(cmd,"STOR",4)==0){

            char fileName[256];
            memcpy(fileName,cmd+5,strlen(cmd)-3);
            fileName[strlen(fileName)-1] = 0;

            FILE *fp = fopen(fileName,"rb");
            if (fp == NULL){
                printf("File not found\n");
            }

            char msg[512];
            sprintf(msg, "STOR %s\r\n", fileName);
            send(ctrl_socket,msg,strlen(msg),0);

            len = recv(ctrl_socket, buf, sizeof(buf), 0);
            buf[len] = 0;
            puts(buf);

            char data[512];

            while (!feof(fp))
            {
                /* code */ 
                int ret = fread(data,1,sizeof(data),fp);
                if (ret <= 0)
                    break;
                send(data_socket, data, ret, 0);
            }
            
        printf("Updated\n");
        fclose(fp);
        close(data_socket);

        len = recv(ctrl_socket, buf, sizeof(buf), 0);
        buf[len] = 0;
        puts(buf);

        }
            
    }
    
    if (strncmp(cmd, "QUIT",4) == 0){

        //Gui lenh QUIT
        send(ctrl_socket, "QUIT\r\n", 6, 0);
        len = recv(ctrl_socket, buf, sizeof(buf), 0);
        buf[len] = 0;
        puts(buf);
        // Ket thuc, dong socket
        close(ctrl_socket);

        return 0;
    }
   
    
    }
   
     
}
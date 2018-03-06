#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "mesg.h"

#define MAX_MSGSIZE           256    // max message size
#define SERVER_MSG_TYPE (long)1     // server's message type
#define CLIENT_MSG_TYPE (long)2     // client's message type
#define MSG_PERM (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH) // msg flags

typedef struct Message              
{
    // int mesg_len; /* #bytes in mesg_data */
    long mesg_type; /* message type */
    char mesg_data[MAX_MSGSIZE];
} Message;

int main(int argc, char** argv) {

    FILE *fp;
    pid_t childpid = 0;
    int msgQID;           // msg queue id
    int msgFlags;         // msg flags
    int temp;
    int count = 0;
    long msgType;         // msg type
    key_t msgKey;         // msg key
    char filename[MAX_MSGSIZE];
    Message sendMsg;         // msg to send
    Message recvMsg;         // msg to receive


    if (argc < 2) {
        printf("usage: %s MSGKEY\n", argv[0]);
        exit(0);
    }

    if (sscanf(argv[1], "%d", &temp) < 1) {
        perror("Failed to read key input");
        exit(0);
    }

    msgKey = (long) temp;

    // obtain the corresponding message queue
    if ((msgQID = msgget(msgKey, 0666)) < 0 ) {
       perror("CLIENT: msgget");
       exit(EXIT_FAILURE);
    }

    sendMsg.mesg_type = SERVER_MSG_TYPE;
    msgFlags = 0;
    sprintf(sendMsg.mesg_data, "SERVER: Nothing to send\n %c", EOF);

    msgType = CLIENT_MSG_TYPE;
    msgFlags = 0;

    while(1) {
        printf("Waiting for client...\n Client count: %d\n", count);

        if (msgrcv(msgQID, &recvMsg, MAXMESSAGEDATA, msgType, 0) < 0)
        {
            perror("SERVER: msgrcv");
            exit(EXIT_FAILURE);
        }

        childpid = fork();
        if (childpid == -1) {
            perror("Fork failed!");
            exit(2);
        }

        if (childpid == 0) {
            long client_pid;
            sscanf(recvMsg.mesg_data, "%s %ld", filename, &client_pid);
            printf("Requested file: %s\n", filename);
            sendMsg.mesg_type = client_pid;
            // printf("Requested file: %s\n", recvMsg.mesg_data);
            // sprintf(filename, "%s", recvMsg.mesg_data);
            
            // open file for sending
            fp = fopen(filename, "r");
            if (fp == NULL) {

                if (msgsnd(msgQID, &sendMsg, strlen(sendMsg.mesg_data) + 1, msgFlags) < 0) {
                    perror("SERVER: msgsnd");
                    exit(EXIT_FAILURE);
                }
                perror("Could not open file");
                exit(1);

            } else {
                // send contents
                while (fgets(sendMsg.mesg_data, MAX_MSGSIZE, fp)) {

                    if (msgsnd(msgQID, &sendMsg, strlen(sendMsg.mesg_data) + 1, msgFlags) < 0) {
                        perror("SERVER: msgsnd");
                        exit(EXIT_FAILURE);
                    }

                }
            }

            // send stop receiving signal
            sprintf(sendMsg.mesg_data, "%c", EOF);
            if (msgsnd(msgQID, &sendMsg, strlen(sendMsg.mesg_data) + 1, msgFlags) < 0) {
                perror("SERVER: msgsnd");
                exit(EXIT_FAILURE);
            }

            fclose(fp);
            printf("File successully sent.\n");
            exit(EXIT_SUCCESS);

        } else {
            count++;
            continue;
        }
    }
}  

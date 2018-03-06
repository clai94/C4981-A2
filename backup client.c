#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "mesg.h"

/*------------------------------------------+
| Constants                                 |
+------------------------------------------*/
/* maximum message size           */
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

    int msgQID;           // msg queue id
    int msgFlags;         // msg flags
    int temp;
    int x = 0;
    long msgType;         // msg type
    key_t msgKey;         // msg key
    Message sendMsg;         // msg to send
    Message recvMsg;         // msg to receive
    

/*------------------------------------------*/
/* Get the message queue id for MSG_KEY,    */
/* which was set by the                     */
/* message server.                          */
/*------------------------------------------*/
//    msgKey = (long) 101;
    if (argc < 3) {
        printf("usage: %s MSGKEY FILENAME\n", argv[0]);
        exit(0);
    }

    if (sscanf(argv[1], "%d", &temp) < 1) {
        perror("Failed to read key input");
        exit(0);
    }

    msgKey = (long) temp;
    // assign mKey from user input here
    // obtain the corresponding message queue
    if ((msgQID = msgget(msgKey, 0666)) < 0 ) {
       perror("CLIENT: msgget");
       exit(EXIT_FAILURE);
    }

    // sendMsg.mesg_len = sizeof(struct Message);
    sendMsg.mesg_type = CLIENT_MSG_TYPE;
    msgFlags = 0;

    // send the requested filename to server
    sprintf(sendMsg.mesg_data, "%s", argv[2]);
    if (msgsnd(msgQID, &sendMsg, strlen(sendMsg.mesg_data) + 1, msgFlags) < 0)
    {
        perror("CLIENT: msgsnd");
        exit(EXIT_FAILURE);
    }
    printf("Requested filename: %s\n", sendMsg.mesg_data);
    printf("Waiting for file contents...\n");
    /*-----------------------------------------*/
    /* Receive filename data from server.          */
    /*-----------------------------------------*/
    msgType = SERVER_MSG_TYPE;
    msgFlags = 0;

    
    while (x != 1) {
        if (msgrcv(msgQID, &recvMsg, MAXMESSAGEDATA, msgType, 0) < 0)
        {
            perror("CLIENT: msgrcv");
            exit(EXIT_FAILURE);
        }
        printf("%s\n", recvMsg.mesg_data);
        for (int i = 0; recvMsg.mesg_data[i] != '\0'; i++) {
            if (recvMsg.mesg_data[i] == EOF) {
                x = 1;
                break;
            }
        }
    }
    printf("Message successfully received.\n");
    exit(EXIT_SUCCESS);
}  /* end of main() */

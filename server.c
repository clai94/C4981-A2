#include "mesg.h"
#include "messages.c"

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
    Mesg sendMsg;         // msg to send
    Mesg recvMsg;         // msg to receive


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
    if ((msgQID = msgget(msgKey, 0666 | IPC_CREAT)) < 0 ) {
       perror("SERVER: msgget");
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
            long priority;
            sscanf(recvMsg.mesg_data, "%s %ld %ld", filename, &client_pid, &priority);
            printf("%s\n", recvMsg.mesg_data);
            printf("Requested file: %s\n", filename);
            printf("Client ID: %ld\n", client_pid);
            printf("Client Priority: %ld\n", priority);

            sendMsg.mesg_type = client_pid;
            
            // open file for sending
            fp = fopen(filename, "r");
            if (fp == NULL) {

                if (msgsnd(msgQID, &sendMsg, strlen(sendMsg.mesg_data) + 1, msgFlags) < 0) {
                    perror("SERVER: msgsnd");
                    exit(EXIT_FAILURE);
                }
                // mesg_send(msgQID, sendMsg);
                perror("Could not open file");
                exit(1);

            } else {
                // send contents
                while (fgets(sendMsg.mesg_data, (MAX_MSGSIZE) / priority, fp)) {

                    if (msgsnd(msgQID, &sendMsg, (strlen(sendMsg.mesg_data) + 1), msgFlags) < 0) {
                        perror("SERVER: msgsnd");
                        exit(EXIT_FAILURE);
                    }
                    // mesg_send(msgQID, sendMsg);
                }
            }

            // send stop receiving signal
            sprintf(sendMsg.mesg_data, "%c", EOF);
            if (msgsnd(msgQID, &sendMsg, strlen(sendMsg.mesg_data) + 1, msgFlags) < 0) {
                perror("SERVER: msgsnd");
                exit(EXIT_FAILURE);
            }
            // mesg_send(msgQID, &sendMsg);


            fclose(fp);
            printf("File successully sent.\n");
            exit(EXIT_SUCCESS);

        } else {
            count++;
            continue;
        }
    }
}  

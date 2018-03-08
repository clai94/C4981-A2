/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: client.c
--
-- PROGRAM: 
--
-- FUNCTIONS:
-- 
-- int main(int, char**)
-- void *competion_check(void *x_void_ptr)
-- DATE: March 1, 2018
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Calvin Lai
--
-- PROGRAMMER: Calvin Lai
--
-- NOTES:
-- A client program that requests and receives a file from the server using a message queue.
----------------------------------------------------------------------------------------------------------------------*/

#include "mesg.h"
#include "messages.c"

// Function Prototypes
void *completion_check(void *x_void_ptr);

int main(int argc, char** argv) {

    int msgQID;           // msg queue id
    int msgFlags;         // msg flags
    int temp;
    int temp2;
    int x = 0;
    int start = 0;
    int init = 0;
    int final = 0;
    long priority;
    long msgType;         // msg type
    key_t msgKey;         // msg key
    time_t t = time(NULL);  
    struct tm tm = *localtime(&t);
    Mesg sendMsg;         // msg to send
    Mesg recvMsg;         // msg to receive
    pthread_t completion_check_thread;
    
    if (argc < 4) {
        printf("usage: %s [MSGKEY] [FILENAME] [PRIORITY]\n", argv[0]);
        exit(0);
    }

    if (sscanf(argv[1], "%d", &temp) < 1) {
        perror("Failed to read key input");
        exit(0);
    }

    if (sscanf(argv[3], "%d", &temp2) < 1) {
        perror("Failed to read priority");
    }

    msgKey = (long) temp;
    priority = (long) temp2;
    // assign mKey from user input here
    // obtain the corresponding message queue
    if ((msgQID = msgget(msgKey, 0666)) < 0 ) {
       perror("CLIENT: msgget");
       exit(EXIT_FAILURE);
    }

    sendMsg.mesg_type = priority;
    msgFlags = 0;

    // send the requested filename to server
    sprintf(sendMsg.mesg_data, "%s %d %ld", argv[2], getpid(), priority);

    if (msgsnd(msgQID, &sendMsg, strlen(sendMsg.mesg_data) + 1, msgFlags) < 0)
    {
        perror("CLIENT: msgsnd");
        exit(EXIT_FAILURE);
    }

    printf("Requested filename and PID: %s\n", sendMsg.mesg_data);
    printf("Waiting for file contents...\n");

    msgType = getpid();
    
    while (x != 1) {
        if (msgrcv(msgQID, &recvMsg, MAXMESSAGEDATA / priority, msgType, 0) < 0)
        {
            perror("CLIENT: msgrcv");
            exit(EXIT_FAILURE);
        }

        if (start == 0) {
            init = (tm.tm_min * 60) + tm.tm_sec;
            start++;
            if (pthread_create(&completion_check_thread, NULL, completion_check, &x)) {
                fprintf(stderr, "Error creating thread\n");
            }
        }
        printf("%s", recvMsg.mesg_data);
        for (int i = 0; recvMsg.mesg_data[i] != '\0'; i++) {
            if (recvMsg.mesg_data[i] == EOF) {
                x = 1;
                break;
            }
        }
    }
    t = time(NULL);
    tm = *localtime(&t);
    final = (tm.tm_min * 60) + tm.tm_sec;
    if (pthread_join(completion_check_thread, NULL)) {

        fprintf(stderr, "\nError joining thread");

    }
    printf("\nFile successfully received.\nTotal transfer time: %d seconds.\nPriority: %ld\n", (final - init), priority);
    exit(EXIT_SUCCESS);
} 

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: completion_check
--
-- DATE: March 1, 2018
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Calvin Lai
--
-- PROGRAMMER: Calvin Lai
--
-- INTERFACE: void *completion_check(void *x_void_ptr)
--
-- RETURNS:  void.
--
-- NOTES:
-- Prints a message once the file transfer is complete.
----------------------------------------------------------------------------------------------------------------------*/
void *completion_check(void *x_void_ptr) {

    int *x_ptr = (int *) x_void_ptr;
    while (*x_ptr != 1) {
        ;
    }
    printf("File transfer complete.\n");
    /* the function must return something - NULL will do */
    return NULL;

}
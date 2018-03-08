/*------------------------------------------------------------------------------------------------------------------
-- HEADER FILE: mesg.h
--
-- PROGRAM: 
--
-- FUNCTIONS:
--
--
-- DATE: March 1, 2018
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Calvin Lai
--
-- PROGRAMMER: Calvin Lai
--
-- NOTES:
--
----------------------------------------------------------------------------------------------------------------------*/

#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define MAX_MSGSIZE           256    // max message size
#define SERVER_MSG_TYPE (long) 1     // server's message type
#define CLIENT_MSG_TYPE (long) -10     // client's message type
#define MSG_PERM (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH) // msg flags
#define MAXMESSAGEDATA (4096-16) /* don't want sizeof(Mesg) > 4096 */
#define MESGHDRSIZE (sizeof(Mesg) - MAXMESGDATA)

typedef struct {
    long mesg_type; /* message type */
    char mesg_data[MAXMESSAGEDATA];
    int mesg_len; /* #bytes in mesg_data */
} Mesg;

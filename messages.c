/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: messages.c
--
-- PROGRAM: 
--
-- FUNCTIONS:
-- int mesg_recv(int msgQID, Mesg *msg, long type)
-- int mesg_send(int msgQID, Mesg *msg)
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

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: mesg_recv
--
-- DATE: March 1, 2018
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Calvin Lai
--
-- PROGRAMMER: Calvin Lai
--
-- INTERFACE: int mesg_recv(int msgQID, Mesg *msg, long type)
--
-- RETURNS: int.
--
-- NOTES:
-- This function acts as a wrapper function for receiving messages.
----------------------------------------------------------------------------------------------------------------------*/
int mesg_recv(int msgQID, Mesg *msg, long type) {
    if (msgrcv(msgQID, &msg, MAXMESSAGEDATA, type, IPC_NOWAIT) < 0) {
        perror("SERVER: msgrcv");
        exit(EXIT_FAILURE);
    }
    return 1;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: mesg_send
--
-- DATE: March 1, 2018
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Calvin Lai
--
-- PROGRAMMER: Calvin Lai
--
-- INTERFACE: int mesg_send(int msgQID, Mesg *msg)
--
-- RETURNS: int.
--
-- NOTES:
-- This function is a wrapper function for sending messages.
----------------------------------------------------------------------------------------------------------------------*/
int mesg_send(int msgQID, Mesg* msg) {
    if (msgsnd(msgQID, &msg, strlen(msg->mesg_data) + 1, IPC_NOWAIT) < 0) {
        perror("SERVER: msgsnd");
        exit(EXIT_FAILURE);
    }
    return 1;
}
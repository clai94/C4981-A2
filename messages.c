int mesg_recv(int msgQID, Mesg *msg, long type) {
    if (msgrcv(msgQID, &msg, MAXMESSAGEDATA, type, IPC_NOWAIT) < 0) {
        perror("SERVER: msgrcv");
        exit(EXIT_FAILURE);
    }
    return 1;
}

int mesg_send(int msgQID, Mesg msg) {
    if (msgsnd(msgQID, &msg, strlen(msg.mesg_data) + 1, IPC_NOWAIT) < 0) {
        perror("SERVER: msgsnd");
        exit(EXIT_FAILURE);
    }
    return 1;
}
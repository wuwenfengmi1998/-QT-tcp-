#include "tcpusers.h"

tcpusers::tcpusers(QObject *parent) : QObject(parent)
{

}

void tcpusers::send_to_user(tcpusers *user,const char *data)
{
    user->usersocket->write(data);

}


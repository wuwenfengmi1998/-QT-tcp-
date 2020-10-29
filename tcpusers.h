#ifndef TCPUSERS_H
#define TCPUSERS_H

#include <QObject>
#include <QTcpSocket>  //通信套接字
class tcpusers : public QObject
{
    Q_OBJECT
public:
    explicit tcpusers(QObject *parent = nullptr);
    void send_to_user(tcpusers *user,const char *data);
    void send_to_all(const char *data);
    tcpusers *upone;
    unsigned int *userid;
    QTcpSocket *usersocket;
    QDateTime *timeout;
    QString *connect_type;
    tcpusers *nextone;
signals:

};

#endif // TCPUSERS_H

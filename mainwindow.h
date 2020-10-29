#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>  //监听套接字
#include <QTcpSocket>  //通信套接字
#include <QTimer>
#include <QDateTime>
#include <QMessageBox>
#include "tcpusers.h"
#include "conm.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void mainrun_timeout();
    QTcpServer *tcpserver;// 监听套接字
    QTimer *mainrun;
    tcpusers *users;
private slots:

    void on_comm_button_clicked();
protected:
    //这是一个虚函数，继承自QEvent.只要重写了这个虚函数，当你按下窗口右上角的"×"时，就会调用你所重写的此函数.
    void closeEvent(QCloseEvent*event);
private:
    Ui::MainWindow *ui;
    void procomm(tcpusers *user,QString *comm);
    void sendto(tcpusers *user,QString *comm);
    void sendall(tcpusers *user,QString *comm);
    void distcp(tcpusers *user,QString *comm);
    void tick(tcpusers *user,QString *comm);

};
#endif // MAINWINDOW_H

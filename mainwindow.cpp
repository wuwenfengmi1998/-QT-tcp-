#include "mainwindow.h"
#include "ui_mainwindow.h"

unsigned int connect_num=0;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tcpserver = new QTcpServer(this);//创建监听套接字//指定父对象可以自动回收空间
    users=NULL;//指针入口
    tcpserver->listen(QHostAddress::Any,58888);//绑定ip和端口  //ip是任意ip
    connect(tcpserver,&QTcpServer::newConnection,
            [=]()
    {
        tcpusers *lastone;
        lastone=users;//获取指针入口
        if(lastone==NULL)
        {
            lastone=new tcpusers(this);       //如果入口一个对象地址都没有，就new一个
            lastone->upone=NULL;       //初始化内部
            users=lastone;             //给入口地址
        }else                          //入口有地址
        {
            while(lastone->nextone!=NULL)  //将指针移动到最后一个对象
            {
                lastone=lastone->nextone;
            }
            tcpusers *upone=lastone;//备份当前对象指针
            //创建下一个对象
            lastone->nextone=new tcpusers(this);
            lastone=lastone->nextone;     //将指针移动到最后创建的对象
            lastone->upone=upone;
        }
        //初始化对象的值
        //取出建立好连接的套接字//这个不需要new 但是要防止野指针
        lastone->usersocket = tcpserver->nextPendingConnection();//在监听套接字中直接取，取到的就是一个指针 所以不需要再分配空间
        lastone->nextone=NULL;
        lastone->userid=new unsigned int;
        *(lastone->userid)=(rand()%100)+1;//登录器还没做  随机分配一个id
        lastone->timeout=new QDateTime;
        *(lastone->timeout)=QDateTime::currentDateTime();



        //获取对方的ip和端口
        QString ip = lastone->usersocket->peerAddress().toString();
        qint16 port = lastone->usersocket->peerPort();
        QString temp = QString("[%1:%2]:(%3)成功链接 %4").arg(ip).arg(65536+port).arg(*(lastone->userid)).arg(lastone->timeout->toString("yyyy.MM.dd hh:mm:ss"));//合成字符串  类似c  sprintf
        ui->textEdit_read->append(temp);//将它显示出来
        //将收到的数据显示出来
        connect(lastone->usersocket, &QTcpSocket::readyRead,
                [=]()
        {
             //获取对方的ip和端口

            //QString ip = lastone->usersocket->peerAddress().toString();
            //qint16 port = lastone->usersocket->peerPort();//
            QString array = lastone->usersocket->readAll(); //读出所有数据
            //QString temp=QString("[%1:%2]:").arg(ip).arg(65536+port);
            //ui->textEdit_read->append(temp);        //用追加方式写进编辑框
            //ui->textEdit_read->append(array);        //用追加方式写进编辑框

            procomm(lastone,&array);




        }
                );
        //对象主动断开处理
        connect(lastone->usersocket, &QTcpSocket::disconnected,
                [=]()
        {
            //获取对方的ip和端口
            QString ip = lastone->usersocket->peerAddress().toString();
            qint16 port = lastone->usersocket->peerPort();
            QString temp = QString("[%1:%2]:断开连接").arg(ip).arg(65536+port);//合成字符串  类似c  sprintf
            ui->textEdit_read->append(temp);//将它显示出来
            //释放资源
            if(lastone->upone!=NULL)
            {
                lastone->upone->nextone=lastone->nextone;
                if(lastone->nextone!=NULL)
                {
                    lastone->nextone->upone=lastone->upone;

                }
            }else
            {
                users=lastone->nextone;
                if(users!=NULL)
                {
                    users->upone=NULL;
                }
            }
            lastone->disconnect();
            delete lastone;
            connect_num--;
            ui->connect_num->setText(QString("[%1]").arg(connect_num));
        }
                );
        connect_num++;
        ui->connect_num->setText(QString("[%1]").arg(connect_num));
    }
            );

    mainrun = new QTimer(this);
    connect(mainrun,&QTimer::timeout,this,&MainWindow::mainrun_timeout);
    mainrun->start(10000);
}
//*******************************
void MainWindow::mainrun_timeout()
{
    //每隔10秒检测一次心跳 将停止心跳的对象清除
    int flag=0;
    tcpusers *scanall;
    scanall=users;
    do
    {
        if(scanall!=NULL)
        {
            flag=1;
            if(QDateTime::currentDateTime()>scanall->timeout->addSecs(10))
            {
                scanall->usersocket->write(QString("timeout heard").toUtf8());
                scanall->usersocket->disconnectFromHost();
            }
            scanall=scanall->nextone;
        }else
        {
            flag=0;
        }
    }while(flag);

}
//*******************************

void split_a_b(QString *str,QString *a,QString *b)
{
    //将一串字符分割成两份，以第一个空格作为分割点
    QStringList list = str->split(" ");
    *a=list[0];
     QString B;
    for(int a=1;a<list.size();a++)
    {
        if(a==list.size()-1)
        {
            B.append(list[a]);
        }else
        {
            B.append(QString("%1 ").arg(list[a]));
        }

    }
    *b=B;
}

void MainWindow::sendto(tcpusers *user,QString *comm)
{
    if(*comm==QString::fromLocal8Bit("help")||*comm==QString::fromLocal8Bit("?"))
    {

        QString temp=QString("sendto (userid) 'msg'\n");
        if(user==NULL)
        {
            ui->textEdit_read->append(temp);
        }else
        {
            user->usersocket->write(temp.toUtf8());
        }
        return ;

    }else
    {
        QString A,B;
        split_a_b(comm,&A,&B);
        unsigned int uid=0;
        uid=A.toInt();

        QString temp;
        if(user!=NULL)
        {
            temp=QString("%1:%2").arg(*(user->userid)).arg(B);
        }else
        {
            temp=QString("server:%1").arg(B);
        }

        if(uid!=0)
        {
            int flag=0;
            tcpusers *scanall;
            scanall=users;

            do
            {
                if(scanall!=NULL)
                {
                    flag=1;
                    if(scanall->usersocket!=NULL)
                    {
                        if(*(scanall->userid)==uid)
                        {
                            scanall->usersocket->write(temp.toUtf8());
                            return ;
                        }
                    }
                    scanall=scanall->nextone;
                }else
                {
                    flag=0;
                }
            }while(flag);
            if(user!=NULL)
            {
                user->usersocket->write(QString("ERROR").toUtf8());
            }else
            {
                 ui->textEdit_read->append("ERROR \n");
            }

            return ;
        }else
        {
            ui->textEdit_read->append(temp);
            return ;
        }

    }

}

void MainWindow::sendall(tcpusers *user,QString *comm)
{
    int flag=0;
    tcpusers *scanall;
    scanall=users;
    QString temp;
    if(user!=NULL)
    {
        temp=QString("%1:%2").arg(*(user->userid)).arg(*comm);
    }else
    {
        temp=QString("server:%1").arg(*comm);
    }

    do
    {
        if(scanall!=NULL)
        {
            flag=1;
            if(scanall->usersocket!=NULL)
            {              
               scanall->usersocket->write(temp.toUtf8());
            }
            scanall=scanall->nextone;
        }else
        {
            flag=0;
        }
    }while(flag);

    ui->textEdit_read->append(temp);

}

void MainWindow::distcp(tcpusers *user,QString *comm)
{
    if(user==NULL)
    {
        unsigned int uid=0;
        uid=comm->toInt();
        if(uid!=0)
        {
            int flag=0;
            tcpusers *scanall;
            scanall=users;
            do
            {
                if(scanall!=NULL)
                {
                    flag=1;
                    if(scanall->usersocket!=NULL)
                    {
                        if(*(scanall->userid)==uid)
                        {
                            scanall->usersocket->disconnectFromHost();
                            QString temp=QString("disconnect %1").arg(uid);
                            ui->textEdit_read->append(temp);
                            return ;
                        }
                    }
                    scanall=scanall->nextone;
                }else
                {
                    flag=0;
                }
            }while(flag);
        }
        QString temp=QString("error");
        ui->textEdit_read->append(temp);
    }

}
void MainWindow::tick(tcpusers *user,QString *comm)
{
    if(user!=NULL)
    {
        if(*comm==QString::fromLocal8Bit("help")||*comm==QString::fromLocal8Bit("?"))
        {
            QString temp=QString(" heard\n");
            user->usersocket->write(temp.toUtf8());
            return ;
        }else
        {
            if(*comm==QString::fromLocal8Bit("heard"))
            {
                *(user->timeout)=QDateTime::currentDateTime();
                user->usersocket->write(QString("tick heard").toUtf8());
            }
        }
    }
}
void MainWindow::procomm(tcpusers *user,QString *comm)//命令解析器  不知道怎么写 先凑合写出功能吧
{
    //命令处理,命令入口
    //可使用命令
    //1.sandto
    //2.sendall
    //3.connect
    //4.distcp

    if(*comm==QString::fromLocal8Bit("help")||*comm==QString::fromLocal8Bit("?"))
    {

        QString temp=QString("you can user:\n sandto (uid) msg\n sandall \n distcp (uid)");
        if(user==NULL)
        {
            ui->textEdit_read->append(temp);
        }else
        {
            user->usersocket->write(temp.toUtf8());
        }

        return ;
    }else
    {
        QString A,B;
        split_a_b(comm,&A,&B);
        if(A==QString::fromLocal8Bit("sandto"))
        {
            sendto(user,&B);
            return ;
        }
        if(A==QString::fromLocal8Bit("sandall"))
        {
            sendall(user,&B);
            return ;
        }
        if(A==QString::fromLocal8Bit("distcp"))
        {          
            distcp(user,&B);
            return ;
        }
        if(A==QString::fromLocal8Bit("tick"))
        {
            tick(user,&B);
            return ;
        }

    }

    //QString temp=QString("server:%1").arg(*comm);
    //ui->textEdit_read->append(temp);

}


MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_comm_button_clicked()
{
    //获取编辑器内容
    QString str=ui->comm_edit->text();
    //ui->comm_edit->setText("");
    procomm(NULL,&str);
}

void MainWindow::closeEvent(QCloseEvent *event) //获取窗口关闭信号//关闭窗口的时候断开所有tcp连接
{
    int flag=0;
    tcpusers *scanall;
    scanall=users;
    do
    {
        if(scanall!=NULL)
        {
            flag=1;
            if(scanall->usersocket!=NULL)
            {
                scanall->usersocket->write(QString("Server Close").toUtf8());
                scanall->usersocket->disconnectFromHost();
            }
            scanall=scanall->nextone;
        }else
        {
            flag=0;
        }
    }while(flag);

}

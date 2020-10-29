#ifndef CONM_H
#define CONM_H

#include <QObject>

class conm : public QObject
{
    Q_OBJECT
public:
    explicit conm(QObject *parent = nullptr);
    void test();
signals:

};

#endif // CONM_H

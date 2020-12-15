#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>
#include <QtDebug>
#include <QMessageBox>
#include <QTime>

class SerialThread: public QThread
{
    Q_OBJECT
    bool work;
    QString cstr_;
    QString br_;
    QSerialPort* serialPort;
    int qs_;

public:
    inline int getDataFromCommand(char command[1]);
    SerialThread(QObject* parent, QString cstr, QString br, int quantitySensors);
    ~SerialThread() override;
    void run() override;

signals:
    void dataSignal(int data, int sig_num);
};

#endif // SERIALTHREAD_H

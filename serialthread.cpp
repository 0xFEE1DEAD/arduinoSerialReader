#include "serialthread.h"

SerialThread::SerialThread(QObject* parent, QString cstr, QString br, int quantitySensors):QThread(parent) {
    work = true;
    cstr_ = cstr;
    br_ = br;
    qs_ = quantitySensors;
}

void SerialThread::run() {
    serialPort = new QSerialPort();
    serialPort->setPortName(cstr_);
    serialPort->setBaudRate(br_.toInt());
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if (serialPort->open(QIODevice::ReadWrite)) {
        char command[1] = {0};
        serialPort->clear();
        QTime loopStart;
        QTime loopEnd;
        QTime loopMaxTime;
        loopMaxTime.setHMS(0, 0, 0, 16);
        for(;work;) {
            loopStart = QTime::currentTime();
            for(int i = 0; i < qs_; ++i) {
                command[0] = static_cast<char>(i);
                emit dataSignal(getDataFromCommand(command), i);
            }
            loopEnd = QTime::currentTime();
            int sleep = 33 - (loopEnd.msec() - loopStart.msec());
            sleep = sleep > 0 ? sleep : 1;
            msleep(static_cast<unsigned long>(sleep));
        }
        serialPort->close();
    } else {
        QMessageBox msgBox;
        msgBox.setText("Невозможно подключиться к порту с номером " + cstr_ + " ошибка: " + serialPort->errorString());
        msgBox.exec();
    }
    delete serialPort;
}

inline int SerialThread::getDataFromCommand(char command[1]) {
    QByteArray ar;
    int d;
    char count_read_attempt = 0;
    serialPort->write(command, 1);
    //По истечению если байт не записался, пытаемся еще раз отправить
    while(!serialPort->waitForBytesWritten(20)) {
        serialPort->write(command, 1);
    }
    //Ждем пока не появяться данные  (таймаут)
    while(ar.length() < 2 && count_read_attempt < 5 && serialPort->waitForReadyRead(20)) {
        ar.append(serialPort->readAll());
        ++count_read_attempt;
    }
    d = ar[1] << 8 | ar[0];
    return d;
}

SerialThread::~SerialThread() {
    work = false;
    wait();
}

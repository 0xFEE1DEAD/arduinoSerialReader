#include "widget.h"
#include "ui_widget.h"

void Widget::addDataFromArduino(int newData, int chart_num)
{
    chartsVector[chart_num]->appendData(newData);
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    connected = false;



    const auto infos = QSerialPortInfo::availablePorts();
    if(infos.length() > 0) {
        for (const QSerialPortInfo &info : infos)
            ui->comboBox->addItem(info.portName());
    } else {
        ui->comboBox->addItem("Нет доступных портов");
        ui->comboBox->setDisabled(true);
        ui->connectButton->setDisabled(true);
        ui->boudRateCombo->setDisabled(true);
        ui->quantityCharts->setDisabled(true);
    }

    QStringList boudRateA;
    boudRateA << "1200"
              << "2400"
              << "4800"
              << "9600"
              << "19200"
              << "38400"
              << "57600"
              << "115200";

    ui->quantityCharts->setRange(1, 9);
    ui->boudRateCombo->addItems(boudRateA);
}

Widget::~Widget()
{
    if(connected) {
        disconnect(thread, &SerialThread::dataSignal, this, &Widget::addDataFromArduino);
        delete thread;
    }

    for(int i = 0; i < chartsVector.length(); ++i) {
        delete chartsVector[i];
    }

    delete ui;
}

void Widget::on_connectButton_clicked()
{
    if(!connected) {
        thread = new SerialThread(this, ui->comboBox->currentText(), ui->boudRateCombo->currentText(), ui->quantityCharts->value());
        thread->start();
        connect(thread, &SerialThread::dataSignal, this, &Widget::addDataFromArduino);
        ui->boudRateCombo->setDisabled(true);
        ui->comboBox->setDisabled(true);
        ui->quantityCharts->setDisabled(true);
        ui->connectButton->setText("Отключиться");
        connected = true;
        for(int i = 0; i < ui->quantityCharts->value(); ++i) {
          Chart* chart = new Chart("samples", "value", "Sensor " + QString::number(i));
          chartsVector.append(chart);
          ui->chartsLayout->addWidget(chart->getChart(), (i / 3), i % 3);
        }
    } else {
        disconnect(thread, &SerialThread::dataSignal, this, &Widget::addDataFromArduino);
        delete thread;
        connected = false;
        ui->boudRateCombo->setDisabled(false);
        ui->comboBox->setDisabled(false);
        ui->connectButton->setText("Подключиться");
        ui->quantityCharts->setDisabled(false);
        for(int i = 0; i < chartsVector.length(); ++i) {
            chartsVector[i]->getChart()->hide();
            ui->chartsLayout->removeWidget(chartsVector[i]->getChart());
            delete chartsVector[i];
        }
        chartsVector.clear();
    }
}

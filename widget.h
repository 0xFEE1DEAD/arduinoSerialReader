#ifndef WIDGET_H
#define WIDGET_H

#include <QtDebug>

#include <QWidget>
#include <QFile>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QChartView>
#include <QStringList>
#include "serialthread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Chart {
public:
    Chart(
            const QString& titleX = "titleX",
            const QString& titleY = "titleY",
            const QString& chartTitle = "chartTitle",
            int minY = -512,
            int maxY = 512
    ) {
            if (data.isEmpty()) {
                data.reserve(500);
                for (int i = 0; i < 500; ++i)
                    data.append(QPointF(i, 0));
            }

            chartView = new QtCharts::QChartView();
            series = new QtCharts::QLineSeries();
            chart = new QtCharts::QChart();
            axisX = new QtCharts::QValueAxis();
            axisY = new QtCharts::QValueAxis();

            series->replace(data);

            chart->legend()->hide();
            chart->addSeries(series);
            chart->setTitle(chartTitle);

            axisX->setRange(0, 500);
            axisX->setLabelFormat("%g");
            axisX->setTitleText(titleX);

            axisY->setRange(minY, maxY);
            axisY->setTitleText(titleY);

            chart->addAxis(axisX, Qt::AlignBottom);
            series->attachAxis(axisX);
            chart->addAxis(axisY, Qt::AlignLeft);
            series->attachAxis(axisY);

            chartView->setChart(chart);
            chartView->setRenderHint(QPainter::Antialiasing);
    }

    ~Chart() {
        delete axisX;
        delete axisY;
        delete series;
        delete chart;
        delete chartView;
    }

    QtCharts::QChartView* getChart() {
        return chartView;
    }

    void appendData(int newData) {
        for(int i = 0; i < data.length() - 1; ++i)
            data[i].setY(data[i+1].y());

        data.last().setY(newData);
        series->replace(data);
    }

private:
    QVector<QPointF> data;
    QtCharts::QChartView* chartView;
    QtCharts::QLineSeries* series;
    QtCharts::QChart* chart;
    QtCharts::QValueAxis* axisX;
    QtCharts::QValueAxis* axisY;
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

public slots:
    void addDataFromArduino(int newData, int comand_num);

private slots:
    void on_connectButton_clicked();

private:
    Ui::Widget *ui;
    QString connectStr;
    SerialThread* thread;
    QVector<Chart*> chartsVector;
    bool connected;
};
#endif // WIDGET_H

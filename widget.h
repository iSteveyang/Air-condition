#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QtDebug>
#include <QMutex>
#include <QThread>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <math.h>
#include "motoron.h"
#include "motoroff.h"

#define TEMP 0
#define HUMI 1

namespace Ui {
    class Widget;
}

class Widget : public QWidget{
    Q_OBJECT
public:
    Widget(QWidget *parent = 0);

    ~Widget();

    //QThread *t_irda;
    //int fd_irda;
    //int fd_smog;
    //int fd_miph;
    int fd_sht11;


protected:
    void changeEvent(QEvent *e);

public:
    Ui::Widget *ui;

    QMutex mutex;
    void Insmod_moudles();
    void delay(int time);
    void calc_sht11(float *p_humidity,float *p_temprature);
    float calc_dewpoint(float h,float t);
    void GetStatus();

    void Show_sht11();
   // void Show_irda();
   // void Show_smog();
   // void Show_miph();

private slots:


    //void on_pb_matrix_off_clicked();
    //void on_pb_matrix_on_clicked();
    void on_spinBox_valueChanged(int val);
    void on_pb_buzzer_off_clicked();
    void on_pb_buzzer_on_clicked();
    //void on_pb_led_off_clicked();
    //void on_pb_led_on_clicked();
    void on_pb_motor_off_clicked();
    void on_pb_motor_on_clicked();


public slots:
    void refreshStatus();

};

#endif // WIDGET_H

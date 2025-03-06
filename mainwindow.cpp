#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cmath>
#include <vector>
#include <QApplication>
#include <type_traits>
#define PI 3.14159265
//Число пи

float Ax, Ay, Bx, By, Tx, Ty, qngle, res;
struct Point{
    double x, y;
    Point(double x = 0, double y = 0): x(x), y(y) {}
};
//Структура точки

double calck(const Point& p1, const Point& p2){
    if(p2.x != p1.x){
        return (p2.y - p1.y) / (p2.x - p1.x);
    }
    else{
        return INT_MAX;
    }
}
//Подсчёт углового коэф. прямой


double dop(double k, double dopagl) {
    if(k != INT_MAX){
        if(1 - k * tan(dopagl) != 0){

            return (k + tan(dopagl)) / (1 - k * tan(dopagl));
        }
        else{
            return INT_MAX;
        }
    }
    else{

        return tan(PI + dopagl);
    }
}
//Подсчёт углового коэф. доп. прямых


Point cross(double k1, double c1, double k2, double c2){
    if(k1 == INT_MAX){
        double x = (c2 - c1) / k2;
        double y = c1;
        return Point(x, y);
    }
    else if(k2 == INT_MAX){
        double x = (c2 - c1) / k1;
        double y = c2;
        return Point(x, y);
    }
    else{
        double x = (c2 - c1) / (k1 - k2);
        double y = k1 * x + c1;
        return Point(x, y);
    }
}
//Поиск точки пересечений

double calcSqare(const std::vector<Point>& versh){
    double sqr = 0.0;
    int n = versh.size();
    for(int i = 0; i < n; i++){
        int j = (i + 1) % n;
        sqr += (versh[i].x * versh[j].y - versh[j].x * versh[i].y);
    }
    return abs(sqr) / 2.0;
}
//Площадь получившегося четырёхугольника


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{

    Ax = ui->crdAX->text().toFloat();
    Ay = ui->crdAY->text().toFloat();
    Bx = ui->crdBX->text().toFloat();
    By = ui->crdBY->text().toFloat();
    Tx = ui->crdTX->text().toFloat();
    Ty = ui->crdTY->text().toFloat();
    qngle = ui->qngle->text().toFloat();

    if(qngle <180){
        qngle = abs((qngle * PI)/180);
        Point A(Ax, Ay);
        Point B(Bx, By);
        Point T(Tx, Ty);

        double kAT = calck(A, T);
        double kBT = calck(B, T);

        double kA1 = dop(kAT, qngle/2);
        double kA2 = dop(kAT, -qngle/2);
        double kB1 = dop(kBT, qngle/2);
        double kB2 = dop(kBT, -qngle/2);
        double cA1, cA2, cB1, cB2;

        if(kA1 != INT_MAX){
            cA1 = A.y - kA1 * A.x;
        }
        else{
            cA1 = A.y;
        }
        if(kA2 != INT_MAX){
            cA2 = A.y - kA2 * A.x;
        }
        else{
            cA2 = A.y;
        }
        if(kB1 != INT_MAX){
            cB1 = B.y - kB1 * B.x;
        }
        else{
            cB1 = B.y;
        }
        if(kB1 != INT_MAX){
            cB2 = B.y - kB2 * B.x;

        }
        else{
            cB2 = B.y;
        }

        Point P1, P3;
        if(kA1 != kB1){
            P1 = cross(kA1, cA1, kB1, cB1);
        }
        else{
            P1 = A;
        }
        Point P2 = cross(kA1, cA1, kB2, cB2);
        if(kA1 != kB1){
            P3 = cross(kA2, cA2, kB2, cB2);
        }
        else{
            P3 = B;
        }
        Point P4 = cross(kA2, cA2, kB1, cB1);

        std::vector<Point> quad = {P1, P2, P3, P4};

        double square = calcSqare(quad);

        ui->result->setText(QString::number(square));
        ui->ptx1->setText(QString::number(P1.x));
        ui->ptx2->setText(QString::number(P2.x));
        ui->ptx3->setText(QString::number(P3.x));
        ui->ptx4->setText(QString::number(P4.x));
        ui->pty1->setText(QString::number(P1.y));
        ui->pty2->setText(QString::number(P2.y));
        ui->pty3->setText(QString::number(P3.y));
        ui->pty4->setText(QString::number(P4.y));
    }
    else{
        ui->result->setText("Err");
    }
}


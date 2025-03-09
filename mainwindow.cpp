#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cmath>
#include <vector>
#include "iostream"
#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#define PI 3.14159265
//Число пи

float Ax, Ay, Bx, By, Tx, Ty, qngle, res;
struct Point{
    double x, y;
    Point(double x = 0, double y = 0): x(x), y(y) {}
};
//Структура точки

double calculateAngle(const QPointF& center, const QPointF& point) {
    return std::atan2(point.y() - center.y(), point.x() - center.x());
}

void sortPointsClockwise(QVector<QPointF>& points) {
    QPointF center(0, 0);
    for (const auto& point : points) {
        center += point;
    }
    center /= points.size();

    std::sort(points.begin(), points.end(), [&](const QPointF& a, const QPointF& b) {
        return calculateAngle(center, a) < calculateAngle(center, b);
    });
}


bool findIntersection(const Point& p1, double angle1, const Point& p2, double angle2, Point& intersection) {
    double mtplr1 = 1, mtplr2 = 1;
    if(p1.x > Tx){
        mtplr1 = -1;
    }
    if(p2.x > Tx){
        mtplr2 = -1;
    }

    double dx1 = std::cos(angle1);
    double dy1 = std::sin(angle1);
    double dx2 = std::cos(angle2);
    double dy2 = std::sin(angle2);

    double det = dx1 * dy2 * mtplr1 * mtplr2 - dx2 * dy1 * mtplr1 * mtplr2;

    if (std::abs(det) < 1e-9) {
        return false;
    }


    double t = ((p2.x - p1.x) * dy2 * mtplr2 - (p2.y - p1.y) * dx2 * mtplr2) / det;
    double s = ((p2.x - p1.x) * dy1 * mtplr1 - (p2.y - p1.y) * dx1 * mtplr1) / det;


    if (t >= 0 && s >= 0) {
        intersection.x = p1.x + t * dx1 * mtplr1;
        intersection.y = p1.y + t * dy1 * mtplr1;
        return true;
    }

    return false;
}

double calculateAngle(const Point& center, const Point& point) {
    return std::atan2(point.y - center.y, point.x - center.x);
}

Point calculateCentroid(const std::vector<Point>& points) {
    double x = 0.0, y = 0.0;
    for (const auto& p : points) {
        x += p.x;
        y += p.y;
    }
    x /= points.size();
    y /= points.size();
    return {x, y};
}

void sortPointsByAngle(std::vector<Point>& points) {
    Point center = calculateCentroid(points);
    std::sort(points.begin(), points.end(), [&](const Point& a, const Point& b) {
        return calculateAngle(center, a) < calculateAngle(center, b);
    });
}

double calcSqare(const std::vector<Point>& versh){
    int n = versh.size();
        if (n < 3) return 0.0;
        double area = 0.0;
        for (int i = 0; i < n; i++) {
            int j = (i + 1) % n;
            area += (versh[i].x * versh[j].y - versh[j].x * versh[i].y);
        }

        return std::abs(area) / 2.0;
}


//Отрисовщик
PaintWidget::PaintWidget(QWidget *parent) : QWidget(parent) {

}

void PaintWidget::setLines(const QVector<QLineF>& lines) {
    this->lines = lines;
    update();
}

void PaintWidget::setPoints(const QVector<QPair<QPointF, QString>>& points){
    this->points = points;
    update();
}

void PaintWidget::zakras(const QPolygonF& points){
    this->quadr = points;
    update();
}




void PaintWidget::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);

    QPainter painter(this);

    painter.drawRoundedRect(0,5,width()-5, height()-7,3,3);

    painter.translate(width() / 2, height() / 2);
    painter.setPen(QPen(Qt::black, 2));


    for (const QLineF& line : lines) {
        painter.drawLine(line);
    }
    for (const auto& point : points){
        painter.drawEllipse(point.first, 3, 3);

        painter.setPen(QPen(Qt::red, 2));
        painter.drawText(point.first + QPointF(0, -5), point.second);
    }
    QPainterPath path;
    path.addPolygon(quadr);


    painter.fillPath(path, QColor(255,255,0,128));
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    paintWidget = new PaintWidget(this);
    ui->verticalLayout->addWidget(paintWidget);



    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);
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
        double atgl, btgl, atgl1, atgl2, btgl1, btgl2;
        if(Ax == Tx){
            atgl = (PI/2) * (Ty-Ay)/abs(Ty-Ay);
        }
        else{
            atgl = atan((Ty - Ay)/(Tx - Ax));
        }

        if(Bx == Tx){
            btgl = (PI/2) * (Ty-By)/abs(Ty - By);
        }
        else{
            btgl = atan((Ty - By)/(Tx - Bx));
        }
        atgl1 = atgl + qngle/2;
        atgl2 = atgl - qngle/2;
        btgl1 = btgl + qngle/2;
        btgl2 = btgl - qngle/2;

        Point intersection;
        std::vector<Point> versh;
        std::vector<QPointF> sect;
        int ctr = 0;
        bool flag = false;
        if(findIntersection(A, atgl1, B, btgl1, intersection)){
            QPointF buff(intersection.x*1 - Tx*1, intersection.y*(-1) +Ty*1);
            versh.push_back(intersection);
            sect.push_back(buff);
            ctr++;
        }
        if(findIntersection(A, atgl1, B, btgl2, intersection)){
            QPointF buff(intersection.x*1 - Tx*1, intersection.y*(-1) + Ty*1);
            sect.push_back(buff);
            versh.push_back(intersection);
            ctr++;
        }
        if(ctr == 1){
            flag = true;
        }
        if(findIntersection(A, atgl2, B, btgl1, intersection)){
            QPointF buff(intersection.x*1 - Tx*1, intersection.y*(-1) + Ty*1);
            sect.push_back(buff);
            versh.push_back(intersection);
            ctr++;
        }
        if(findIntersection(A, atgl2, B, btgl2, intersection)){
            QPointF buff(intersection.x*1 - Tx*1, intersection.y*(-1) + Ty*1);
            sect.push_back(buff);
            versh.push_back(intersection);
            ctr++;
        }
        if(ctr == 2 && flag){
            QPointF buff(Ax*1 - Tx*1, Ay*(-1) + Ty*1);
            sect.push_back(buff);
            versh.push_back(A);
            ctr+=2;
        }
        else if(ctr==2 && !flag){
            QPointF buff(Bx*1 - Tx*1, By*(-1) + Ty*1);
            sect.push_back(buff);
            versh.push_back(B);
            ctr+=2;
        }

        QVector<QLineF> lines;
        QVector<QPair<QPointF, QString>> points;


        double lengthb = 2000, lengtha = 2000;
        if(Ax > Tx){
            lengtha = lengtha * (-1);
        }
        if(Bx > Tx){
            lengthb = lengthb * (-1);
        }
        QPointF qppr1(Ax*1 + lengtha * std::cos(atgl1) - Tx*1,
                                 (-1) * Ay + (-lengtha) * std::sin(atgl1) + Ty*1);
        QPointF qppr2(Ax*1 + lengtha * std::cos(atgl2) - Tx*1,
                                 (-1) * Ay + (-lengtha) * std::sin(atgl2) + Ty*1);
        QPointF qppr3(Bx*1 + lengthb * std::cos(btgl1) - Tx*1,
                                 (-1) * By + (-lengthb) * std::sin(btgl1) + Ty*1);
        QPointF qppr4(Bx*1 + lengthb * std::cos(btgl2) - Tx*1,
                                 (-1) * By + (-lengthb) * std::sin(btgl2) + Ty*1);
        QPointF qptTA(Ax*1 + lengtha * std::cos(atgl) - Tx*1,
                                 (-1) * Ay + (-lengtha) * std::sin(atgl) + Ty*1);
        QPointF qptTB(Bx*1 + lengthb * std::cos(btgl) - Tx*1,
                                 (-1) * By + (-lengthb) * std::sin(btgl) + Ty*1);
        QPointF qptA(Ax*1 - Tx*1,Ay*(-1) + Ty*1), qptB(Bx*1 - Tx*1,By*(-1) + Ty*1);




        QLineF lineAT(qptA, qptTA);
        QLineF lineBT(qptB, qptTB);
        QLineF lineAA1(qptA, qppr1);
        QLineF lineAA2(qptA, qppr2);
        QLineF lineBB1(qptB, qppr3);
        QLineF lineBB2(qptB, qppr4);

        lines.append(lineAT);
        lines.append(lineBT);
        lines.append(lineAA1);
        lines.append(lineAA2);
        lines.append(lineBB1);
        lines.append(lineBB2);
        double square;
        sortPointsByAngle(versh);
        square = calcSqare(versh);

        if(square != 0){
            QPolygonF quadr;
            for(auto& var: sect){
                quadr << var;
            }
            ui->ptx1->setText(QString::number(ctr));
            ui->pty1->setText(QString::number(sect.size()));

            paintWidget->setLines(lines);
            sortPointsClockwise(quadr);
            paintWidget->zakras(quadr);
            points.append({QPointF(Ax*1-Tx*1, Ay*(-1) + Ty*1), "A"});
            points.append({QPointF(0, 0), "T"});
            points.append({QPointF(Bx*1-Tx*1, By*(-1)+Ty*1), "B"});

            paintWidget->setPoints(points);

            ui->result->setText(QString::number(square));
            ui->ptx1->setText(QString::number(versh[0].x));
            ui->ptx2->setText(QString::number(versh[1].x));
            ui->ptx3->setText(QString::number(versh[2].x));
            ui->ptx4->setText(QString::number(versh[3].x));
            ui->pty1->setText(QString::number(versh[0].y));
            ui->pty2->setText(QString::number(versh[1].y));
            ui->pty3->setText(QString::number(versh[2].y));
            ui->pty4->setText(QString::number(versh[3].y));
        }
        else{
            ui->result->setText("Err");
        }










    }
    else{
        ui->result->setText("Err");
    }
}


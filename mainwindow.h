#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class PaintWidget: public QWidget{
    Q_OBJECT
public:
    explicit PaintWidget(QWidget *parent = nullptr);
    void setLines(const QVector<QLineF>& lines);
    void setPoints(const QVector<QPair<QPointF, QString>>& points);
    void zakras(const QPolygonF& points);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<QLineF> lines;
    QVector<QPair<QPointF, QString>> points;
    QPolygonF quadr;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    PaintWidget *paintWidget;
};
#endif // MAINWINDOW_H

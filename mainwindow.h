#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>

#include "Box2D/Box2D.h"

#include <QTimer>
#include <QDebug>

#include <QFile>

#include <QMessageBox>

#include "roulettesimulator.h"

#include <algorithm>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT



public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void graphicalInit();

    void startRoulette(int n);


private slots:
    void on_btn_start_clicked();
    void graphicalUpdate(int tick);

    void on_nbilles_dial_sliderMoved(int position);

private:



    Ui::MainWindow *ui;

    QPixmap unknown;

    QGraphicsScene *scene;
    QGraphicsItemGroup* roue_g;
    QVector<QGraphicsEllipseItem*> g_billes;

    QStringList playersList;
    QVector<QPixmap> photos;

    QVector<QGraphicsPixmapItem*> photos_g;
    QVector<QGraphicsPolygonItem*> outlight_g;

    RouletteSimulator* rsim;

};

#endif // MAINWINDOW_H

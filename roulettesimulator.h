#ifndef ROULETTESIMULATOR_H
#define ROULETTESIMULATOR_H

#include <QDebug>
#include <QTimer>

#include <Box2D/Box2D.h>

#define FPS 60

class RouletteSimulator : public QObject
{
    Q_OBJECT

public:
    RouletteSimulator(int nplayers_p);

    void                reset();
    void                start(int nballs);

    b2Body*             getRoulette() { return roue; }
    QVector<b2Body*>    getBalls() {return b_billes;}


protected:
    void                addBall(float x, float y);

    void test();
    QVector<int> result();

private slots:
    void update();

signals:
    void beforeUpdate(int tick);
    void afterUpdate(int tick);

private:
    int nplayers;
    float baseSpeed;

    int tick;
    QTimer* timer;

    QVector<b2Body*> b_billes;
    b2World *world;
    b2Body* roue;
};

#endif // ROULETTESIMULATOR_H

#include "roulettesimulator.h"

RouletteSimulator::RouletteSimulator(int nplayers_p)
{
    nplayers=nplayers_p;

    b2Vec2 gravity(0.0f,0.0f);

    world = new b2World(gravity);

    b2BodyDef bodyDef;
    bodyDef.type = b2_kinematicBody;
    bodyDef.position.Set(0,0);

    roue = world->CreateBody(&bodyDef);
    b2ChainShape polygon;

    float radius=1.5;
    int centerx=0;
    int centery=0;



    QVector<b2Vec2> points;

    for(int i=0;i<=nplayers*3;i++) {
        float angle = ((float)(i/3)/(float)nplayers)*3.14159*2;//+4.5/360.0f*2*3.14159;

        if(i%3 == 1) {
            points.append(b2Vec2(centerx+(radius-0.05)*cos(angle), centery+(radius-0.05)*sin(angle)));
        } else if(i%3 == 0) {
            points.append(b2Vec2(centerx+radius*cos(angle-0.02), centery+radius*sin(angle-0.02)));
        } else {
            points.append(b2Vec2(centerx+radius*cos(angle+0.02), centery+radius*sin(angle+0.02)));
        }
    }

    polygon.CreateChain(points.data(),points.size());


    b2FixtureDef fdef;
    fdef.restitution=1;
    fdef.friction=100;
    fdef.density=100;
    fdef.shape = &polygon;

    roue->CreateFixture(&fdef);
    roue->SetAngularVelocity(1);

    timer = new QTimer;
    timer->setInterval(1000/FPS);
    connect(timer,SIGNAL(timeout()),this,SLOT(update()));
    timer->start(1000/FPS);


    tick = 0;

    baseSpeed = 0;

    //test();
}

void RouletteSimulator::test() {
    int ntest = 10000;
    int duration_test = 20*FPS;
    int nballes = 8;

    qDebug() << "===== ==== === TEST EN COURS === ==== =====";

    QVector<int> resultats;
    for(int i=0;i<nplayers;i++)
        resultats.append(0);


    int pourcentage = 0;

    for(int i=0;i<ntest;i++) {
        if(i*100/ntest>pourcentage) {
            pourcentage = i*100/ntest;
            qDebug() << "## " << pourcentage << "%";
        }


        reset();
        for(int i=0;i<nballes;i++) {
            addBall(rand()/(float)RAND_MAX-0.5,rand()/(float)RAND_MAX-0.5);
        }

        tick = 0;

        baseSpeed = 5 + rand()/(float)RAND_MAX - 0.5;



        for(int t=0;t<duration_test;t++) {
            update();
        }

        QVector<int> r = result();

        for(int ri=0;ri<r.size();ri++) {
            resultats[ri] += r[ri];
        }
    }

    qDebug() << "====== RESULTATS DU TEST =======";
    for(int i=0;i<resultats.size();i++) {
        qDebug() << "[" << i << "] " << resultats[i] << " = " << resultats[i]*100.0f/((float)ntest*nballes) << "%";
    }
    qDebug() << "===============================";
}

QVector<int> RouletteSimulator::result() {
    QVector<int> res;
    for(int i=0;i<nplayers;i++)
        res.append(0);

    for(int i=0;i<b_billes.size();i++) {

        b2Body *b = b_billes[i];

        float angle = 0;

        if(b->GetPosition().x > 0) {
            angle = atan((double)b->GetPosition().y/(double)b->GetPosition().x);
        } else {
            angle = 3.14159 + atan((double)b->GetPosition().y/(double)b->GetPosition().x);
        }

        angle += 3.14159/2;
        angle -= getRoulette()->GetAngle();
        while(angle < 0) {
            angle += 2*3.14159;
        }

        int id = angle / (2*3.14159) * nplayers;
        res[id] += 1;
    }

    return res;
}

void RouletteSimulator::reset() {

    timer->stop();

    for(int i=0;i<b_billes.size();i++) {
        b2Body *b = b_billes[i];
        if(world != NULL)
            world->DestroyBody(b);
    }

    b_billes.clear();
}

void RouletteSimulator::start(int nballs) {

    for(int i=0;i<nballs;i++) {
        addBall(rand()/(float)RAND_MAX-0.5,rand()/(float)RAND_MAX-0.5);
    }

    tick = 0;

    baseSpeed = 5 + rand()/(float)RAND_MAX - 0.5;
    timer->start();
}

void RouletteSimulator::addBall(float x, float y) {

    b2BodyDef bille_bodydef;
    bille_bodydef.position.Set(x,y);
    bille_bodydef.type = b2_dynamicBody;
    bille_bodydef.bullet = true;

    b2Body *bille = world->CreateBody(&bille_bodydef);

    b2CircleShape c;
    c.m_p.Set(0.0,0);
    c.m_radius = 0.09;


    b2FixtureDef fdef;
    fdef.restitution=0.95;
    fdef.friction=50;
    fdef.density=6;
    fdef.shape = &c;




    bille->CreateFixture(&fdef);
    bille->SetLinearDamping(0.5f);
    b_billes.append(bille);


    b2RopeJointDef joint;
    joint.bodyA = bille;
    joint.bodyB = roue;
    joint.localAnchorA = b2Vec2(0,0);
    joint.localAnchorB = b2Vec2(0,0);
    joint.maxLength = 1.44;
    joint.collideConnected = true;
    world->CreateJoint(&joint);
}

float max(float a, float b) {return a<b?b:a;}

void RouletteSimulator::update() {
    emit(beforeUpdate(tick));

    for(int i=0;i<b_billes.size();i++) {
        b2Body *b = b_billes[i];
        float distance = sqrt(b->GetPosition().x*b->GetPosition().x + b->GetPosition().y*b->GetPosition().y);

        b2Vec2 force((1.5-distance)*b->GetPosition().x/10,(1.5-distance)*b->GetPosition().y/10);
        b_billes[i]->ApplyLinearImpulse(force,b2Vec2(0,0),true);
    }

    tick++;
    roue->SetAngularVelocity(baseSpeed*(max(0,1.0f-tick/((float)5*60))));
    roue->GetFixtureList()->SetRestitution(1.5*exp(-tick/((float)7*60)));


    world->Step(1.0f / FPS,16,8);
    world->ClearForces();


    for(int i=0;i<b_billes.size();i++) {
        b2Body *b = b_billes[i];
        float distance = sqrt(b->GetPosition().x*b->GetPosition().x + b->GetPosition().y*b->GetPosition().y);
        if(distance > 1.45f) {
            b_billes[i]->SetTransform(b2Vec2(b->GetPosition().x*1.393/distance,b->GetPosition().y*1.393/distance),0);
        }
    }

    emit(afterUpdate(tick));
}

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("La CubiRoulette!");

    srand(time(NULL));

    QFile *f = new QFile("data/eleves.csv");


    unknown = QPixmap ("data/img/unknown.png");


    if(f->open(QFile::ReadOnly)) {
        QString content = QString(f->readAll());
        content = content.remove("\n");
        playersList = content.split(";");
        qDebug() << playersList.size() << " noms chargés!";


    } else {
        QMessageBox::warning(this,"Erreur","Problème d'ouverture du fichier data/eleves.csv","OK");
        for(int i=0;i<48;i++) {
            playersList << QString::number(i);
        }
    }

    std::random_shuffle(playersList.begin(), playersList.end());


    for(int i=0;i<playersList.size();i++) {
        QString path = "data/img/"+playersList[i].replace(" ","_")+".png";
        QPixmap photo(path);
        if(photo.isNull()) {
            QString name = playersList[i];
            QString init = "";
            for(int c=0;c<name.size();c++) {
                if(name[c].isUpper()) {
                    init += name[c];
                }
            }

            QPixmap u("data/img/unknown.png");

            QPainter p(&u);
            p.setRenderHint(QPainter::Antialiasing);


            int fontSize = 42;
            if(init.size() == 2) {
                fontSize = 130;
            } else if(init.size() == 3) {
                fontSize = 100;
            } else {
                fontSize = 70;
            }
            p.setFont(QFont("Ubuntu",fontSize));



            QFontMetrics fm = p.fontMetrics();

            QPoint center = QPoint( ( u.width()-fm.width(init))/2,300+(  - fm.height())/2 );

            p.drawText(center,init);
            photos.append(u);
        } else {
            photos.append(photo);
        }

        playersList[i].replace("_"," ");
    }

    graphicalInit();

    rsim = new RouletteSimulator(playersList.size());

    connect(rsim, SIGNAL(afterUpdate(int)),this,SLOT(graphicalUpdate(int)));
}


void MainWindow::graphicalInit() {
    scene = new QGraphicsScene(-50,-90,95,95);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

    scene->setBackgroundBrush(QBrush(QColor(12,20,30)));

    scene->addPixmap(QPixmap("data/img/title.png"))->setPos(-280,-500);

    QGraphicsEllipseItem* lbr = scene->addEllipse(-360,-360,720,720);
    lbr->setPen(QPen(QBrush(QColor(255,255,255)),85));

    QGraphicsEllipseItem* contour = scene->addEllipse(-340,-340,680,680);
    contour->setBrush(QBrush(QColor(230,230,255)));
    contour->setPen(QPen(QBrush(QColor(0,0,0)),5));

    roue_g = new QGraphicsItemGroup(contour);
  //  roue_g->addToGroup(contour);


    for(int i=0;i<playersList.size();i++) {
        float angle = (float)i/(float)playersList.size()*2*3.14159- 3.14159/2;
        QGraphicsLineItem* l = scene->addLine(10*cos(angle),10*sin(angle),340*cos(angle),340*sin(angle),QPen(QColor(i%2 == 0 ? 192: 0,0,0,192)));
        roue_g->addToGroup(l);

        QGraphicsPixmapItem* p = scene->addPixmap(photos[i]);
        p->setScale(0.18f);
        p->setPos(395*cos(angle+0.02),395*sin(angle+0.02));
        p->setRotation(angle/(2*3.14159)*360 + 90+3*360/48/4);

        roue_g->addToGroup(p);

    }



}


void MainWindow::graphicalUpdate(int tick) {

    QVector<b2Body*> b_billes = rsim->getBalls();

    for(int i=0;i<g_billes.size();i++) {

        b2Body *b = b_billes[i];
        g_billes[i]->setPos(b->GetPosition().x*229-7,b->GetPosition().y*229-7);
        float angle = 0;

        if(b->GetPosition().x > 0) {
            angle = atan((double)b->GetPosition().y/(double)b->GetPosition().x);
        } else {
            angle = 3.14159 + atan((double)b->GetPosition().y/(double)b->GetPosition().x);
        }

        if(tick >= (3+i/(float)g_billes.size())*60) {
            outlight_g[i]->setVisible(true);
            outlight_g[i]->setRotation(angle*360/(2*3.14159) - 360/(playersList.size()*2.0));
        }


        if(tick >= 0.5*60) {

            angle += 3.14159/2;
            angle -= rsim->getRoulette()->GetAngle();
            while(angle < 0) {
                angle += 2*3.14159;
            }

            int res = angle / (2*3.14159) * playersList.size();

            if(i%2 == 0) {
                ui->liste_tributs_1->item(i/2)->setText(playersList.at(res));
            } else if(i%2 == 1) {
                ui->liste_tributs_2->item(i/2)->setText(playersList.at(res));
            }

            photos_g[i]->setPixmap(photos[res]);

        }

    }

    roue_g->setRotation(rsim->getRoulette()->GetAngle()/(2*3.14159)*360);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_start_clicked()
{
    rsim->reset();
    scene->clear();
    graphicalInit();
    ui->liste_tributs_1->clear();
    ui->liste_tributs_2->clear();
    g_billes.clear();
    photos_g.clear();
    outlight_g.clear();

    int n = ui->nbilles_dial->value();

    for(int i=0;i<n;i++) {
        if(i%2 == 0) {
            ui->liste_tributs_1->addItem("Palmer?");
        } else if(i%2 == 1) {
            ui->liste_tributs_2->addItem("Palmer?");
        }
    }

    rsim->start(n);
    QVector<b2Body*> b=rsim->getBalls();

    for(int i=0;i<b.size();i++) {
        QGraphicsEllipseItem* g_bille = scene->addEllipse(-10,-10,35,35,QPen(), QBrush(QColor(200,86,23)));
        g_billes.append(g_bille);

        QGraphicsPixmapItem* p = scene->addPixmap(unknown);

        if(n<= 12) {
            p->setScale(0.5f);

            int total_height = (ceil(b.size()/2.0))*150+40;
            p->setPos(i%2==0? -540 : 520-100, -total_height/2+(i/2)*150);
        } else {
            float scale = 0.5*12/(float)n;
            p->setScale(scale);

            float height = 150*12/(float) n;
            int total_height = (ceil(b.size()/2.0))*height+40;
            p->setPos(i%2==0? -540 : 520-100*12/(float)n, -total_height/2+(i/2)*height);

        }
        photos_g.append(p);

        QVector<QPointF> points;
        points.append(QPointF(0,0));
        points.append(QPointF(1500,0));
        points.append(QPointF(1500*cos(1/48.0*2*3.14159),1500*sin(1/48.0*2*3.14159)));

        QGraphicsPolygonItem* outlight = scene->addPolygon(QPolygonF(points),QPen(QColor(255,255,255,128)),QBrush(QColor(255,192+(rand()%64 - 32),0,96)));
        outlight->setZValue(-1);
        outlight->setTransformOriginPoint(QPointF(0,0));
        outlight->setVisible(false);
        outlight_g.append(outlight);

    }

}

void MainWindow::on_nbilles_dial_sliderMoved(int position)
{
    ui->label->setText("Nombre de billes: "+QString::number(position));
}

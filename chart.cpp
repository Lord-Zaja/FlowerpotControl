#include "chart.h"
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCore/QRandomGenerator>
#include <QtCore/QDebug>
#include <cmath>

Chart::Chart(QGraphicsItem *parent, Qt::WindowFlags wFlags):
    QChart(QChart::ChartTypeCartesian, parent, wFlags),
    dx(0),
    dy(0),
    range_x(1),
    range_y({0,10}),
    m_series(0)
{
    chartSetup();
    //this->scroll(0.5,0.0);
}

/*
 * Function for search of first two max x point.x values
 */
QList<qreal*> Chart::findMaxes(QList<QPointF> points){
    qreal *max_x = nullptr;
    qreal *second_max_x = nullptr;
    qreal *max_y = nullptr;
    QPointF point;
    foreach(point,points){
        if(max_x == nullptr){
            max_x = new qreal(point.x());
            max_y = new qreal(point.y());
        }else{//DOD2LAT!!!
            if(point.x() > *max_x){
                if(second_max_x == nullptr){
                    second_max_x = new qreal(*max_x);
                }else{
                    *second_max_x = *max_x;
                }
                *max_x = point.x();
            }
            if(point.y() > *max_y){
                *max_y = point.y();
            }
        }
    }
    delete max_y;
    return {max_x,second_max_x};
}

QList<qreal> Chart::getYRange(QList<QPointF> points, qreal *max_x, qreal x_new, qreal y_new){
    QPointF point;
    qreal y = y_new;
    qreal x = x_new;
    QList<qreal> new_range_y = range_y;
    auto max = [](qreal a, qreal b){ return (a<b)?b:a; };

    // nastav range y tak, aby se velikost y osy přizpůsobila zobrazovanému maximu
    if(max_x == nullptr){
            // změní y osu podle viditelného maxima, což je vkládaný bod, protože jiný bod není zobrazen
        if(y >= 0){
            new_range_y = {0,y};

        }else{
            new_range_y = {y,0};
        }
    }else{
        new_range_y[1] = y;
        new_range_y[0] = y;
        foreach(point, points){
            if(point.x() >= max(*max_x,x)-range_x){ // body, které se zobrazují - jsou vidět v grafu
                if(point.y() > new_range_y[1]){
                    new_range_y[1] = point.y();    // změní y osu podle viditelného maxima
                }
                if(point.y() < new_range_y[0]){
                    new_range_y[0] = point.y();    // změní y osu podle viditelného minima
                }
            }
        }
    }
    return new_range_y;
}

void Chart::addPoint(qreal x, qreal y){
    //auto max = [](qreal a, qreal b){ return (a<b)?b:a; };
    // nejdřív se bude zobrazovat pouze hodina do zádu
    //najdi max hodnoty
    /*
    qreal *max_x = nullptr;
    qreal *second_max_x = nullptr;
    qreal *max_y = nullptr;
    QPointF point;
    foreach(point, m_series->points()){
        if(max_x == nullptr){
            max_x = new qreal(point.x());
            max_y = new qreal(point.y());
        }else{//DOD2LAT!!!
            if(point.x() > *max_x){
                if(second_max_x == nullptr){
                    second_max_x = new qreal(*max_x);
                }else{
                    *second_max_x = *max_x;
                }
                *max_x = point.x();
            }
            if(point.y() > *max_y){
                *max_y = point.y();
            }
        }
    }
    */
    QList<qreal*> temp = findMaxes(m_series->points());
    qreal *max_x = temp[0];
    qreal *second_max_x = temp[1];
    m_series->append(x,y);  // přidání bodu
    if(max_x != nullptr){
        if(x > *max_x){
            if(second_max_x == nullptr){
                second_max_x = new qreal(*max_x);

            }else{
                *second_max_x = *max_x;
            }
            *max_x = x;
        }
    }
    // posun v ose x
    qreal new_dx;
    if(max_x == nullptr){ // první prvek
        new_dx = x;
    }else{
        new_dx = *max_x - *second_max_x;
    }
    if(new_dx < 0){
        qDebug()<<"Záporný posun!";
    }
        /*
    }else if(second_max_x==nullptr){   //prvek nalevo - menší než maximum
        new_dx = *max_x;
    }else if(x > *max_x){   // nový prvek je nejnovější - největší bod v x ose
        new_dx = x - *max_x;
    }else{  // nový prvek není nejnovější - existuje jeden nebo více prvků, které jsou novější
        new_dx = *max_x - *second_max_x;
    }*/

    //posun v ose y
    qreal new_dy = 0.0;

    dx += new_dx;
    dy += new_dy;

    m_timer->start();  // posuň se v grafu o dx, dy .. až za daný čas .. protože je to celé podělané a jinak to nefunguje
    // musí se načítat posun dx,dy a za zpoždění posunout o tu celou velkou hodnotu

    range_y = getYRange(m_series->points(), max_x, x, y);

    /*
    // nastav range y tak, aby se velikost y osy přizpůsobila zobrazovanému maximu
    if(max_x == nullptr){
            // změní y osu podle viditelného maxima, což je vkládaný bod, protože jiný bod není zobrazen
        if(y >= 0){
            range_y = {0,y};

        }else{
            range_y = {y,0};
        }
    }else{
        range_y[1] = y;
        range_y[0] = y;
        foreach(point, m_series->points()){
            if(point.x() >= max(*max_x,x)-range_x){ // body, které se zobrazují - jsou vidět v grafu
                if(point.y() > range_y[1]){
                    range_y[1] = point.y();    // změní y osu podle viditelného maxima
                }
                if(point.y() < range_y[0]){
                    range_y[0] = point.y();    // změní y osu podle viditelného minima
                }
            }
        }
    }
    */
    m_axisY->setRange(range_y[0], range_y[1]);


/*
    qreal posun_grafu = plotArea().width() / m_axisX->tickCount();
    qreal x_size = (m_axisX->max() - m_axisX->min()) / m_axisX->tickCount();
    m_x += x_size;
    m_y = QRandomGenerator::global()->bounded(5) - 2.5;
    m_series->append(m_x, m_y);
    scroll(posun_grafu, 0); // posun pouze v ose x
    if (m_x == 100)
        m_timer->stop();
*/

    //qDebug()<<m_axisX->max();

    delete max_x;
    delete second_max_x;
}

void Chart::setAxisYLabel(QString label){
    m_axisY->setTitleText(label);
}

void Chart::chartSetup(){
    //m_timer = new QTimer;
    //connect(m_timer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
    //m_timer->setInterval(1000);

    //m_series = new QSplineSeries(this);
    m_series = new QLineSeries(this);
    QPen green(Qt::red);
    green.setWidth(3);
    m_series->setPen(green);
    //m_series->append(0, 0);
    addSeries(m_series);

    m_axisX = new QValueAxis();
    m_axisY = new QValueAxis();
    addAxis(m_axisX,Qt::AlignBottom);
    addAxis(m_axisY,Qt::AlignLeft);
    m_series->attachAxis(m_axisX);
    m_series->attachAxis(m_axisY);
    m_axisX->setTickCount(1);   // Ticky = 1 .. hodnoty budou v hodinách
    m_axisX->setRange(0, range_x);
    m_axisY->setRange(range_y[0], range_y[1]);

    m_axisX->setTitleText(tr("t [h]"));
    m_axisY->setTitleText(tr("y [-]"));
    QFont font = m_axisX->titleFont();
    font.setPixelSize(20);
    m_axisX->setTitleFont(font);
    m_axisY->setTitleFont(font);
    font = m_axisX->labelsFont();
    font.setPixelSize(20);
    m_axisX->setLabelsFont(font);
    m_axisY->setLabelsFont(font);
    m_axisX->setTickCount(10);
    m_axisY->setTickCount(10);

    //QRectF area = this->plotArea();
    //area.setWidth(100);
    //area.setHeight(200);
    //this->setPlotArea(area);

    m_timer = new QTimer;
    m_timer->setInterval(1000); // každou sekundu se aktualizují data v grafu   (graf se posune)
    connect(m_timer, SIGNAL(timeout()), this, SLOT(scrolling()));
}

void Chart::scrolling(){
    qreal posun_px;
    if(m_series->points().size() == 1){ // první prvek, posunu o zbytek horní celé části doleva
        dx = dx - range_x;
    }
    posun_px = (plotArea().width() / range_x)*dx;
    scroll(posun_px,0.0);
    dx = 0;
    dy = 0;
    m_timer->stop();
}

void Chart::setXRange(qreal range){
    this->range_x = range;
    m_axisX->setRange(0, range_x);
    if(m_series->points().size() != 0){
        QList<qreal*> temp = findMaxes(m_series->points());
        qreal *max_x = temp[0];
        //qreal *second_max_x = temp[1];
        m_axisX->setRange(*max_x-range_x,*max_x);
        //qreal new_dx = *max_x - *second_max_x;
        //dx += new_dx;
        range_y = getYRange(m_series->points(), max_x, 0, 0);
        m_axisY->setRange(range_y[0], range_y[1]);
        delete max_x;
        //delete second_max_x;
    }
}


Chart::~Chart()
{

}
/*
void Chart::handleTimeout()
{
    qreal posun_grafu = plotArea().width() / m_axisX->tickCount();
    qreal x_size = (m_axisX->max() - m_axisX->min()) / m_axisX->tickCount();
    m_x += x_size;
    m_y = QRandomGenerator::global()->bounded(5) - 2.5;
    m_series->append(m_x, m_y);
    scroll(posun_grafu, 0); // posun pouze v ose x
    if (m_x == 100)
        m_timer->stop();
}
*/

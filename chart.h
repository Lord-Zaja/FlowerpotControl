#ifndef CHART_H
#define CHART_H

#include <QtCharts/QChart>
#include <QtCore/QTimer>

QT_BEGIN_NAMESPACE
class QSplineSeries;
class QLineSeries;
class QScatterSeries;
class QValueAxis;
QT_END_NAMESPACE

QT_USE_NAMESPACE

//![1]
class Chart: public QChart
{
    Q_OBJECT
public:
    Chart(QGraphicsItem *parent = nullptr, Qt::WindowFlags wFlags = {});
    virtual ~Chart();
    void addPoint(qreal x, qreal y);
    void setAxisYLabel(QString label);
    void setXRange(qreal range);

    //přidat:
    //showDay() - vykreslí celých 24 hodin


public slots:
    void scrolling();

private:
    void chartSetup();
    QList<qreal*> findMaxes(QList<QPointF> points);
    QList<qreal> getYRange(QList<QPointF> points,qreal *max_x, qreal x_new, qreal y_new);

    qreal dx;
    qreal dy;
    qreal range_x;  // hodnota, s jakou hledím v ose x do minulosti
    QList<qreal> range_y;   // rozsah zobrazovaných hodnot v y ose <od;do>
    QTimer *m_timer;
    //QSplineSeries *m_series;
    QLineSeries *m_series;
    QStringList m_titles;
    QValueAxis *m_axisX;
    QValueAxis *m_axisY;

};
//![1]

#endif /* CHART_H */

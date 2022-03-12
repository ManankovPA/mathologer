#include <math.h>
#include <QElapsedTimer>
#include "hypocycloid_widget.h"
#include "dual.h"

QPointF to_qpoint(point<dual> point_tmp)
{
    return QPointF(point_tmp.x.re, point_tmp.y.re);
}


QPointF to_qpoint(point<double> point_tmp)
{
    return QPointF(point_tmp.x, point_tmp.y);
}


hypocycloid_widget::hypocycloid_widget(QWidget *parent)
    :QWidget(parent)
{
        last_repaint.start();
}

int hypocycloid_widget::division(int numerator, int denumerator)
{
    while(denumerator)
    {
        numerator %= denumerator;
        std::swap(numerator, denumerator);
    }
    return numerator;
}

double hypocycloid_widget::radius_min ()
{
    int tmp = division(numerator, denumerator);
    return (radius_max * (numerator / tmp)) / (denumerator / tmp);
}

template <typename T>
point<T> hypocycloid_widget::Q (T phi, point<T> center)
{   
    return point<T>::from_polar(phi, radius_max - radius_min()) + center;
}

template <typename T>
point<T> hypocycloid_widget::trace(T phi,  point<T> center)
{
    return point<T>::from_polar(phi * (1. - radius_max/radius_min()), radius_min()) + Q(phi, center);
}


void hypocycloid_widget::paintEvent(QPaintEvent *)
{
    QPainter painter (this);

    point<dual> center (400., 300.);

    painter.drawEllipse(to_qpoint(Q(dual::variable(phi), center)), radius_min(), radius_min());

    painter.setPen(QPen(Qt::red));

    painter.drawEllipse(to_qpoint(trace(dual::variable(phi), center)), 4, 4);

    QPolygonF track;

    double max_phi = (numerator / division(numerator, denumerator)) * (3.1415926535 * 2);

   /* for(double phi = 0.; phi < max_phi; phi += max_phi / (denumerator * 18) )
    {
        painter.setPen(QPen(Qt::blue));
        painter.drawEllipse(to_qpoint(trace(dual::variable(phi), center)), 4, 4);
        track << to_qpoint(trace(dual::variable(phi), center));
    }

   painter.drawPolygon(track);
*/
    QPainterPath curve;
    curve.moveTo(to_qpoint(trace(dual::variable(0), center)));

    for(double phi = 0.; phi < max_phi; phi += max_phi / (denumerator * 18))
    {
        double start = phi;
        double end = start + max_phi / (denumerator * 18);
        point<dual> start_point = trace(dual::variable(start), center);
        point<dual> end_point = trace(dual::variable(end), center);
        point<double> P0 (start_point.x.re, start_point.y.re);
        point<double> R0 (start_point.x.im, start_point.y.im);
        point<double> P3 (end_point.x.re, end_point.y.re);
        point<double> R3 (end_point.x.im, end_point.y.im);

        point<double> P1 = P0 + R0 / 3.0;
        point<double> P2 = P3 - R3 / 3.0;

        painter.setPen(QPen(Qt::blue));
        painter.drawEllipse(to_qpoint(P0), 4, 4);

        painter.setPen(QPen(Qt::green));
        painter.drawEllipse(to_qpoint(P1), 4, 4);

        painter.setPen(QPen(Qt::yellow));
        painter.drawEllipse(to_qpoint(P2), 4, 4);

        painter.setPen(QPen(Qt::black));
        curve.cubicTo(to_qpoint(P1), to_qpoint(P2), to_qpoint(P3));
    }

    painter.drawPath(curve);

    qint64 time = last_repaint.restart();
    phi -= 0.0009 * time;

    painter.setPen(QPen(Qt::red));
    painter.drawEllipse(to_qpoint(center), radius_max, radius_max);

    update();
}

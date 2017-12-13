#include "SistemasdeControle/headers/graphicLibs/plot/plot.h"

Plot::Plot(QWidget *parent):
    QGraphicsView(new QGraphicsScene, parent),
    m_coordX(0),
    m_coordY(0),
    m_chart(0),
    m_tooltip(0)
{
    setDragMode(QGraphicsView::NoDrag);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // chart
    m_chart = new QChart;
    m_chart->setMinimumSize(531, 180);
    m_chart->setMaximumSize(531, 180);
//    m_chart->setGeometry(parent->geometry());
    m_chart->legend()->hide();

    series = new QSplineSeries*[1];
    series[0] = new QSplineSeries;
//    series[0]->setMarkerSize(1.0);
//    series[0]->append(0.0,0.0);

    m_chart->addSeries(series[0]);
    m_chart->createDefaultAxes();
    m_chart->setAcceptHoverEvents(true);

    setRenderHint(QPainter::Antialiasing);
    scene()->addItem(m_chart);

    m_coordX = new QGraphicsSimpleTextItem(m_chart);
    m_coordY = new QGraphicsSimpleTextItem(m_chart);

    connect(series[0], SIGNAL(clicked(QPointF)), this, SLOT(keepCallout()));
    connect(series[0], SIGNAL(hovered(QPointF, bool)), this, SLOT(tooltip(QPointF,bool)));

    this->setMouseTracking(true);
}

Plot::Plot(const LinAlg::Matrix<double> &X, const LinAlg::Matrix<double> &Y,
           QWidget *parent):
    QGraphicsView(new QGraphicsScene, parent),
    m_coordX(0),
    m_coordY(0),
    m_chart(0),
    m_tooltip(0)
{
    setDragMode(QGraphicsView::NoDrag);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // chart
    m_chart = new QChart;
    m_chart->setMinimumSize(640, 480);
    m_chart->legend()->hide();

    series = new QSplineSeries*[X.getNumberOfRows()];
    for(unsigned i = 0; i < X.getNumberOfRows(); ++i)
    {
        series[i] = new QSplineSeries;
//        series[i]->setMarkerSize(1.0);
        for(unsigned j = 1; j <= Y.getNumberOfColumns(); ++j)
            series[i]->append(X(i,j),Y(i,j));
        m_chart->addSeries(series[i]);
    }

    m_chart->createDefaultAxes();
    m_chart->setAcceptHoverEvents(true);

    setRenderHint(QPainter::Antialiasing);
    scene()->addItem(m_chart);

    m_coordX = new QGraphicsSimpleTextItem(m_chart);
    m_coordY = new QGraphicsSimpleTextItem(m_chart);

    for(unsigned i = 0; i < X.getNumberOfRows(); ++i)
    {
        connect(series[i], SIGNAL(clicked(QPointF)), this, SLOT(keepCallout()));
        connect(series[i], SIGNAL(hovered(QPointF, bool)), this, SLOT(tooltip(QPointF,bool)));
    }

    this->setMouseTracking(true);
}

void Plot::realTimeDataUpdate(const double &x, const double &y)
{
    series[0]->append(x,y);
    m_chart->removeSeries(series[0]);
    m_chart->addSeries(series[0]);
    m_chart->createDefaultAxes();
//    m_chart->createDefaultAxes();
//    m_chart->addSeries(series[0]);
//    m_chart->;
//    m_chart->setAcceptHoverEvents(true);

//    setRenderHint(QPainter::Antialiasing);
//    scene()->addItem(m_chart);

//    m_coordX = new QGraphicsSimpleTextItem(m_chart);
//    m_coordY = new QGraphicsSimpleTextItem(m_chart);
}

void Plot::resizeEvent(QResizeEvent *event)
{
    if (scene()) {
        scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
         m_chart->resize(event->size());
         m_coordX->setPos(m_chart->size().width()/2 - 50, m_chart->size().height() - 20);
         m_coordY->setPos(m_chart->size().width()/2 + 50, m_chart->size().height() - 20);
         foreach (Callout *callout, m_callouts)
             callout->updateGeometry();
    }
    QGraphicsView::resizeEvent(event);
}

//void plot::mouseMoveEvent(QMouseEvent *event)
//{
//    m_coordX->setText(QString("X: %1").arg(m_chart->mapToValue(event->pos()).x()));
//    m_coordY->setText(QString("Y: %1").arg(m_chart->mapToValue(event->pos()).y()));
//    QGraphicsView::mouseMoveEvent(event);
//}

void Plot::keepCallout()
{
    m_callouts.append(m_tooltip);
    m_tooltip = new Callout(m_chart);
}

void Plot::tooltip(QPointF point, bool state)
{
    if (m_tooltip == 0)
        m_tooltip = new Callout(m_chart);

    if (state) {
        m_tooltip->setText(QString("X: %1 \nY: %2 ").arg(point.x()).arg(point.y()));
        m_tooltip->setAnchor(point);
        m_tooltip->setZValue(11);
        m_tooltip->updateGeometry();
        m_tooltip->show();
    } else {
        m_tooltip->hide();
    }
}

void Plot::setTitle(QString title)
{
    m_chart->setTitle(title);
}

void Plot::setRange(const double &xMin,
                    const double &xMax,
                    const double &yMin,
                    const double &yMax)
{
//    QScatterSeries *series = new QScatterSeries;
    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(xMin, xMax);
//    axisX->setTickCount(10);
    axisX->setLabelFormat("%.2f");

    m_chart->setAxisX(axisX,series[0]);
    m_chart->setAxisX(axisX,series[1]);

    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(yMin, yMax);
//    axisY->setTickCount(10);
    axisY->setLabelFormat("%.2f");

    m_chart->setAxisY(axisY,series[0]);
    m_chart->setAxisY(axisY,series[1]);
}

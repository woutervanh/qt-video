#include "tlvtouringwidget.h"
#include <QDebug>
#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QPainter>
#include <QTimer>

static void forceUpdate(void*, gpointer* user_data)
{
    TlvTouringWidget* widget = reinterpret_cast<TlvTouringWidget*>(user_data);
    widget->update();
    qDebug() << "Touring update";
}

TlvTouringWidget::TlvTouringWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      initialized(0)
{
    _bin = tlv_touring_new(TLV_TOURING_QT5GLVIDEOSINK);

    if (_bin == NULL) {
        qWarning() << "Could not get Touring object.";
    }
    else {
        g_object_set(G_OBJECT(_bin), "background", NULL, "pattern", 2, "enable-timeoverlay", FALSE, nullptr);
        g_signal_connect(G_OBJECT(_bin), "update" , G_CALLBACK(forceUpdate), this);
    }
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
       timer->start(1000);
}

void TlvTouringWidget::paintEvent(QPaintEvent *)
{
    static float fps;
    qDebug() << "Touring paint";
    if (m_frameCount%60 == 0) {
        if(m_time.elapsed() != 0)
        {
            fps = m_frameCount / (float(m_time.elapsed()) / 1000.0f);
            qDebug("FPS is %f\n", fps);
        }

        m_time.restart();
        m_frameCount = 0;
    }
    m_frameCount++;


    if(_bin)
    {
        if(initialized==0)
        {
            g_object_set (G_OBJECT(_bin), "qopenglwidget", (void*) this,NULL);
            tlv_touring_set_state(_bin, GST_STATE_PLAYING);
            initialized = 1;
            //tlv_touring_add_uridecodebin(_bin, qPrintable("rtsp://root:pass@80.251.97.97/axis-media/media.amp?streamprofile=MMI"));
        }
        else {
            QPainter painter(this);

            g_signal_emit_by_name(G_OBJECT(_bin), "paint", (void*) &painter,(qreal) x(), (qreal) y(), (qreal) width(), (qreal) height());

            QPen pen;

            pen.setStyle(Qt::DashDotLine);
            pen.setWidth(10);
            pen.setBrush(Qt::red);
            pen.setCapStyle(Qt::RoundCap);
            pen.setJoinStyle(Qt::RoundJoin);

            painter.setPen(pen);
            painter.drawRect(x(),y(),width(),height());

            painter.translate(width() - 50,height() - 50);
            painter.rotate((qreal)270);

            painter.drawText(0, 0, QString::number(fps) + " fps");
            painter.end();
        }
    }
}

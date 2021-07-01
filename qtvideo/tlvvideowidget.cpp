#include "tlvvideowidget.h"
#include <QHBoxLayout>
#include <QMediaPlayer>
#include <gst/gst.h>
#include <QLabel>
#include <QGraphicsTextItem>
#include <QPainter>
#include <QEvent>
#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QFile>

static void forceUpdate(void*, gpointer* user_data)
{
    TlvVideoWidget* widget = reinterpret_cast<TlvVideoWidget*>(user_data);
    widget->update();
}

static void cb_new_pad (GstElement *element, GstPad *pad, gpointer data)
{
    gchar *name;
    GstElement *other = (GstElement*)data;

    name = gst_pad_get_name (pad);
    g_print ("A new pad %s was created for %s\n", name, gst_element_get_name(element));
    g_free (name);

    g_print ("element %s will be linked to %s\n",
             gst_element_get_name(element),
             gst_element_get_name(other));
    gst_element_link(element, other);
}


TlvVideoWidget::TlvVideoWidget(QWidget *parent, QString pipe_desc)
    : QOpenGLWidget(parent)
{
    initialized = 0;
    m_frameCount = 0;

    GError *err = NULL;
    GstElement *decodebin, *transform;

    if(pipe_desc.length() > 0)
        pipeline = gst_parse_launch( pipe_desc.toStdString().c_str(), &err );
    else
        pipeline = gst_parse_launch( "videotestsrc ! video/x-raw,framerate=25/1,format=I420 ! qt5glvideosink name=videosink", &err );

    if (!pipeline) {
        g_printerr ("One element could not be created. Exiting.\n");
    }

    sink = gst_bin_get_by_name( GST_BIN( pipeline ), "videosink" );
    g_signal_connect(sink, "update" , G_CALLBACK(forceUpdate), this);

    decodebin = gst_bin_get_by_name( GST_BIN( pipeline ), "decodebin" );
    transform = gst_bin_get_by_name( GST_BIN( pipeline ), "transform" );

    g_signal_connect (decodebin, "pad-added", G_CALLBACK (cb_new_pad), transform);
    g_object_unref (decodebin);
    g_object_unref (transform);

    //    g_object_unref (sink);
}

TlvVideoWidget::~TlvVideoWidget()
{
    g_object_unref (sink);
    g_object_unref (pipeline);
}

void TlvVideoWidget::paintVideo(QPainter* painter, int rotateMethod)
{
    static int rotation;
    rotation++;

    switch(rotateMethod)
    {
    case 1: //method1 90CW 1
        painter->translate(width(),0);
        painter->rotate((qreal)(90));
        g_signal_emit_by_name(sink, "paint", (void*) painter,(qreal) 0, (qreal) 0, (qreal) height(), (qreal) width());
        break;
    case 2: //method2 180 2
        painter->translate(width(),height());
        painter->rotate((qreal)(180));
        g_signal_emit_by_name(sink, "paint", (void*) painter,(qreal) 0, (qreal) 0, (qreal) width(), (qreal) height());
        break;
    case 3: //method3 90CCW 3
        painter->translate(0,height());
        painter->rotate((qreal)(270));
        g_signal_emit_by_name(sink, "paint", (void*) painter,(qreal) 0, (qreal) 0, (qreal) height(), (qreal) width());
        break;
    case 4: //Horizontal flip 4
        painter->translate(width(),0);
        painter->scale(-1.0, 1.0);
        g_signal_emit_by_name(sink, "paint", (void*) painter,(qreal) 0, (qreal) 0, (qreal) width(), (qreal) height());
        break;
    case 5: //Vertical flip 5
        painter->translate(0,height());
        painter->scale(1.0, -1);
        g_signal_emit_by_name(sink, "paint", (void*) painter,(qreal) 0, (qreal) 0, (qreal) width(), (qreal) height());
        break;
    case 6: //upper-left-diagonal
        painter->translate(0,0);
        painter->rotate((qreal)(90));
        g_signal_emit_by_name(sink, "paint", (void*) painter,(qreal) 0, (qreal) 0, (qreal) height(), (qreal) -width());
        break;
    case 7: //upper-left-diagonal 7
        painter->translate(width(),height());
        painter->rotate((qreal)(90));
        g_signal_emit_by_name(sink, "paint", (void*) painter,(qreal) 0, (qreal) 0, (qreal) -height(), (qreal) width());
        break;
    case 8:
        painter->translate(width()/2,height()/2);
        painter->rotate((qreal)(rotation%360));
        g_signal_emit_by_name(sink, "paint", (void*) painter,(qreal) 0, (qreal) 0, (qreal) -width()/2, (qreal) -height()/2);
        break;
    default:
        g_signal_emit_by_name(sink, "paint", (void*) painter,(qreal) 0, (qreal) 0, (qreal) width(), (qreal) height());
        break;
    }
}

void TlvVideoWidget::paintEvent(QPaintEvent *)
{
    static float fps;

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

    if(initialized==0)
    {
        g_object_set (sink, "qopenglwidget", (void*)this, NULL);
        gst_element_set_state (pipeline, GST_STATE_PLAYING);
        initialized = 1;
    }
    else {
        QPainter painter(this);
        painter.save();

        paintVideo(&painter, 8);

        QPen pen;

        pen.setStyle(Qt::DashDotLine);
        pen.setWidth(10);
        pen.setBrush(Qt::red);
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::RoundJoin);

        painter.restore();
        painter.setPen(pen);

        painter.drawRect((qreal) 0, 0,width(),height());

        painter.translate(width() - 50,height() - 50);
        painter.rotate((qreal)270);

        painter.drawText(0, 0, QString::number(fps) + " fps");
        painter.end();
    }
}


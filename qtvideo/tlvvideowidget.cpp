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


TlvVideoWidget::TlvVideoWidget(QOpenGLWidget *parent, QString pipe_desc)
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
        makeCurrent();
        g_object_set (sink, "glcontext", (void*) QOpenGLContext::currentContext(),NULL);
        doneCurrent();
        gst_element_set_state (pipeline, GST_STATE_PLAYING);
        initialized = 1;
    }
    else {
        QPainter painter(this);
        g_signal_emit_by_name(sink, "paint", (void*) &painter,(qreal) x(), (qreal) y(), (qreal) width(), (qreal) height());

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


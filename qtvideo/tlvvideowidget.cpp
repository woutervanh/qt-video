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

static void forceUpdate(void*, gpointer* user_data)
{
    QOpenGLWidget* widget = reinterpret_cast<QOpenGLWidget*>(user_data);
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


TlvVideoWidget::TlvVideoWidget()
{
    widget1_initialized = 0;
    widget2_initialized = 0;
    m_frameCount = 0;

    widget = new QOpenGLWidget(this);
    widget->setGeometry(0,0,1280,800);
    widget->installEventFilter(this);

    widget2 = new QOpenGLWidget(this);
    widget2->setGeometry(0,0,640,400);
    widget2->installEventFilter(this);

    GError *err = NULL;
    GstElement  *sink;

    //    /* Create gstreamer elements */
    //    //pipeline1 = gst_parse_launch( "rtspsrc location=rtsp://root:pass@80.251.97.97/axis-media/media.amp?streamprofile=MMI ! decodebin name=decodebin ! imxg2dvideotransform name=transform ! video/x-raw, width=1280, height=800, format=BGRA ! qt5glvideosink name=videosink", &err );
    //    //pipeline2 = gst_parse_launch( "rtspsrc location=rtsp://root:pass@80.251.97.98/axis-media/media.amp?streamprofile=MMI ! decodebin name=decodebin ! imxg2dvideotransform name=transform ! video/x-raw, width=640, height=400, format=BGRA ! qt5glvideosink name=videosink", &err );

    //    //pipeline1 = gst_parse_launch( "videotestsrc ! qwidget5videosink name=videosink", &err );
    //    //pipeline2 = gst_parse_launch( "videotestsrc ! qwidget5videosink name=videosink", &err );

    pipeline1 = gst_parse_launch( "videotestsrc ! video/x-raw, format=I420, framerate=100/1 ! qt5glvideosink name=videosink", &err );
    pipeline2 = gst_parse_launch( "videotestsrc ! video/x-raw, format=I420, framerate=100/1 ! qt5glvideosink name=videosink", &err );

    if (!pipeline1 || !pipeline2) {
        g_printerr ("One element could not be created. Exiting.\n");
    }


    sink = gst_bin_get_by_name( GST_BIN( pipeline1 ), "videosink" );
    g_signal_connect(sink, "update" , G_CALLBACK(forceUpdate), widget);
    g_object_unref (sink);

    sink = gst_bin_get_by_name( GST_BIN( pipeline2 ), "videosink" );
    g_signal_connect(sink, "update" , G_CALLBACK(forceUpdate), widget2);
    g_object_unref (sink);

    widget->show();
    widget2->show();
}

TlvVideoWidget::~TlvVideoWidget()
{

}

bool TlvVideoWidget::eventFilter(QObject* o, QEvent* e)
{
    QOpenGLWidget* in = reinterpret_cast<QOpenGLWidget*>(o);

    if ((in==widget || in==widget2) && e && e->type() == QEvent::Paint) {
        if (m_frameCount%60 == 0) {
            if(m_time.elapsed() != 0)
                qDebug("FPS is %f\n", m_frameCount / (float(m_time.elapsed()) / 1000.0f));

            m_time.restart();
            m_frameCount = 0;
        }
        m_frameCount++;

        GstElement  *sink;
        if(in==widget && widget1_initialized==0)
        {
            sink = gst_bin_get_by_name( GST_BIN( pipeline1 ), "videosink" );
            in->makeCurrent();
            g_object_set (sink, "glcontext", (void*) QOpenGLContext::currentContext(),NULL);
            in->doneCurrent();
            gst_element_set_state (pipeline1, GST_STATE_PLAYING);
            widget1_initialized = 1;
        }
        else if(in==widget2 && widget2_initialized==0)
        {
            sink = gst_bin_get_by_name( GST_BIN( pipeline2 ), "videosink" );
            in->makeCurrent();
            g_object_set (sink, "glcontext", (void*) QOpenGLContext::currentContext(),NULL);
            in->doneCurrent();
            gst_element_set_state (pipeline2, GST_STATE_PLAYING);
            widget2_initialized = 1;
        }
        else {
            QPainter painter(in);

            sink = gst_bin_get_by_name( GST_BIN( in==widget ? pipeline1: pipeline2 ), "videosink" );
            g_signal_emit_by_name(sink, "paint", (void*) &painter,(qreal) in->x(), (qreal) in->y(), (qreal) in->width(), (qreal) in->height());
            g_object_unref (sink);

            QPen pen;

            pen.setStyle(Qt::DashDotLine);
            pen.setWidth(10);
            pen.setBrush(Qt::red);
            pen.setCapStyle(Qt::RoundCap);
            pen.setJoinStyle(Qt::RoundJoin);

            painter.setPen(pen);
            painter.drawRect(in->x(),in->y(),in->width(),in->height());

            painter.rotate((qreal)50);
            painter.setFont(QFont("Verdana", 12, QFont::Normal));
            painter.drawText(100, 100, "NORMAL TEXT");
            painter.end();
        }
    }

    // call base class implementation
   return QWidget::eventFilter(o, e);
}

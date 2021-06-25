#include <QHBoxLayout>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QWidget>
#include <gst/gst.h>
#include <QLabel>
#include <QGraphicsTextItem>
#include <QPainter>
#include <QEvent>

#include "mainwindow.h"


static void forceUpdate(void*, gpointer* user_data)
{
    QWidget* widget = reinterpret_cast<QWidget*>(user_data);
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


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    widget = new QWidget(this);
    widget->setGeometry(0,0,1280,800);
    widget->installEventFilter(this);

    widget2 = new QWidget(this);
    widget2->setGeometry(0,0,640,400);
    widget2->installEventFilter(this);

    GError *err = NULL;
    GstElement  *sink, *decodebin, *transform;
    
    /* Create gstreamer elements */
    pipeline1 = gst_parse_launch( "rtspsrc location=rtsp://root:pass@80.251.97.97/axis-media/media.amp?streamprofile=MMI ! decodebin name=decodebin ! imxg2dvideotransform name=transform ! video/x-raw, width=1280, height=800, format=BGRA ! qt5videosink name=videosink", &err );
    pipeline2 = gst_parse_launch( "rtspsrc location=rtsp://root:pass@80.251.97.98/axis-media/media.amp?streamprofile=MMI ! decodebin name=decodebin ! imxg2dvideotransform name=transform ! video/x-raw, width=640, height=400, format=BGRA ! qt5videosink name=videosink", &err );

    //pipeline1 = gst_parse_launch( "videotestsrc ! qwidget5videosink name=videosink", &err );
    //pipeline2 = gst_parse_launch( "videotestsrc ! qwidget5videosink name=videosink", &err );

    //pipeline1 = gst_parse_launch( "videotestsrc ! qt5videosink name=videosink", &err );
    //pipeline2 = gst_parse_launch( "videotestsrc ! qt5videosink name=videosink", &err );

    if (!pipeline1 || !pipeline2) {
        g_printerr ("One element could not be created. Exiting.\n");
    }

    decodebin = gst_bin_get_by_name( GST_BIN( pipeline1 ), "decodebin" );
    transform = gst_bin_get_by_name( GST_BIN( pipeline1 ), "transform" );
    sink = gst_bin_get_by_name( GST_BIN( pipeline1 ), "videosink" );

//    widget->makeCurrent();

    g_signal_connect(sink, "update" , G_CALLBACK(forceUpdate), widget);
    g_object_set (sink, "widget", (void*) widget, NULL);
//    g_object_set(sink,"glcontext", (void*) QGLContext::currentContext(), NULL);
//    widget->doneCurrent();

    g_object_unref (decodebin);
    g_object_unref (transform);
    g_object_unref (sink);

    decodebin = gst_bin_get_by_name( GST_BIN( pipeline2 ), "decodebin" );
    transform = gst_bin_get_by_name( GST_BIN( pipeline2 ), "transform" );
    sink = gst_bin_get_by_name( GST_BIN( pipeline2 ), "videosink" );

    g_signal_connect(sink, "update" , G_CALLBACK(forceUpdate), widget2);
    g_object_set (sink, "widget", (void*) widget2, NULL);

    g_object_unref (decodebin);
    g_object_unref (transform);
    g_object_unref (sink);

    gst_element_set_state (pipeline1, GST_STATE_PLAYING);
    gst_element_set_state (pipeline2, GST_STATE_PLAYING);

    widget->show();
    widget2->show();
}

MainWindow::~MainWindow()
{
}

bool MainWindow::eventFilter(QObject* o, QEvent* e)
{
    QWidget* in = reinterpret_cast<QWidget*>(o);

    if ((in==widget || in==widget2) && e && e->type() == QEvent::Paint) {
        QPainter painter(in);

        GstElement *sink = gst_bin_get_by_name( GST_BIN( in==widget ? pipeline1: pipeline2 ), "videosink" );
        g_signal_emit_by_name(sink, "paint", (void*) &painter,(qreal) in->x(), (qreal) in->y(), (qreal) in->width(), (qreal) in->height());
        g_object_unref (sink);

        QPen pen;  // creates a default pen

        pen.setStyle(Qt::DashDotLine);
        pen.setWidth(10);
        pen.setBrush(Qt::red);
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::RoundJoin);

        painter.setPen(pen);
        painter.drawRect(in->x(),in->y(),in->width(),in->height());

        painter.rotate((qreal)50);
        painter.setFont(QFont("Verdana", 12, QFont::Normal));
        painter.drawText(100, 100, "NORMAL TEXT");                  }

    // call base class implementation
    return QWidget::eventFilter(o, e);
}

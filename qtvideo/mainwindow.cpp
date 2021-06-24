#include <QHBoxLayout>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QGLWidget>
#include <gst/gst.h>
#include <QLabel>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *widget = new QWidget(this);
    widget->setGeometry(0,0,1280,800);
    
    QWidget *widget2 = new QWidget(this);
    widget2->setGeometry(0,0,640,400);
    GError *err = NULL;
    GstElement  *sink;
    
    /* Create gstreamer elements */
    pipeline1 = gst_parse_launch( "rtspsrc location=rtsp://root:pass@80.251.97.97/axis-media/media.amp?streamprofile=MMI ! decodebin ! qwidget5videosink name=videosink", &err );
//    pipeline1 = gst_parse_launch( "videotestsrc ! qwidget5videosink name=videosink", &err );
    pipeline2 = gst_parse_launch( "videotestsrc ! qwidget5videosink name=videosink", &err );


    if (!pipeline1 || !pipeline2) {
        g_printerr ("One element could not be created. Exiting.\n");
    }

    sink = gst_bin_get_by_name( GST_BIN( pipeline1 ), "videosink" );
    g_object_set (sink, "widget", (void*) widget, NULL);
    g_object_unref (sink);

    sink = gst_bin_get_by_name( GST_BIN( pipeline2 ), "videosink" );
    g_object_set (sink, "widget", (void*) widget2, NULL);
    g_object_unref (sink);

    gst_element_set_state (pipeline1, GST_STATE_PLAYING);
    gst_element_set_state (pipeline2, GST_STATE_PLAYING);

    widget->show();
    
    QWidget* test = new QWidget(this);
    test->setGeometry(0,0,640,400);
    test->setStyleSheet("border: 5px solid red;");
    
    QHBoxLayout* layout = new QHBoxLayout();
    test->setLayout(layout);

    QLabel* lbl = new QLabel();
    lbl->setText("Loading...");
    layout->addWidget(lbl);
    test->show();
}

MainWindow::~MainWindow()
{
}


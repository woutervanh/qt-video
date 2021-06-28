#ifndef TLVVIDEOWIDGET_H
#define TLVVIDEOWIDGET_H

#include <QObject>
#include <QOpenGLWidget>
#include <gst/gst.h>
#include <QTime>
#include <QMainWindow>

class TlvVideoWidget : public QMainWindow
{
    Q_OBJECT
public:
    TlvVideoWidget();
    int widget1_initialized;
    int widget2_initialized;

private:
    GstElement *pipeline1;
    GstElement *pipeline2;

    QTime m_time;
    int m_frameCount;

public:
    QOpenGLWidget *widget;
    QOpenGLWidget *widget2;
    ~TlvVideoWidget();

protected:
    bool eventFilter(QObject* o, QEvent* e);
};

#endif // TLVVIDEOWIDGET_H

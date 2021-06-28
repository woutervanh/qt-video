#ifndef TLVVIDEOWIDGET_H
#define TLVVIDEOWIDGET_H

#include <QObject>
#include <QOpenGLWidget>
#include <gst/gst.h>
#include <QTime>
#include <QMainWindow>

class TlvVideoWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    TlvVideoWidget(QOpenGLWidget *parent = 0, QString uri = "");
    ~TlvVideoWidget();
    int initialized;

private:
    GstElement *pipeline;
    GstElement  *sink;

    QTime m_time;
    int m_frameCount;

public:

protected:
    void paintEvent(QPaintEvent *);
};

#endif // TLVVIDEOWIDGET_H

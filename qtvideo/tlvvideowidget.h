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
    TlvVideoWidget(QWidget *parent = 0, QString uri = "");
    ~TlvVideoWidget();
    int initialized;

private:
    GstElement *pipeline;
    GstElement  *sink;

    QTime m_time;
    int m_frameCount;

    void paintVideo(QPainter* painter, int rotateMethod);

protected:
    void paintEvent(QPaintEvent *);
};

#endif // TLVVIDEOWIDGET_H

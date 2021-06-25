#ifndef TLVVIDEOWIDGET_H
#define TLVVIDEOWIDGET_H

#include <QObject>
#include <QGLWidget>
#include <gst/gst.h>

class TlvVideoWidget : public QGLWidget
{
    Q_OBJECT
public:
    TlvVideoWidget();

private:
    GstElement *pipeline1;
    GstElement *pipeline2;


public:
    QGLWidget *widget;
    QGLWidget *widget2;
    ~TlvVideoWidget();

signals:
    void onPaint(QPaintEvent *event);

protected:
    bool eventFilter(QObject* o, QEvent* e);
};

#endif // TLVVIDEOWIDGET_H

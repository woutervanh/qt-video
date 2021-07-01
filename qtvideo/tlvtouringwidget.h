#ifndef TLVTOURINGWIDGET_H
#define TLVTOURINGWIDGET_H

#include <QObject>
#include <QOpenGLWidget>
#include <QTime>
#include <televic-touring/tlvtour.h>

class TlvTouringWidget : public QOpenGLWidget
{
    Q_OBJECT
private:
    int initialized;
    TlvTouring *_bin;
    QTime m_time;
    int m_frameCount;

public:
    TlvTouringWidget(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *e);
};

#endif // TLVTOURINGWIDGET_H

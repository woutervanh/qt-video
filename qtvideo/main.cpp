#include <gst/gst.h>
#include <QApplication>
#include <tlvvideowidget.h>
#include <tlvtouringwidget.h>
#include <QLayout>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication a(argc, argv);
    gst_init (&argc, &argv);
    QWidget layout;

    //    TlvVideoWidget *w = new TlvVideoWidget(&layout, "rtspsrc location=rtsp://root:pass@80.251.97.97/axis-media/media.amp?streamprofile=MMI ! decodebin name=decodebin ! imxipuvideotransform name=transform output-rotation=1 ! video/x-raw, width=320, height=200, format=I420 ! qt5glvideosink name=videosink sync=false");
    //TlvVideoWidget *w = new TlvVideoWidget(&layout, "");
    TlvTouringWidget *w = new TlvTouringWidget(&layout);
    w->setGeometry(0,0,1280,800);

    //    TlvVideoWidget *pip = new TlvVideoWidget(&layout, "rtspsrc location=rtsp://root:pass@80.251.97.98/axis-media/media.amp?streamprofile=MMI ! decodebin name=decodebin ! imxipuvideotransform name=transform output-rotation=3 ! video/x-raw, width=320, height=200, format=I420  ! qt5glvideosink name=videosink sync=false");
    //TlvVideoWidget *pip = new TlvVideoWidget(&layout, "");
    //TlvTouringWidget *pip = new TlvTouringWidget(&layout);
    //pip->setGeometry(0,0,640,400);

    layout.show();

    return a.exec();
}

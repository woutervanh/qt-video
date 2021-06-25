#include <gst/gst.h>
#include <QApplication>
#include <tlvvideowidget.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    gst_init (&argc, &argv);
    TlvVideoWidget w;
    w.show();

    return a.exec();
}

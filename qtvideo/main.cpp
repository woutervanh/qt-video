#include "mainwindow.h"
#include <gst/gst.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    gst_init (&argc, &argv);
    MainWindow w;
    w.show();
    return a.exec();
}

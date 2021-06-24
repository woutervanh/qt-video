#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <gst/gst.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    GstElement *pipeline1;
    GstElement *pipeline2;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif // MAINWINDOW_H

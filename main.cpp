#include <memory>

#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    std::unique_ptr<MainWindow> w_ptr;

    try
    {
        w_ptr = std::unique_ptr<MainWindow>(new MainWindow());
    }
    catch (const std::runtime_error &e)
    {
        Q_UNUSED(e);
        return 0;
    }

    w_ptr->resize(640, 480);
    w_ptr->show();

    return a.exec();
}

#include "widget.h"
#include <QApplication>
#include "q_qchat.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setWindowTitle("Q_Q *因网络或服务器问题，无法正常使用*");
    w.setWindowIcon(QIcon(":/new/prefix1/a_5"));
    w.show();

    return a.exec();
}

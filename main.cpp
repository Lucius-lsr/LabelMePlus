#include "welcome.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Welcome w;
    w.setWindowTitle(("图片标注"));
    w.show();

    return a.exec();
}

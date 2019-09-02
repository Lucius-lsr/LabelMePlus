#ifndef LABEL3D_H
#define LABEL3D_H

#include <QMainWindow>

namespace Ui {
class Label3D;
}

class Label3D : public QMainWindow
{
    Q_OBJECT

public:
    explicit Label3D(QWidget *parent = nullptr);
    ~Label3D();

private:
    Ui::Label3D *ui;
};

#endif // LABEL3D_H

#ifndef WELCOME_H
#define WELCOME_H

#include <QMainWindow>
#include "label2d.h"
#include "label3d.h"

namespace Ui {
class Welcome;
}

class Welcome : public QMainWindow
{
    Q_OBJECT

public:
    explicit Welcome(QWidget *parent = nullptr);
    ~Welcome();

private:
    Ui::Welcome *ui;
    Label2D *label2D;
    Label3D *label3D;
};

#endif // WELCOME_H

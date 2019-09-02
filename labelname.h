#ifndef LABLENAME_H
#define LABLENAME_H

#include <QDialog>

namespace Ui {
class Labelname;
}

class Labelname : public QDialog
{
    Q_OBJECT

public:
    explicit Labelname(QWidget *parent = nullptr);
    ~Labelname();

protected slots:
    void submit();

private:
    Ui::Labelname *ui;
public:
    QString name;
};


#endif // LABLENAME_H

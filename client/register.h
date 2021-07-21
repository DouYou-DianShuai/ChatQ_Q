#ifndef REGISTER_H
#define REGISTER_H

#include <QWidget>
#include <QCloseEvent>
#include <QMessageBox>

namespace Ui {
class Register;
}

class Register : public QWidget
{
    Q_OBJECT

public:
    explicit Register(QWidget *parent = 0);
    ~Register();

private slots:
    void on_loginButton_clicked();

private:
    Ui::Register *ui;

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void mySignal(QString , QString);
    void trueSignal();
};

#endif // REGISTER_H

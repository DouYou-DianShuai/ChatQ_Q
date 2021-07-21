#ifndef ADDFRIEND_H
#define ADDFRIEND_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>

namespace Ui {
class Addfriend;
}

class Addfriend : public QWidget
{
    Q_OBJECT

public:
    explicit Addfriend(QTcpSocket *,QString,QWidget *parent = 0);
    ~Addfriend();

private slots:
    
    void on_pushButton_clicked();

    void on_cancelButton_2_clicked();

private:
    Ui::Addfriend *ui;
    QTcpSocket *tcpsocket;
    QString userName;
};

#endif // ADDFRIEND_H

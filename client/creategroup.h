#ifndef CREATEGROUP_H
#define CREATEGROUP_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>

namespace Ui {
class Creategroup;
}

class Creategroup : public QWidget
{
    Q_OBJECT

public:
    explicit Creategroup(QTcpSocket *,QString,QWidget *parent = 0);
    ~Creategroup();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::Creategroup *ui;
    QTcpSocket *tcpsocket;
    QString userName;
};

#endif // CREATEGROUP_H

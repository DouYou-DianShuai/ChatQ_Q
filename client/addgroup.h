#ifndef ADDGROUP_H
#define ADDGROUP_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>

namespace Ui {
class Addgroup;
}

class Addgroup : public QWidget
{
    Q_OBJECT

public:
    explicit Addgroup(QTcpSocket *,QString,QWidget *parent = 0);
    ~Addgroup();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::Addgroup *ui;

    QTcpSocket *tcpsocket;
    QString userName;
};

#endif // ADDGROUP_H

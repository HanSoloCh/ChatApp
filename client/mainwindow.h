#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>

#include "client.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString getNickname();

private:
    Ui::MainWindow *ui;
    Client *client;

signals:
    void sendToServer(const QString &nickname, const QString &message);

private slots:
    void slotShowMessage(const QString &nickname, const QString &message);
    void slotSendMessage();

};

#endif // MAINWINDOW_H

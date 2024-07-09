#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTimer>

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
    QTimer *sendTimer;

signals:
    void sendToServer(const QString &nickname, const QString &message, int maxSize = 512);

private slots:
    void slotShowMessage(const QString &nickname, const QString &message);
    void slotSendMessage();

    void on_spinBox_2_valueChanged(int arg1);
};

#endif // MAINWINDOW_H

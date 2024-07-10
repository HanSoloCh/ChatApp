#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTimer>
#include <QFile>

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
    void sendMessageToServer(const QString &nickname, const QString &message, int maxSize = 512);
    void sendFileToServer(const QString &nickname, QFile &file, int maxSize = 512);

private slots:
    void slotShowMessage(const QString &nickname, const QString &message);
    void slotShowFile(const QString &nickname, const QString &fileName);

    void slotSendMessage();

    void on_spinBox_2_valueChanged(int arg1);
    void slot_on_pushButton_clicked();

};

#endif // MAINWINDOW_H

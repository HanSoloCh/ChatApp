#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTimer>
#include <QFile>
#include <QUuid>
#include <QListWidgetItem>

#include "client.h"
#include "command.h"


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

    QListWidgetItem *getItemByMessageId(const QUuid messageId) const;

signals:
    void sendToServer(const BaseCommand &command);

private slots:
    void slotShowMessage(const QString &nickname, const QString &message, const QUuid messageId);
    void slotShowFile(const QString &nickname, const QString &fileName, const QUuid messageId);

    void slotServerReceivedMessage(const QUuid messageId);
    void slotAllClientsReceivedMessage(const QUuid messageId);

    void slotSendMessage();

    void on_spinBox_2_valueChanged(int arg1);
    void slot_on_pushButton_clicked();

};

#endif // MAINWINDOW_H

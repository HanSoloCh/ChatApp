#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QTimer>
#include <QUdpSocket>
#include <QUuid>

#include "client.h"
#include "messageData.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
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
    void sendToServer(const BaseMessageData &messageData);

  private slots:
    void slotShowMessage(const QString &nickname, const QString &message, const QUuid messageId);
    void slotShowFile(const QString &nickname, const QString &fileName, const QUuid messageId);

    void slotServerReceivedMessage(const QUuid messageId);
    void slotAllClientsReceivedMessage(const QUuid messageId);

    void slotSendMessage();
    void slotSendFile();

    void on_spinBox_2_valueChanged(int arg1);
};

#endif // MAINWINDOW_H

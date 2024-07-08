#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDataStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::slotSendMessage);

    client = new Client(2323, this);
    connect(client, &Client::showMessage, this, &MainWindow::slotShowMessage);
    connect(this, &MainWindow::sendToServer, client, &Client::slotSendToServer);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::getNickname()
{
    QString nickname = ui->nicknameEdit->text();
    if (nickname.isEmpty())
        return "Unknown user";

    return nickname;
}


void MainWindow::slotShowMessage(const QString &message, const QString &nickname)
{
    ui->textBrowser->append(QString("%1: %2").arg(nickname, message));
}


void MainWindow::slotSendMessage()
{
    emit sendToServer(ui->messageEdit->text(), getNickname());
    ui->messageEdit->clear();
}


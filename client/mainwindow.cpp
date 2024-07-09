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

    sendTimer = new QTimer(this);
    connect(sendTimer, &QTimer::timeout, client, &Client::slotSendPackage);
    sendTimer->start(0);
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


void MainWindow::slotShowMessage(const QString &nickname, const QString &message)
{
    ui->textBrowser->append(QString("%1: %2").arg(nickname, message));
}


void MainWindow::slotSendMessage()
{
    QString text = ui->messageEdit->text();
    slotShowMessage(QString("Me"), text);
    emit sendToServer(getNickname(), text, ui->spinBox->value());
    ui->messageEdit->clear();
}


void MainWindow::on_spinBox_2_valueChanged(int arg1)
{
    sendTimer->start(arg1 * 1000);
}


#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDataStream>
#include <QFile>
#include <QFileInfo>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::slotSendMessage);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::slot_on_pushButton_clicked);

    client = new Client(2323, this);
    connect(client, &Client::showMessage, this, &MainWindow::slotShowMessage);
    connect(client, &Client::showFile, this, &MainWindow::slotShowFile);
    connect(this, &MainWindow::sendMessageToServer, client, &Client::slotSendMessageToServer);
    connect(this, &MainWindow::sendFileToServer, client, &Client::slotSendFileToServer);

    sendTimer = new QTimer(this);
    connect(sendTimer, &QTimer::timeout, client, &Client::slotSendPackage);
    sendTimer->start(1);
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
    ui->listWidget->addItem(QString("%1: %2").arg(nickname, message));
}

void MainWindow::slotShowFile(const QString &nickname, const QString &fileName)
{
    QListWidgetItem *item = new QListWidgetItem();
    ui->listWidget->addItem(item);

    QPushButton *downloadButton = new QPushButton(QString("%1 send %2").arg(nickname, QFileInfo(fileName).baseName()), ui->listWidget);
    ui->listWidget->setItemWidget(item, downloadButton);

    connect(downloadButton, &QPushButton::clicked, [fileName] {
        QString savePath = QFileDialog::getSaveFileName(nullptr, "Save File", QFileInfo(fileName).fileName());
        if (!savePath.isEmpty())
        {
            QFile::copy(fileName, savePath);
        }
    });
}


void MainWindow::slotSendMessage()
{
    QString text = ui->messageEdit->text();
    slotShowMessage(QString("Me"), text);
    emit sendMessageToServer(getNickname(), text, ui->spinBox->value());
    ui->messageEdit->clear();
}


void MainWindow::on_spinBox_2_valueChanged(int arg1)
{
    if (arg1 == 0)
        sendTimer->start(1);
    else
        sendTimer->start(arg1 * 1000);
}


void MainWindow::slot_on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select File to Send");

    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly))
        {
            slotShowFile(QString("You"), fileName);
            emit sendFileToServer(getNickname(), file, ui->spinBox->value());
            file.close();
        }
    }
}


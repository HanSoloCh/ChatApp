#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDataStream>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::slotSendMessage);
    connect(ui->fileSendButton, &QPushButton::clicked, this, &MainWindow::slotSendFile);

    client = new Client(ui->currentPort->value(), this);
    connect(client, &Client::showMessage, this, &MainWindow::slotShowMessage);
    connect(client, &Client::showFile, this, &MainWindow::slotShowFile);

    connect(client, &Client::showMessageReceived, this, &MainWindow::slotShowMessageReceived);

    connect(this, &MainWindow::sendMessage, client, &Client::slotSendMessage);
    connect(this, &MainWindow::portChanged, client, &Client::slotPortChanged);

    sendTimer = new QTimer(this);
    connect(sendTimer, &QTimer::timeout, client, &Client::slotSendPackage);
    sendTimer->start(1);
}

MainWindow::~MainWindow() { delete ui; }

QString MainWindow::getNickname()
{
    QString nickname = ui->nicknameEdit->text();
    if (nickname.isEmpty())
        return "Unknown user";

    return nickname;
}

QListWidgetItem *MainWindow::getItemByMessageId(const QUuid messageId) const
{
    QListWidgetItem *item;
    for (int i = 0; i < ui->listWidget->count(); ++i)
    {
        item = ui->listWidget->item(i);
        if (item->data(Qt::UserRole).toUuid() == messageId)
            return item;
    }
    return nullptr;
}

void MainWindow::slotShowMessage(const QString &nickname, const QString &message, const QUuid messageId)
{
    QListWidgetItem *item = new QListWidgetItem(QString("%1: %2").arg(nickname, message));
    item->setData(Qt::UserRole, messageId);
    ui->listWidget->addItem(item);
}

void MainWindow::slotShowFile(const QString &nickname, const QString &fileName, const QUuid messageId)
{
    QListWidgetItem *item = new QListWidgetItem();
    item->setData(Qt::UserRole, messageId);
    ui->listWidget->addItem(item);

    QPushButton *downloadButton =
        new QPushButton(QString("%1 send %2").arg(nickname, QFileInfo(fileName).baseName()), ui->listWidget);
    ui->listWidget->setItemWidget(item, downloadButton);
    connect(downloadButton, &QPushButton::clicked,
            [fileName]
            {
                QString savePath = QFileDialog::getSaveFileName(nullptr, "Save File", QFileInfo(fileName).fileName());
                if (!savePath.isEmpty())
                {
                    QFile::copy(fileName, savePath);
                }
            });
}

void MainWindow::slotShowMessageReceived(const QUuid messageId)
{
    QListWidgetItem *item = getItemByMessageId(messageId);
    if (item)
        item->setText(item->text() + "~");
}

void MainWindow::slotSendMessage()
{
    QUuid messageId = QUuid::createUuid();
    slotShowMessage(QString("Me"), ui->messageEdit->text(), messageId);
    TextMessageData messageData(UserMessage, getNickname(), ui->messageEdit->text(), messageId, ui->spinBox->value());
    ui->messageEdit->clear();
    emit sendMessage(messageData, qMakePair(ui->senderIP->text(), ui->senderPort->value()));
}

void MainWindow::slotSendFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select File to Send");

    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly))
        {
            QUuid messageId = QUuid::createUuid();
            slotShowFile(QString("You"), fileName, messageId);
            FileMessageData messageData(UserFile, getNickname(), file, messageId, ui->spinBox->value());
            emit sendMessage(messageData, qMakePair(ui->senderIP->text(), ui->senderPort->value()));
            file.close();
        }
    }
}

void MainWindow::on_spinBox_2_valueChanged(int arg1)
{
    if (arg1 == 0)
        sendTimer->start(1);
    else
        sendTimer->start(arg1);
}


void MainWindow::on_currentPort_valueChanged(int arg1)
{
    emit portChanged(arg1);
}

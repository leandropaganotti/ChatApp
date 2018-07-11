#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QObject>
#include <QStringList>
#include <QDebug>
#include <QMessageBox>
#include <QKeyEvent>

std::string timestamp2string(std::time_t timestamp)
{
    struct tm t;
    if ( gmtime_r( &timestamp, &t ) == nullptr )
    {
        timestamp = 0;
        gmtime_r( &timestamp, &t );
    }
    char buffer[64] = { };
    strftime( buffer, 64, "%F %T", &t );
    return buffer;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->username->setFocus();
    ui->message->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_send_clicked()
{

    if (ui->users->currentText() == "")
    {
        QMessageBox msgBox;
        msgBox.setText("Please inform a user to send message");
        msgBox.exec();
        return;
    }

    chatclient->writeTo(ui->message->toPlainText().toStdString().c_str(), ui->users->currentData().toInt());


    QString str = QString("%1 %2 says to %3: %4").arg( QString::fromStdString(timestamp2string( time(nullptr))))
            .arg(ui->username->text())
            .arg(ui->users->currentText())
            .arg(ui->message->toPlainText());

    ui->history->append(str);
    ui->message->clear();
}

void MainWindow::on_connect_clicked()
{
    if (ui->username->text().trimmed() == "")
    {
        QMessageBox msgBox;
        msgBox.setText("Please inform your username");
        msgBox.exec();
        ui->username->setFocus();
        return;
    }

    chatclient.reset(new ChatClient(ui->host->text(), ui->port->text().toInt(), ui->username->text() ));
    ui->users->clear();
    connect(chatclient.get(), SIGNAL(allUsers(QString)), this, SLOT(on_chatUsers(QString)));
    connect(chatclient.get(), SIGNAL(newMessage(QString)), this, SLOT(on_newMessage(QString)));
    connect(chatclient.get(), SIGNAL(newUser(QString)), this, SLOT(on_newUser(QString)));
    connect(chatclient.get(), SIGNAL(userRemoved(QString)), this, SLOT(on_userRemoved(QString)));
    if(!chatclient->connect())
    {
        QMessageBox msgBox;
        msgBox.setText("Connection failed");
        msgBox.exec();
        ui->username->setFocus();
    }
    else
    {
        ui->send->setEnabled(true);
        ui->history->setEnabled(true);
        ui->users->setEnabled(true);
        ui->message->setEnabled(true);
        ui->message->setFocus();
    }
}

void MainWindow::on_chatUsers(QString str)
{

    QStringList list = str.split(":");

    for(int i = 1; i < list.size()-1; i+=2)
    {
        if(list[i].toInt()!=chatclient->getUserId())
            ui->users->addItem(list[i+1].trimmed(), list[i].toInt());
    }
}

void MainWindow::on_newMessage(QString msg)
{

    QRegExp rx("(\\d+):(\\d+):(\\d+):([\\s\\S]*)");
    rx.indexIn(msg);
    QStringList list = rx.capturedTexts();

    QString from, to;

    int idx = ui->users->findData(list[2].toInt());
    if (idx>=0)
    {
        from = ui->users->itemText(idx);
    }
    idx = ui->users->findData(list[3].toInt());
    if (idx>=0)
    {
        to = ui->users->itemText(idx);
    }

    QString str = QString("%1 %2 says to %3: %4").arg( QString::fromStdString(timestamp2string( time(nullptr))))
            .arg(from)
            .arg(to)
            .arg(list[4].trimmed());
    ui->history->append(str);
}

void MainWindow::on_newUser(QString msg)
{
    QStringList list = msg.split(":");
    ui->users->addItem(list[2].trimmed(), list[1].toInt());
}

void MainWindow::on_userRemoved(QString)
{

}

void MainWindow::on_username_returnPressed()
{
    if(ui->username->text().trimmed() != "")
        on_connect_clicked();
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->message && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Return)
        {
            on_send_clicked();
            return true;
        }
    }
    return false;
}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QObject>
#include <QStringList>
#include <QDebug>

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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_sendbtn_clicked()
{
    chatclient->writeTo(ui->message->toPlainText().toStdString().c_str(), ui->users->currentData().toInt());


    QString str = QString("%1 %2 says to %3: %4").arg( QString::fromStdString(timestamp2string( time(nullptr))))
            .arg(ui->username->text())
            .arg(ui->users->currentText())
            .arg(ui->message->toPlainText());

    ui->history->append(str);
    ui->message->clear();
}

void MainWindow::on_pushButton_clicked()
{
    chatclient.reset(new ChatClient(ui->host->text(), ui->port->text().toInt(), ui->username->text() ));
    ui->users->clear();
    connect(chatclient.get(), SIGNAL(allUsers(QString)), this, SLOT(on_chatUsers(QString)));
    connect(chatclient.get(), SIGNAL(newMessage(QString)), this, SLOT(on_newMessage(QString)));
    connect(chatclient.get(), SIGNAL(newUser(QString)), this, SLOT(on_newUser(QString)));
    connect(chatclient.get(), SIGNAL(userRemoved(QString)), this, SLOT(on_userRemoved(QString)));
    chatclient->connect();
}

void MainWindow::on_chatUsers(QString str)
{

    QStringList list = str.split(":");

    for(int i = 1; i < list.size()-1; i+=2)
    {
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


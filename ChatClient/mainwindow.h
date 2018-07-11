#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "chatclient.h"
#include <memory>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_send_clicked();

    void on_connect_clicked();

    void on_chatUsers(QString);

    void on_newMessage(QString);

    void on_newUser(QString);

    void on_userRemoved(QString);

    void on_username_returnPressed();

private:
    Ui::MainWindow *ui;

    std::unique_ptr<ChatClient> chatclient;

    bool eventFilter(QObject *object, QEvent *event);
};



#endif // MAINWINDOW_H

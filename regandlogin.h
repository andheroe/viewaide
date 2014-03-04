#ifndef REGANDLOGIN_H
#define REGANDLOGIN_H

#include <QMainWindow>
#include <QThread>
#include <QTime>
#include <QLabel>
#include <QtWebKit/QtWebKit>

namespace Ui {
class RegAndLogIn;
}

class RegAndLogIn : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit RegAndLogIn(QWidget *parent = 0);
    ~RegAndLogIn();
    
private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_clicked();
    void on_pushButton_5_clicked();
    void on_lineEdit_3_textChanged(const QString &arg1);
    void on_lineEdit_4_textChanged(const QString &arg1);
    void on_lineEdit_editingFinished();
    void on_lineEdit_5_editingFinished();
    void on_lineEdit_2_editingFinished();
    void helpIn();
    void helpReg();
    void deal();
    void isAccCreate();
    void dataAcceptedOnServ();
    void dataNotAcceptedOnServ();
    void on_pushButton_7_clicked();
    void on_lineEdit_6_editingFinished();
    void on_lineEdit_3_editingFinished();
    void on_lineEdit_4_editingFinished();

private:
    Ui::RegAndLogIn *ui;

public slots:
    void hideLogin();
    void inputToApp();
    void notInputToApp();
    void slotLogout();

signals:
    void netIsNotWorking();
    void fileWasCreate();
    void windowCreate();
    void sigRunMainProgram();

protected:
    void showEvent(QShowEvent *e)
    {
        QMainWindow::showEvent(e);
        emit windowCreate();
    }
};

#endif // REGANDLOGIN_H

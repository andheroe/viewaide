#ifndef CONNECTWITHSERVER_H
#define CONNECTWITHSERVER_H

#include <QObject>
#include <QFile>
#include <QtNetwork/QNetworkReply>
#include <QMessageBox>
#include <QDateTime>
#include <QEventLoop>
#include <QThread>
#include <QDir>

class ConnectWithServer : public QObject
{
    Q_OBJECT
public:
    explicit ConnectWithServer(QObject *parent = 0);
    void registration(QString,QString,QString,QString);
    void login(QString,QString);
    void uploadAllData(QString, QString surl, QString request_name);
    bool netIsWorking();
    
signals:
    void notConnectWithServer();
    void trueLogin();
    void falseLogin();
    void trueReg();
    void falseReg();

public slots:
    void allVarWithPhoto(QImage,bool,bool,bool,bool,bool,bool,bool,bool,bool,bool,bool,float);
    void loginRead();
    void regRead();

private:
    QNetworkReply *rep;
    QNetworkReply *netReply;
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkRequest req;

private slots:

};

#endif // CONNECTWITHSERVER_H

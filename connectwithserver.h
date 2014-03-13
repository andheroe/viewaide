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
    bool OpenEyes;
    bool NarrowedEyes;
    bool CloseEyes;
    bool NormalDist;
    bool NearDist;
    bool NormalHeight;
    bool Highly;
    bool Low;
    bool NormalLight;
    bool LightToFace;
    bool BadLight;
    float dist;
    void uploadFile(QString);
    void registration(QString,QString,QString,QString);
    void login(QString,QString);
    void uploadVariable(bool,bool,bool,bool,bool,bool,bool,bool,bool,bool,bool,float);
    void uploadAllData(QString,QString);
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

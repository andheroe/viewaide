#include "connectwithserver.h"

ConnectWithServer::ConnectWithServer(QObject *parent) :
    QObject(parent)
{
}


void ConnectWithServer::allVarWithPhoto(QImage img,bool OpenEyes,bool NarrowedEyes,bool CloseEyes,bool NormalDist,bool NearDist,bool NormalHeight,bool Highly,bool Low,bool NormalLight,bool LightToFace,bool BadLight,float dist)
{
    QString filename;
    QDateTime NowDateTime;
    NowDateTime=QDateTime::currentDateTime();
    filename=NowDateTime.toString("yyyyMdHms");
    filename.append(".jpg");
    img.save(filename, "jpg");
}

void ConnectWithServer::registration(QString firstname,QString secondname,QString mail,QString password)
{
    QUrl url("http://viewaide.com/signup.php");

    req.setUrl(url);
    req.setRawHeader("Host", "viewaide.com");
    req.setRawHeader("Content-type", "multipart/form-data, boundary=AyV04a");
    req.setRawHeader("Cache-Control", "no-cache");
    req.setRawHeader("Accept","*/*");

    QByteArray body;
    body.append("--AyV04a\r\n");
    body.append("Content-disposition: form-data; name=\"firstname\"\r\n");
    body.append("\r\n");
    body.append(firstname);
    body.append("\r\n");
    body.append(secondname);
    body.append("\r\n");
    body.append(mail);
    body.append("\r\n");
    body.append(password); // ЗАШИФРУЙ МЕНЯ, РАК
    body.append("\r\n");
    body.append("agree");
    body.append("\r\n");
    body.append("--AyV04a\r\n");
    body.append("Content-Transfer-Encoding: binary\r\n");
    body.append("--AyV04a--");

    rep=manager->post(req,body);
    QEventLoop loop;
    connect(rep,SIGNAL(finished()),this,SLOT(regRead()));
    loop.exec();
}

void ConnectWithServer::login(QString logIn, QString password)
{
    QUrl url("http://viewaide.com/login.php");

    req.setUrl(url);
    req.setRawHeader("Host", "viewaide.com");
    req.setRawHeader("Content-type", "multipart/form-data, boundary=AyV04a");
    req.setRawHeader("Cache-Control", "no-cache");
    req.setRawHeader("Accept","*/*");

    QByteArray body;
    body.append("--AyV04a\r\n");
    body.append("Content-disposition: form-data; name=\"email\"\r\n");
    body.append("\r\n");
    body.append(logIn);
    body.append("\r\n");
    body.append(password);
    body.append("\r\n");
    body.append("--AyV04a\r\n");
    body.append("Content-Transfer-Encoding: binary\r\n");
    body.append("--AyV04a--");

    rep=manager->post(req, body);
    QEventLoop loop;
    connect(rep,SIGNAL(finished()),this,SLOT(loginRead()));
    loop.exec();
}

void ConnectWithServer::uploadAllData(QString dataToServer, QString surl, QString request_name)
{
    QUrl url(surl);

    req.setUrl(url);
    req.setRawHeader("Host", "viewaide.com");
    req.setRawHeader("Content-type", "multipart/form-data, boundary=AyV04a");
    req.setRawHeader("Cache-Control", "no-cache");
    req.setRawHeader("Accept","*/*");

    QByteArray body;
    body.append("--AyV04a\r\n");
    body.append("Content-disposition: form-data; name=\""+request_name+"\"\r\n");
    body.append("\r\n");
    body.append(dataToServer);
    body.append("\r\n");
    body.append("--AyV04a\r\n");
    body.append("Content-Transfer-Encoding: binary\r\n");
    body.append("--AyV04a--");

    rep=manager->post(req, body);
    QEventLoop loop;
    connect(rep,SIGNAL(finished()),&loop,SLOT(quit()));
    loop.exec();
}

bool ConnectWithServer::netIsWorking()
{
    req.setUrl(QUrl("http://viewaide.com"));
    rep=manager->get(req);
    QEventLoop loop;
    connect(rep, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    if(rep->error()==QNetworkReply::NoError)
        return true;
    else
        return false;
}

void ConnectWithServer::loginRead()
{
    if (rep->error() == QNetworkReply::NoError)
    {
        QByteArray replyBytes;
        replyBytes=rep->readAll();
        QString message=replyBytes.constData();
        QString wordTrue;
        bool find(false);
        for(int i=0;i<message.length();i++)
        {
            if((message.at(i)!=' ')&&(message.at(i)!='<')&&(message.at(i)!='>'))
                wordTrue.append(message.at(i));
            else
            {
                if(wordTrue=="true")
                    find=true;
                wordTrue="";
            }
        }
        if(find==true)
            emit trueLogin();
        else
            emit falseLogin();
    }
    else
    {
        emit notConnectWithServer();
    }
}

void ConnectWithServer::regRead()
{
    if (rep->error() == QNetworkReply::NoError)
    {
        QByteArray replyBytes;
        replyBytes=rep->readAll();
        QString message=replyBytes.constData();
        QString wordTrue;
        bool find(false);
        for(int i=0;i<message.length();i++)
        {
            if((message.at(i)!=' ')&&(message.at(i)!='<')&&(message.at(i)!='>'))
                wordTrue.append(message.at(i));
            else
            {
                if(wordTrue=="true")
                    find=true;
                wordTrue="";
            }
        }
        if(find==true)
            emit trueReg();
        else
            emit falseReg();
    }
    else
    {
        emit notConnectWithServer();
    }
}

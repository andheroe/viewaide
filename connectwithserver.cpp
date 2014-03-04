#include "connectwithserver.h"

ConnectWithServer::ConnectWithServer(QObject *parent) :
    QObject(parent)
{
}

void ConnectWithServer::uploadFile(QString filename)
{
    if (filename.isEmpty())
        return;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox msgBox;
        msgBox.setText("NO FILE!");
        msgBox.exec();
        return;
    }

    QUrl url("http://livedp.org.ua/eyedoc/upload.php");

    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("Host", "viewaide.com");
    request.setRawHeader("Content-type", "multipart/form-data, boundary=AyV04a");
    request.setRawHeader("Cache-Control", "no-cache");
    request.setRawHeader("Accept","*/*");

    QByteArray bytes;
    bytes.append("--AyV04a\r\n");
    bytes.append("Content-disposition: form-data; name=\"submit\"\r\n");
    bytes.append("\r\n");
    bytes.append("1");
    bytes.append("\r\n");
    bytes.append("--AyV04a\r\n");
    bytes.append("Content-Disposition: file; name=\"file\"; filename=\"logo.png\"\r\n");
    bytes.append("Content-Transfer-Encoding: binary\r\n");
    bytes.append("\r\n");
    while (!file.atEnd())
        bytes.append(file.readLine());
    bytes.append("\r\n");
    bytes.append("--AyV04a--");

    QNetworkAccessManager* pManager = new QNetworkAccessManager();
    pManager->post(request, bytes);
}

void ConnectWithServer::uploadVariable(bool OpenEyes,bool NarrowedEyes,bool CloseEyes,
                              bool NormalDist,bool NearDist,bool NormalHeight,
                              bool Highly,bool Low,bool NormalLight,bool LightToFace,
                              bool BadLight,float dist)
{
    QUrl url("http://livedp.org.ua/eyedoc/upload.php");

    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("Host", "viewaide.com");
    request.setRawHeader("Content-type", "multipart/form-data, boundary=AyV04a");
    request.setRawHeader("Cache-Control", "no-cache");
    request.setRawHeader("Accept","*/*");

    QByteArray body;
    body.append("--AyV04a\r\n");
    body.append("Content-disposition: form-data; name=\"dist\"\r\n");
    body.append("\r\n");
    body.append(body.number(OpenEyes));
    body.append("\r\n");
    body.append(body.number(NarrowedEyes));
    body.append("\r\n");
    body.append(body.number(CloseEyes));
    body.append("\r\n");
    body.append(body.number(NormalDist));
    body.append("\r\n");
    body.append(body.number(NearDist));
    body.append("\r\n");
    body.append(body.number(NormalHeight));
    body.append("\r\n");
    body.append(body.number(Highly));
    body.append("\r\n");
    body.append(body.number(Low));
    body.append("\r\n");
    body.append(body.number(NormalLight));
    body.append("\r\n");
    body.append(body.number(LightToFace));
    body.append("\r\n");
    body.append(body.number(BadLight));
    body.append("\r\n");
    body.append(body.number(dist));
    body.append("\r\n");
    body.append("--AyV04a\r\n");
    body.append("Content-Transfer-Encoding: binary\r\n");
    body.append("--AyV04a--");

    QNetworkAccessManager *mgr = new QNetworkAccessManager();
    mgr->post(request, body);
}

void ConnectWithServer::allVarWithPhoto(QImage img,bool OpenEyes,bool NarrowedEyes,bool CloseEyes,bool NormalDist,bool NearDist,bool NormalHeight,bool Highly,bool Low,bool NormalLight,bool LightToFace,bool BadLight,float dist)
{
    QString filename;
    QDateTime NowDateTime;
    NowDateTime=QDateTime::currentDateTime();
    filename=NowDateTime.toString("yyyyMdHms");
    filename.append(".jpg");
    img.save(filename, "jpg");
    uploadFile(filename);
    uploadVariable(OpenEyes,NarrowedEyes,CloseEyes,NormalDist,NearDist,NormalHeight,Highly,Low,NormalLight,LightToFace,BadLight,dist);
    QFile file(filename);
    file.remove();
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

void ConnectWithServer::uploadAllData(QString dataToServer, QString urlstring)
{
    QUrl url(urlstring);

    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("Host", "viewaide.com");
    request.setRawHeader("Content-type", "multipart/form-data, boundary=AyV04a");
    request.setRawHeader("Cache-Control", "no-cache");
    request.setRawHeader("Accept","*/*");

    QByteArray body;
    body.append("--AyV04a\r\n");
    body.append("Content-disposition: form-data; name=\"allData\"\r\n");
    body.append("\r\n");
    body.append(dataToServer);
    body.append("\r\n");
    body.append("--AyV04a\r\n");
    body.append("Content-Transfer-Encoding: binary\r\n");
    body.append("--AyV04a--");

    QNetworkAccessManager *mgr = new QNetworkAccessManager();
    mgr->post(request, body);
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

#include "downloadfile.h"

DownloadFile* DownloadFile::down_file = 0;

DownloadFile::DownloadFile(QObject *parent) :
    QObject(parent)
{
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(downloadFinished(QNetworkReply*)));
}

void DownloadFile::setTarget(const QString &t)
{
    this->target = t;
}

DownloadFile *DownloadFile::GetInstance()
{
    if ( down_file == 0 )
        down_file = new DownloadFile;
    return down_file;
}

void DownloadFile::emitDownload(const QString& url_path)
{
    emit sigDownload(url_path);
}

void DownloadFile::downloadFinished(QNetworkReply *data)
{
    QString file_name = target.section('/', -1);
    //int ind = file_name.indexOf(".");
    //file_name.insert(ind,"_new");
    QString path_to_file = QDir::homePath();
    path_to_file += "/Viewaide/";
    path_to_file += file_name;
    const QByteArray sdata = data->readAll();
    if ( sdata.length() == 0 )
        return;
    QFile localFile(path_to_file);
    if (!localFile.open(QIODevice::WriteOnly))
        return;
    localFile.write(sdata);
    localFile.close();
    emit done(file_name);
}

void DownloadFile::slotDownload(const QString& url_path)
{
    target = url_path;
    QUrl url = QUrl::fromEncoded(this->target.toLocal8Bit());
    QNetworkRequest request(url);
    QObject::connect(manager.get(request), SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
}

void DownloadFile::downloadProgress(qint64 recieved, qint64 total)
{
    //qDebug() << recieved << total;
}


#ifndef DOWNLOADFILE_H
#define DOWNLOADFILE_H

#include <QObject>
#include <QFile>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QDir>

class DownloadFile : public QObject
{
    Q_OBJECT
public:
    explicit DownloadFile(QObject *parent = 0);
    void setTarget(const QString& t);

    static DownloadFile* GetInstance();
    void emitDownload(const QString& url_path);

private:
    QNetworkAccessManager manager;
    QString target;
    static DownloadFile* down_file;


signals:
    void done(const QString& file_name);
    void sigDownload(const QString& url_path);

public slots:
    void slotDownload(const QString& url_path);
    void downloadFinished(QNetworkReply* data);
    void downloadProgress(qint64 recieved, qint64 total);

};

#endif // DOWNLOADFILE_H

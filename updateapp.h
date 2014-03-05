#ifndef UPDATEAPP_H
#define UPDATEAPP_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QCoreApplication>
#include <QRegExp>
#include <QDebug>
#include <QThread>
#include <QStringList>
#include <QProcess>

#include "downloadfile.h"

#ifdef Q_OS_WIN
    #include <windows.h>
    #include <shellapi.h>
#endif

class UpdateApp : public QObject
{
    Q_OBJECT
public:
    explicit UpdateApp(QObject *parent = 0);

    QStringList ParseFile(const QString& path_file);
    bool CompareVersions(const QString& new_version);
    bool CheckFile(const QString& path_file);
    bool DownloadAnyFile(const QString& url_path);

private:
    static QRegExp reg_exp;
    static const QString url_inf_file;
    static const QString url_app_file;
    static const QString inf_file;
    static const QString inf_file_prefix;
    static const QString app_file_prefix;
    static const QString begin_url_new_file;
    static const QString APP_VERSION;

    QStringList new_version;

signals:
    void sigUpdateOrReject();
    void sigAcceptDownload();
    void sigRejectDownload();

public slots:
    void slotDoneLoad(const QString& file_name);
    bool slotCheckUpdate();
    void slotAcceptDownload();
    void slotRejectDownload();

};

#endif // UPDATEAPP_H

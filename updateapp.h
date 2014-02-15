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

class UpdateApp : public QObject
{
    Q_OBJECT
public:
    explicit UpdateApp(QObject *parent = 0);


    QStringList ParseFile(const QString& path_file);
    bool CompareVersions(QStringList old_version, QStringList new_version);
    bool CheckFile(const QString& path_file);
    bool DownloadAnyFile(const QString& url_path);
    bool RenameNewFile(const QString& name, const QString& new_name);
    bool DeleteOldFile(const QString& name);
    static void ChangeRequireUpd(bool upd);

    static bool require_upd;

private:
    static QRegExp reg_exp;
    static const QString url_inf_file;
    static const QString url_app_file;
    static const QString old_inf;
    static const QString new_inf;
    static const QString inf_file_prefix;
    static const QString app_file_prefix;
    static const QString begin_url_new_file;



    QStringList old_version;
    QStringList new_version;



signals:

public slots:
    void slotDoneLoad(const QString& file_name);
    bool slotCheckUpdate();

};

#endif // UPDATEAPP_H

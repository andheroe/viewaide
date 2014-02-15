#include "updateapp.h"

UpdateApp::UpdateApp(QObject *parent):
    QObject(parent)
{
}

QRegExp UpdateApp::reg_exp("(['vV']{1,1})([1-9]{1,1}[.][0-9]{1,1})");
const QString UpdateApp::url_inf_file = "http://viewaide.com/update.inf";
const QString UpdateApp::url_app_file = "http://viewaide.com/viewaide-win-setup.exe";
const QString UpdateApp::old_inf = "//update.inf";
const QString UpdateApp::new_inf = "//update_new.inf";
const QString UpdateApp::inf_file_prefix = "Update Version";
const QString UpdateApp::app_file_prefix = "Update File";
const QString UpdateApp::begin_url_new_file = "http:";

bool UpdateApp::require_upd = false;


bool UpdateApp::CheckFile(const QString& path_file)
{
    QFile file ( path_file );
    if ( !file.open(QIODevice::ReadOnly) )
        return false;
    return true;
}

bool UpdateApp::slotCheckUpdate()
{
    DownloadAnyFile(url_inf_file);
}

bool UpdateApp::CompareVersions(QStringList old_version, QStringList new_version)
{
    if ( old_version.at(0).toDouble() < new_version.at(0).toDouble() )
        return true;
    else
        return false;

}

bool UpdateApp::DownloadAnyFile(const QString& url_path)
{
    DownloadFile::GetInstance()->emitDownload(url_path);
}

bool UpdateApp::RenameNewFile(const QString &name, const QString& new_name)
{
    QFile(name).rename(new_name);
}

bool UpdateApp::DeleteOldFile(const QString &name)
{
    QFile(name).remove();
}

void UpdateApp::ChangeRequireUpd(bool upd)
{
    require_upd = upd;
}

void UpdateApp::slotDoneLoad(const QString& file_name)
{
    if ( file_name == "update_new.inf" )
    {
        QString path_old = QCoreApplication::applicationDirPath();
        path_old += old_inf;
        if ( CheckFile(path_old) )
            old_version = ParseFile(path_old);

        QString path_new = QCoreApplication::applicationDirPath();
        path_new += new_inf;
        if ( CheckFile(path_new) )
            new_version = ParseFile(path_new);

        if ( CompareVersions(old_version, new_version) )
        {


        }
        else
        {
            DownloadAnyFile(url_app_file);
            //QFile(path_new).setPermissions(QFile::ReadOwner | QFile::WriteOwner);
            //QFile(path_new).remove();
        }

    }
    else if ( file_name == "viewaide-win-setup_new.exe" )
    {
        QStringList args;
        args << "Update" << new_version.at(0) << new_version.at(1);
        QProcess::startDetached("Viewaide.exe", args);
        qApp->exit();
//        DeleteOldFile("Viewaide.exe");
//        DeleteOldFile("Update.inf");
//        RenameNewFile("Viewaide_new.exe","Viewaide.exe");
//        RenameNewFile("Update_new.inf","Update.inf");
    }


}

QStringList UpdateApp::ParseFile(const QString& path_file)
{

    reg_exp.setMinimal(true);

    QStringList parsed_file;
    QString str;

    QFile file ( path_file );
    file.open(QIODevice::ReadOnly);

    while ( !file.atEnd() )
    {
        str = file.readLine();
        if ( str.startsWith(inf_file_prefix) )
        {
            if ( reg_exp.indexIn(str) != -1 )
            {
               QString str_version = reg_exp.cap(2);
               parsed_file.push_back(str_version);
            }
        }
        else if ( str.startsWith(app_file_prefix))
        {
            int ind = str.indexOf(begin_url_new_file);
            QString str_url = str.mid(ind);
            parsed_file.push_back(str_url);
        }
    }
    file.close();

    return parsed_file;
}







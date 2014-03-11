#include "updateapp.h"

UpdateApp::UpdateApp(QObject *parent):
    QObject(parent)
{
}

QRegExp UpdateApp::reg_exp("(['vV']{1,1})([1-9]{1,1}[.][0-9]{1,1})");
const QString UpdateApp::url_inf_file = "http://viewaide.com/update.inf";
const QString UpdateApp::url_app_file = "http://viewaide.com/viewaide-win-setup.exe";
const QString UpdateApp::inf_file = "update.inf";
const QString UpdateApp::inf_file_prefix = "Update Version";
const QString UpdateApp::app_file_prefix = "Update App";
const QString UpdateApp::begin_url_new_file = "http:";
const QString UpdateApp::APP_VERSION = "1.0";

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

void UpdateApp::slotAcceptDownload()
{
    QString path = QCoreApplication::applicationDirPath();
    path += "/";
    path += inf_file;
    DownloadAnyFile(url_app_file);
    //QFile(path).remove();

}

void UpdateApp::slotRejectDownload()
{
    QString path = QCoreApplication::applicationDirPath();
    path += "/";
    path += inf_file;
    //QFile(path).remove();
}

bool UpdateApp::CompareVersions(const QString& new_version)
{
    if ( APP_VERSION.toDouble() < new_version.toDouble() )
        return true;
    else
        return false;
}

bool UpdateApp::DownloadAnyFile(const QString& url_path)
{
    DownloadFile::GetInstance()->emitDownload(url_path);
}

void UpdateApp::slotDoneLoad(const QString& file_name)
{

    if ( file_name == inf_file )
    {
        QString path = QCoreApplication::applicationDirPath();
        path += "/";
        path += inf_file;
        if ( CheckFile(path) )
            new_version = ParseFile(path);

        if ( CompareVersions(new_version.at(0)) )
            sigUpdateOrReject();
    }
    else if ( file_name == "viewaide-win-setup.exe" )
    {
        #ifdef Q_OS_WIN
            int result = (int)::ShellExecuteA(0, "open", file_name.toUtf8().constData(), 0, 0, SW_SHOWNORMAL);
            if (SE_ERR_ACCESSDENIED == result)
            {
                // Requesting elevation
                result = (int)::ShellExecuteA(0, "runas", file_name.toUtf8().constData(), 0, 0, SW_SHOWNORMAL);
            }
            if (result <= 32)
            {
                // error handling
            }
        #else
            if (!QProcess::startDetached(file_name))
            {
                // error handling
            }
        #endif
        qApp->exit();
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







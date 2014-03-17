#include "updateapp.h"

UpdateApp::UpdateApp(QObject *parent):
    QObject(parent)
{
}

QRegExp UpdateApp::reg_exp("(['vV']{1,1})([1-9]{1,1}[.][0-9]{1,1})");
const QString UpdateApp::url_inf_file = "http://viewaide.com/update.inf";
const QString UpdateApp::url_win_app_file = "http://viewaide.com/viewaide-win-setup.exe";
const QString UpdateApp::url_mac_app_file = "http://viewaide.com/viewaide-mac-setup.pkg";
const QString UpdateApp::inf_file = "update.inf";
const QString UpdateApp::inf_file_prefix = "Update Version";
const QString UpdateApp::app_win_file_prefix = "WIN";
const QString UpdateApp::app_mac_file_prefix = "MAC";
const QString UpdateApp::begin_url_new_file = "http:";
const QString UpdateApp::APP_VERSION = "1.1";

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
    QString path_to_file = QDir::homePath();
    path_to_file += "/Viewaide/";
    path_to_file += inf_file;
    #ifdef Q_OS_WIN
    DownloadAnyFile(url_win_app_file);
    #endif
    #ifdef Q_OS_MAC
    DownloadAnyFile(url_mac_app_file);
    #endif
    QFile(path_to_file).remove();
}

void UpdateApp::slotRejectDownload()
{
    QString path_to_file = QDir::homePath();
    path_to_file += "/Viewaide/";
    path_to_file += inf_file;
    QFile(path_to_file).remove();
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
    QString path_to_file = QDir::homePath();
    path_to_file += "/Viewaide/";
    path_to_file += file_name;

    if ( file_name == inf_file )
    {
        if ( CheckFile(path_to_file) )
            new_version = ParseFile(path_to_file);
        else
            return;
        if ( CompareVersions(new_version.at(0)) )
            sigUpdateOrReject();
    }

    #ifdef Q_OS_WIN
    else if ( file_name ==  url_win_app_file.section('/', -1) )
    #endif
    #ifdef Q_OS_MAC
    else if ( file_name ==  url_mac_app_file.section('/', -1) )
    #endif
    {
        #ifdef Q_OS_WIN
            int result = (int)::ShellExecuteA(0, "open", path_to_file.toUtf8().constData(), 0, 0, SW_SHOWNORMAL);
            if (SE_ERR_ACCESSDENIED == result)
            {
                // Requesting elevation
                result = (int)::ShellExecuteA(0, "runas", path_to_file.toUtf8().constData(), 0, 0, SW_SHOWNORMAL);
            }
            if (result <= 32)
            {
                // error handling
            }
        #else
            QString cmd = QString("open %1").arg(path_to_file);
            if (!QProcess::startDetached(cmd))
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
        #ifdef Q_OS_WIN
        else if ( str.startsWith(app_win_file_prefix))
        {
            int ind = str.indexOf(begin_url_new_file);
            QString str_url = str.mid(ind);
            parsed_file.push_back(str_url);
        }
        #endif
        #ifdef Q_OS_MAC
        else if ( str.startsWith(app_mac_file_prefix))
        {
            int ind = str.indexOf(begin_url_new_file);
            QString str_url = str.mid(ind);
            parsed_file.push_back(str_url);
        }
        #endif
    }
    file.close();
    return parsed_file;
}







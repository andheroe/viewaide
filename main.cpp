#include <QApplication>
#include <QProcess>

#include "main_window.h"
#include "camstream.h"
#include "algorithms.h"
#include "specialthread.h"
#include "updateapp.h"
#include "downloadfile.h"
#include "regandlogin.h"

using namespace std;

int main(int argc, char *argv[])
{
    QThread::msleep(1000);
    QApplication a(argc, argv);

    CamStream* stream=new CamStream();
    Main_window m_w ( stream );
    SpecialThread s_t ( &m_w );
    DownloadFile down_file;
    UpdateApp upd_app;
    RegAndLogIn reg_and_log;

    reg_and_log.show();

    //QPropertyAnimation::finished()
    //QObject::connect(m_w.anim_come_in, SIGNAL(finished()), m_w.ui_2->widget, SLOT(hide()));
    //QObject::connect(m_w.anim_come_in, SIGNAL(finished()), m_w.anim_come_in, SLOT(deleted()));
    //QObject::connect(m_w.anim_come_in, SIGNAL(finished()), &m_w, SLOT(InitNotifAnim()));

    QObject::connect(m_w.ui_2->cmb_webcam, SIGNAL(currentIndexChanged(int)), stream, SLOT(slotSaveSettings(int)) );
    QObject::connect(m_w.ui_2->cmb_lang, SIGNAL(currentIndexChanged(int)), stream, SLOT(slotSaveSettings(int)) );
    QObject::connect(m_w.ui_2->checkb_autorun, SIGNAL(stateChanged(int)), stream, SLOT(slotSaveSettings(int)));
    QObject::connect(m_w.ui_2->checkb_visual, SIGNAL(stateChanged(int)), stream, SLOT(slotSaveSettings(int)));
    QObject::connect(m_w.ui_2->checkb_voice, SIGNAL(stateChanged(int)), stream, SLOT(slotSaveSettings(int)));


    QObject::connect(m_w.ui_2->checkb_autorun, SIGNAL(toggled(bool)), stream, SLOT(slotAutoRun(bool)));

    QObject::connect(m_w.ui_2->btn_accept, SIGNAL(clicked()), &m_w, SLOT(slotTellAboutDownloading()));
    QObject::connect(m_w.ui_2->btn_accept, SIGNAL(clicked()), &s_t, SLOT(start()) );

    QObject::connect(m_w.ui_2->btn_accept, SIGNAL(clicked()), &upd_app, SLOT(slotAcceptDownload()));
    QObject::connect(m_w.ui_2->btn_notnow, SIGNAL(clicked()), &upd_app, SLOT(slotRejectDownload()));

   // QObject::connect(m_w.ui_2->btn_accept, SIGNAL(clicked()), &m_w, SLOT(slotNotifClose()));
    QObject::connect(m_w.ui_2->btn_notnow, SIGNAL(clicked()), &m_w, SLOT(slotNotifClose()));

    QObject::connect(m_w.ui_2->btn_notnow, SIGNAL(clicked(bool)), &m_w, SLOT(slotBlockPopupMsg(bool)));
    QObject::connect(m_w.ui_2->btn_accept, SIGNAL(clicked(bool)), &m_w, SLOT(slotBlockPopupMsg(bool)));

    QObject::connect(&upd_app, SIGNAL(sigUpdateOrReject()), &m_w, SLOT(slotDrawUpdWnd()), Qt::QueuedConnection);

    //QObject::connect(&upd_app, SIGNAL(sigRejectDownload()), &upd_app, SLOT(slotRejectDownload()));
    //QObject::connect(&upd_app, SIGNAL(sigAcceptDownload()), &upd_app, SLOT(slotAcceptDownload()));

    QObject::connect(m_w.ui_2->btn_check_update, SIGNAL(clicked()), &upd_app, SLOT(slotCheckUpdate()));


    QObject::connect(&down_file, SIGNAL(done(const QString&)), &upd_app, SLOT(slotDoneLoad(const QString&)));
    QObject::connect(stream, SIGNAL(sigCheckUpdate()), &upd_app, SLOT(slotCheckUpdate()));
    QObject::connect(DownloadFile::GetInstance(), SIGNAL(sigDownload(const QString&)), &down_file, SLOT(slotDownload(const QString&)) );


    QObject::connect(m_w.ui_2->cmb_webcam, SIGNAL(currentIndexChanged(int)), &m_w, SLOT(slotChangeCam(int)) );
    QObject::connect(m_w.ui_2->cmb_lang, SIGNAL(currentIndexChanged(int)), &m_w, SLOT(slotChangeLang(int)) );

    QObject::connect ( m_w.tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), &m_w, SLOT(slotPopupMenu())  );

    QObject::connect(stream, SIGNAL(UnlockCam()), &m_w, SLOT(slotUnlockCam()));
    QObject::connect(stream, SIGNAL(UnlockCam()), &s_t, SLOT(start()) );


    QObject::connect(stream, SIGNAL(BusyCam()), &m_w, SLOT(slotBusyCam()));
    QObject::connect(stream, SIGNAL(BusyCam()), &m_w, SLOT(slotNotifOpen()));

    QObject::connect(m_w.ui_2->btn_close2, SIGNAL(clicked()), m_w.ui_2->widg_options, SLOT(hide()));

    //QObject::connect(m_w.ui->btn_minimize, SIGNAL(clicked()), &m_w, SLOT(slotMinimizeWindow()));

//    QObject::connect(m_w.ui_2->btn_minimize_2, SIGNAL(clicked()), m_w.ui_2->widget_3, SLOT(showMinimized()));
//    QObject::connect(m_w.ui_2->btn_minimize, SIGNAL(clicked()), m_w.ui_2->widg_options, SLOT(showMinimized()));

    QObject::connect(m_w.ui->btn_close, SIGNAL(clicked()), &m_w, SLOT(slotCloseWindow()));
    QObject::connect(m_w.act_options, SIGNAL(triggered()), m_w.ui_2->widg_options, SLOT(show()));
    QObject::connect(m_w.act_exit, SIGNAL(triggered()), &m_w, SLOT(close()));

    QObject::connect(m_w.act_feedback, SIGNAL(triggered()), &m_w, SLOT(slotSendFeedback()));

    //QObject::connect(m_w.act_window, SIGNAL(triggered()), m_w.tray, SLOT(hide()) );
    QObject::connect(m_w.act_window, SIGNAL(triggered()), m_w.ui_2->widget_3, SLOT(show()));

    QObject::connect(m_w.act_pause, SIGNAL(triggered()), &m_w, SLOT(slotToStopOrResume()));

    QObject::connect(m_w.act_logout, SIGNAL(triggered()), &reg_and_log, SLOT(slotLogout()));
    QObject::connect(m_w.act_logout, SIGNAL(triggered()), m_w.ui_2->widget_3, SLOT(hide()));
    QObject::connect(m_w.act_logout, SIGNAL(triggered()), &reg_and_log, SLOT(slotRepeatConnect()));
    QObject::connect(m_w.act_logout, SIGNAL(triggered()), &reg_and_log, SLOT(show()));
    QObject::connect(m_w.act_logout, SIGNAL(triggered()), stream, SLOT(stop()));

    QObject::connect(m_w.ui_2->btn_close_2, SIGNAL(clicked()), m_w.ui_2->widget_3, SLOT(hide()));
    QObject::connect(m_w.ui_2->btn_close_2, SIGNAL(clicked()), &m_w, SLOT(slotToTray()));


//////////////CALIBRATION/////////////////////////////////////////////////////////////////////////

    QObject::connect(m_w.act_calibrate, SIGNAL(triggered()), &m_w, SIGNAL(sigRunGuiCalibrate()));
    //QObject::connect (m_w.act_calibrate, SIGNAL(triggered()), m_w.tray, SLOT(hide()) );

    QObject::connect(&m_w, SIGNAL(sigRunGuiCalibrate()), &m_w, SLOT(slotRunGuiCalibrate()));

    QObject::connect(&m_w, SIGNAL(sigAlertOff()), stream, SLOT(slotAlertOff()));
    QObject::connect(&m_w, SIGNAL(sigAlertOn()), stream, SLOT(slotAlertOn()));
    QObject::connect(&m_w, SIGNAL(sigShowMainWindow()), &m_w, SLOT(show()));
    QObject::connect(&m_w, SIGNAL(sigShowNextTuneup()), &m_w, SLOT(slotShowNextTuneup()));

    QObject::connect(&m_w, SIGNAL(sigSendCalibStage(calibration_stages)), stream, SLOT(ChangeCalibrationStage(calibration_stages)) );

    QObject::connect(&m_w, SIGNAL(sigAfterCalibrate()), &m_w, SLOT(hide()) );
    QObject::connect(&m_w, SIGNAL(sigAfterCalibrate()), m_w.ui_2->widget_2, SLOT(show()) );

    QObject::connect(&m_w, SIGNAL(sigAfterCalibrate()), stream, SLOT(pause()) );

/////////////////////////////////////////////////////////////////////////////////////////////////


    QObject::connect(m_w.ui_2->btn_start, SIGNAL(clicked()), m_w.ui_2->widget_2, SLOT(hide()) );
    QObject::connect(m_w.ui_2->btn_start, SIGNAL(clicked()), &m_w, SIGNAL(sigRunFirstTray()) );

    QObject::connect(m_w.ui_2->btn_start, SIGNAL(clicked()), stream, SLOT(resume()) );

    QObject::connect(&m_w, SIGNAL(sigRunFirstTray()), m_w.ui_2->widget, SLOT(show()) );
    QObject::connect(&m_w, SIGNAL(sigRunFirstTray()), &m_w, SLOT(slotNotifOpen()) );
    QObject::connect(&m_w, SIGNAL(sigRunFirstTray()), &s_t, SLOT(start()) );
    QObject::connect(&m_w, SIGNAL(sigRunFirstTray()), &m_w, SLOT(slotToTray()) );
    QObject::connect(&m_w, SIGNAL(sigRunFirstTray()), &m_w, SIGNAL(sigAlertOn()) );
    QObject::connect(&m_w, SIGNAL(sigRunFirstTray()), &m_w, SLOT(slotFirstMsg()) );
    QObject::connect(&m_w, SIGNAL(sigDrawAnim()), &m_w, SLOT(slotNotifClose()));


    QObject::connect(m_w.ui_2->btn_options, SIGNAL(clicked()), m_w.ui_2->widg_options, SLOT(show()) );

    QObject::connect(stream,SIGNAL(EyesAreTooLow()),&m_w,SLOT(TooLowAlert()),Qt::QueuedConnection );
    QObject::connect(stream,SIGNAL(EyesAreTooLow()), &s_t, SLOT(start()) );
    QObject::connect(stream,SIGNAL(EyesAreTooLow()), &m_w,SLOT(slotSetDefaultWnd()));


    QObject::connect(stream,SIGNAL(TooMuchSquint()),&m_w,SLOT(SquintAlert()),Qt::QueuedConnection );
    QObject::connect(stream,SIGNAL(TooMuchSquint()), &s_t, SLOT(start()) );
    QObject::connect(stream,SIGNAL(TooMuchSquint()), &m_w,SLOT(slotSetDefaultWnd()));


    QObject::connect(stream,SIGNAL(EyesAreTooNear()),&m_w,SLOT(TooNearAlert()),Qt::QueuedConnection );
    QObject::connect(stream,SIGNAL(EyesAreTooNear()), &s_t, SLOT(start()) );
    QObject::connect(stream,SIGNAL(EyesAreTooNear()), &m_w,SLOT(slotSetDefaultWnd()));

    QObject::connect(stream,SIGNAL(EyesAreTooHeigh()),&m_w,SLOT(TooHeighAlert()),Qt::QueuedConnection );
    QObject::connect(stream,SIGNAL(EyesAreTooHeigh()), &s_t, SLOT(start()) );
    QObject::connect(stream,SIGNAL(EyesAreTooHeigh()), &m_w,SLOT(slotSetDefaultWnd()));

    QObject::connect(stream,SIGNAL(TooFewBlink()),&m_w,SLOT(slotEyesBlink()),Qt::QueuedConnection );
    QObject::connect(stream,SIGNAL(TooFewBlink()), &s_t, SLOT(start()) );
    QObject::connect(stream,SIGNAL(TooFewBlink()), &m_w,SLOT(slotSetDefaultWnd()));

    QObject::connect(stream,SIGNAL(EyesAreTooHeigh()),&m_w,SLOT(TooHeighAlert()),Qt::QueuedConnection );
    QObject::connect(stream,SIGNAL(EyesAreTooHeigh()), &s_t, SLOT(start()) );
    QObject::connect(stream,SIGNAL(EyesAreTooHeigh()), &m_w,SLOT(slotSetDefaultWnd()));

    QObject::connect(stream,SIGNAL(BadLightness()),&m_w,SLOT(slotBadLightness()),Qt::QueuedConnection );
    QObject::connect(stream,SIGNAL(BadLightness()), &s_t, SLOT(start()) );
    QObject::connect(stream,SIGNAL(BadLightness()),&m_w,SLOT(slotSetDefaultWnd()));


    QObject::connect(&m_w,SIGNAL(sigRunFirstTray()),&m_w,SLOT(slotSetDefaultWnd()));
    QObject::connect(&m_w,SIGNAL(sigRunGuiCalibrate()),&m_w,SLOT(slotSetDefaultWnd()));

    QObject::connect(stream,SIGNAL(ImageIsReady(QImage)),&m_w,SLOT(ShowImage(QImage)),Qt::QueuedConnection);
    QObject::connect(stream, SIGNAL(CheckCalibration(bool)), &m_w, SLOT(slotReceiveInfo(bool)) );

    QObject::connect(&reg_and_log, SIGNAL(sigRunMainProgram()), &m_w, SLOT(slotWhatModeRun()) );
    QObject::connect(&reg_and_log, SIGNAL(sigRunMainProgram()), &m_w, SLOT(slotSetSettings()) );

    //for metrics
    QObject::connect(&reg_and_log, SIGNAL(signalSaveMetrics()), stream, SLOT(slotSaveMetrics()) );

    return a.exec();
}

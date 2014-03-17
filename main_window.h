#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QThread>
#include <QtMultimedia/QSound>
#include <QMenu>
#include <QPropertyAnimation>
#include <QTextEdit>
#include <QTranslator>
#include <QLocale>
#include <QEvent>
#include <QDebug>
#include <QShortcut>
#include <QDesktopServices>
#include <QWebFrame>
#include <QWheelEvent>
#include <QTime>
#include <QPainter>
#include <QDesktopWidget>
#include <QSysInfo>

#include "ui_main_window.h"
#include "ui_form.h"
#include "camstream.h"

namespace Ui {
class Main_window;
}

namespace Ui {
class Form;
}

class Main_window : public QMainWindow
{
    Q_OBJECT

public:
    Main_window( CamStream* str );
    ~Main_window();

    Ui::Main_window *ui;
    Ui::Form *ui_2;

    QSystemTrayIcon *tray;

    static int calibration_seconds;
    const static int notif_pause;

    QAction* act_window;
    QAction* act_pause;
    QAction* act_options;
    QAction* act_calibrate;
    QAction* act_feedback;
    QAction* act_logout;
    QAction* act_exit;

    QMenu* menu;

    static bool block_popup_msg;

    QPropertyAnimation* anim_come_in;

private:
    static const int sleep_time;

    QPropertyAnimation* anim_come_out;

    static bool cam_busy;

    QTime* calibration_timer;

    CamStream* stream;

    QSound* sound;

    QStringList list_of_cams;

    QTranslator* translator;

    QLabel lbl_show_circle;

    QPoint mLastMousePosition;
    bool mMoving;

    static int calibration_numb;



    static int numb_fails;
    static const int max_numb_fails;

    void changeTranslator ( QString );

    inline void SetRules();
    inline void InitAnim( int start_widg_pos, int end_widg_pos );
    inline void StartAnim();

    inline void ResetCalibration();

    inline void Calibration();


    inline void SetNotifGeom();

    inline bool CheckOptions();

    void CenterToScreen(QWidget* widget);

protected:
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

    void changeEvent(QEvent* event);


public slots:
    void slotMinimizeWindow();
    void slotCloseWindow();

    void slotShowNextTuneup();
    void slotReceiveInfo( bool checked );
    void slotToTray();

    void InitNotifAnim();

    void slotNotifOpen();
    void slotNotifClose();

    void slotMakePause();

    void slotRunGuiCalibrate();


    void slotChangeLang( int number );
    void slotChangeCam( int numb_of_cam );
    void slotUnlockCam();
    void slotBusyCam();

    void slotPopupMenu();

    void ShowImage(QImage qimg);

    void slotPause();
    void slotResume();

    void SquintAlert();
    void TooNearAlert();
    void TooLowAlert();
    void TooHeighAlert();
    void slotEyesBlink();
    void slotBadLightness();
    void slotFirstMsg();

    void slotDrawUpdWnd();
    void slotBlockPopupMsg(bool show_msg);
    void slotSetDefaultWnd();

    void slotWhatModeRun();

    void slotToStopOrResume();

    void slotTellAboutDownloading();

    void slotSetSettings();
    void slotSendFeedback();

    void slotHideNotifWidg();

signals:
    void sigSendCalibStage(calibration_stages stage);
    void sigAfterCalibrate();
    void sigDrawAnim();
    void sigRunFirstTray();
    void sigRunGuiCalibrate();

    void sigAlertOn();
    void sigAlertOff();
    void sigShowMainWindow();
    void sigShowNextTuneup();

};

#endif // MAIN_WINDOW_H

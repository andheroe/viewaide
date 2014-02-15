#include "main_window.h"

const int Main_window::sleep_time = 5000;
bool Main_window::cam_busy = false;

int Main_window::calibration_numb = 0;
int Main_window::numb_fails = 0;
int Main_window::calibration_seconds = 0;
const int Main_window::max_numb_fails = 10;
const int Main_window::notif_pause = 3000;

Main_window::Main_window(CamStream* str) : ui(new Ui::Main_window),
     ui_2(new Ui::Form), stream ( str )
{
    ui->setupUi(this);
    ui_2->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowTitle("Viewaide");

    ui_2->widg_options->setParent(0);
    ui_2->widget_2->setParent(0);
    ui_2->widget_3->setParent(0);

    ui_2->widg_options->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    ui_2->widget_2->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    ui_2->widget_3->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    tray = new QSystemTrayIcon( QIcon (":/res/logo.png"), this );

    menu = new QMenu(this);
    act_window = menu->addAction(tr("Show"));
    act_options = menu->addAction(tr("Options"));
    act_calibrate = menu->addAction(tr("Calibrate"));
    act_exit = menu->addAction(tr("Exit"));
    tray->setContextMenu(menu);

    ui->btn_minimize->setParent(ui->lbl_title);
    ui->btn_close->setParent(ui->lbl_title);

    ui->lbl_background->setParent(ui->lbl_video);
    ui->lbl_pbar->setParent(ui->lbl_video);
    ui->lbl_title->setParent(ui->lbl_video);

    list_of_cams = CamStream::GetCamsList();
    ui_2->cmb_webcam->addItems(list_of_cams);

    ui_2->label_7->setParent(ui_2->label);
    ui_2->btn_options->setParent(ui_2->label);


    ui_2->btn_close_2->setParent(ui_2->lbl_title);
    ui_2->lbl_title->setParent(ui_2->lbl_video);


    QDesktopWidget* desk = qApp->desktop();
    int desk_w = desk->width();
    int desk_h = desk->height();

    tray->setToolTip("Viewaide");

    ui_2->widg_options->setGeometry(desk_w / 2 - ui_2->widg_options->width() / 2, desk_h / 2 - ui_2->widg_options->height() / 2, ui_2->widg_options->width(), ui_2->widg_options->height());

    ui_2->widget_2->setGeometry(desk_w / 2 - ui_2->widget_2->width() / 2, desk_h / 2 - ui_2->widget_2->height() / 2, ui_2->widget_2->width(), ui_2->widget_2->height());

    ui_2->widget_3->setGeometry(desk_w / 2 - ui_2->widget_3->width() / 2, desk_h / 2 - ui_2->widget_3->height() / 2, ui_2->widget_3->width(), ui_2->widget_3->height());

    calibration_timer = new QTime;

    InitNotifAnim();
    SetNotifGeom();

    sound = new QSound ( ":/res/sound.wav", this );

}

Main_window::~Main_window()
{
    stream->stop();
    stream->wait();
    delete stream;
    delete ui;
    delete ui_2;
    delete sound;

}

void Main_window::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        mMoving = true;
        mLastMousePosition = event->pos();
    }
}

void Main_window::mouseMoveEvent(QMouseEvent* event)
{
    if( event->buttons().testFlag(Qt::LeftButton) && mMoving)
        this->move(this->pos() + (event->pos() - mLastMousePosition));
}

void Main_window::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
        mMoving = false;
}

void Main_window::changeTranslator(QString postfix)
{
    QApplication::removeTranslator(translator);
    translator = new QTranslator(this);
    translator->load( this->windowTitle() + "_" + postfix, ":/res" );
    QApplication::installTranslator(translator);
}


void Main_window::slotRunGuiCalibrate()
{
    ui_2->label_7->setText(tr("Notifications\nwill be here"));
    emit sigAlertOff();
    emit sigShowNextTuneup();
    emit sigShowMainWindow();
}

void Main_window::slotReceiveInfo( bool checked )
{
    if ( checked )
    {
        if ( calibration_timer->isNull() )
            calibration_timer->start();
    }
    else
    {
        if ( numb_fails < max_numb_fails && (!calibration_timer->isNull()) )
            ++numb_fails;
        else if ( numb_fails >= max_numb_fails && (!calibration_timer->isNull()) )
        {
            numb_fails = 0;
            delete calibration_timer;
            calibration_timer = new QTime;
            calibration_seconds = 0;
            ui->lbl_count->repaint();
        }
    }

    if ( !calibration_timer->isNull() )
    {
        if ( calibration_timer->elapsed() >= 1000 && calibration_timer->elapsed() < 2000 )
            calibration_seconds = 3;
        else if ( calibration_timer->elapsed() >= 2000 && calibration_timer->elapsed() < 3000 )
            calibration_seconds = 2;
        else if ( calibration_timer->elapsed() >= 3000 && calibration_timer->elapsed() < 4000 )
            calibration_seconds = 1;
        else if ( calibration_timer->elapsed() >= 4000 )
        {
            numb_fails = 0;
            calibration_seconds = 0;
            delete calibration_timer;
            calibration_timer = new QTime;

            slotShowNextTuneup();
        }
        ui->lbl_count->repaint();
    }
}

void Main_window::SetRules()
{
    if ( calibration_numb == 1 )
    {
        ui->lbl_rules->setText(tr("Put your face into the rectangle"));
        ui->lbl_pbar->setStyleSheet("border-image: url(:/res/pbar1.png)");
        emit sigSendCalibStage(STAGE1);
    }
    else if ( calibration_numb == 2 )
    {
        ui_2->lbl_rules2->setText(tr("Make sure that your eyes are highlighted"));
        ui->lbl_pbar->setStyleSheet("border-image: url(:/res/pbar2.png)");
        emit sigSendCalibStage(STAGE2);
    }
    else if ( calibration_numb == 3 )
    {
        ui->lbl_rules->setText(tr("Check the distance to the monitor"));
        ui->lbl_pbar->setStyleSheet("border-image: url(:/res/pbar3.png)");
        emit sigSendCalibStage(STAGE3);
    }
}

void Main_window::slotShowNextTuneup()
{
    ++calibration_numb;
    if ( calibration_numb <= 3 )
    {
        SetRules();
        InitAnim ( 1600, -900 );
        StartAnim();
    }
    else
    {
        emit sigAlertOn();
        ResetCalibration();
        emit sigAfterCalibrate();
    }

    if ( calibration_numb == 3 )
    {
        InitNotifAnim();
        SetNotifGeom();
        Calibration();
    }
}

void Main_window::InitAnim( int start_widg_pos, int end_widg_pos )
{
    if ( calibration_numb % 2 == 0 )
    {
        ui_2->lbl_rules2->setGeometry(start_widg_pos,360,640,90);

        anim_come_out = new QPropertyAnimation ( ui->lbl_rules, "geometry" );
        anim_come_in = new QPropertyAnimation ( ui_2->lbl_rules2, "geometry" );
    }
    else
    {
        ui->lbl_rules->setGeometry(start_widg_pos,360,640,90);

        anim_come_out = new QPropertyAnimation ( ui_2->lbl_rules2, "geometry" );
        anim_come_in = new QPropertyAnimation ( ui->lbl_rules, "geometry" );
    }

    anim_come_out->setDuration(1500);
    anim_come_out->setEasingCurve(QEasingCurve::OutQuad);
    anim_come_out->setEndValue(QRectF(end_widg_pos, 360, 640,90));

    anim_come_in->setDuration(1500);
    anim_come_in->setEasingCurve(QEasingCurve::OutQuad);
    anim_come_in->setEndValue(QRectF(0, 360, 640,90));
}

void Main_window::StartAnim()
{
    anim_come_out->start();
    anim_come_in->start();
}

void Main_window::changeEvent(QEvent * event)
{

    if ( event->type() == QEvent::LanguageChange )
    {
        act_window->setText(tr("Show"));
        act_options->setText(tr("Options"));
        act_calibrate->setText(tr("Calibrate"));
        act_exit->setText(tr("Exit"));
        ui_2->lbl_language->setText(tr("Language"));
        ui_2->lbl_notif->setText(tr("Notifications"));
        ui_2->lbl_webcam->setText(tr("Web-cam"));
        ui_2->checkb_visual->setText(tr("Visual"));
        ui_2->checkb_voice->setText(tr("Sound"));
        ui_2->label_3->setText(tr("Setup is complete"));
        ui_2->btn_start->setText(tr("Launch"));

    }
    else
        QMainWindow::changeEvent(event);
}

void Main_window::slotChangeCam(int numb_of_cam )
{
    stream->ChangeCamIndex(numb_of_cam);
}

void Main_window::slotBusyCam()
{
    ui->lbl_video->setText(tr("Web-cam is busy"));
    ui_2->lbl_video->setText(tr("Web-cam is busy"));
    ui_2->label_7->setText(tr("Web-cam is busy"));

}

void Main_window::slotPopupMenu()
{
    menu->activateWindow();
    menu->popup(QCursor::pos());
}

void Main_window::slotUnlockCam()
{
    ui->lbl_video->setText("");
    ui_2->lbl_video->setText("");
    ui_2->label_7->setText(tr("Notifications\nwill be here"));
    cam_busy = false;
}

void Main_window::slotChangeLang(int number)
{
    if ( number == 0 )
        changeTranslator("eng");
    else if ( number == 1 )
        changeTranslator("ru");
}

void Main_window::ShowImage(QImage qimg)
{
    QPixmap pxmap ( QPixmap::fromImage(qimg) );
    ui->lbl_video->setPixmap(pxmap);
    ui_2->lbl_video->setPixmap(pxmap);
}

void Main_window::slotPause()
{
    stream->pause();
}

void Main_window::slotResume()
{
    stream->resume();
}

void Main_window::Calibration()
{
    stream->SetCalibrationMode(sleep_time);
}

void Main_window::slotToTray()
{
    tray->show();
}

void Main_window::ResetCalibration()
{
    calibration_numb = 0;
    numb_fails = 0;
    calibration_seconds = 0;
}

void Main_window::InitNotifAnim()
{
    anim_come_out = new QPropertyAnimation ( ui_2->widget, "geometry" );
    anim_come_out->setDuration(1500);
    anim_come_out->setEasingCurve(QEasingCurve::OutQuad);
    anim_come_out->setEndValue(QRectF(qApp->desktop()->width()-250, qApp->desktop()->height() - qApp->desktop()->height()*0.8, 250, 120));

    anim_come_in = new QPropertyAnimation ( ui_2->widget, "geometry" );
    anim_come_in->setDuration(1500);
    anim_come_in->setEasingCurve(QEasingCurve::OutQuad);
    anim_come_in->setEndValue(QRectF(qApp->desktop()->width()+1, qApp->desktop()->height() - qApp->desktop()->height()*0.8, 250, 120));
}

void Main_window::SetNotifGeom()
{
    QDesktopWidget* desk = qApp->desktop();
    int desk_w = desk->width();
    int desk_h = desk->height();

    ui_2->widget->setGeometry(desk_w+1, desk_h - desk_h * 0.8, 250, 120);
}

void Main_window::slotNotifOpen()
{
    anim_come_out->start();
}

void Main_window::slotNotifClose()
{
    anim_come_in->start();
    InitNotifAnim();
}

void Main_window::slotMakePause()
{
    QThread::msleep(notif_pause);
    emit sigDrawAnim();
}

void Main_window::slotEyesBlink()
{
    if ( ui_2->checkb_visual->isChecked() )
    {
        ui_2->label_7->setText(tr("Attention!\nBlink more"));
        ui_2->widget->show();
        slotNotifOpen();
    }

    if ( ui_2->checkb_voice->isChecked() )
    {
        sound->setLoops(1);
        if ( sound->isFinished() )
            sound->play();
    }
}

void Main_window::slotBadLightness()
{
    if ( ui_2->checkb_visual->isChecked() )
    {
        ui_2->label_7->setText(tr("Attention!\nBad lighting"));
        ui_2->widget->show();
        slotNotifOpen();
    }

    if ( ui_2->checkb_voice->isChecked() )
    {
        sound->setLoops(1);
        if ( sound->isFinished() )
            sound->play();
    }
}

void Main_window::slotFirstMsg()
{
    tray->showMessage( tr("Viewaide"), tr("Successfully launched!"), QSystemTrayIcon::Information, 1000);
}

void Main_window::SquintAlert()
{
    if ( ui_2->checkb_visual->isChecked() )
    {
        ui_2->label_7->setText(tr("Attention!\nOften squint"));
        ui_2->widget->show();
        slotNotifOpen();
    }

    if ( ui_2->checkb_voice->isChecked() )
    {
        sound->setLoops(1);
        if ( sound->isFinished() )
            sound->play();
    }
}

void Main_window::TooNearAlert()
{
    if ( ui_2->checkb_visual->isChecked() )
    {
        ui_2->label_7->setText(tr("Attention!\nToo close"));
        ui_2->widget->show();
        slotNotifOpen();
    }

    if ( ui_2->checkb_voice->isChecked() )
    {
        sound->setLoops(1);
        if ( sound->isFinished() )
            sound->play();
    }
}

void Main_window::TooLowAlert()
{
    if ( ui_2->checkb_visual->isChecked() )
    {
        ui_2->label_7->setText(tr("Attention!\nToo low, sit up"));
        ui_2->widget->show();
        slotNotifOpen();
    }

    if ( ui_2->checkb_voice->isChecked() )
    {
        sound->setLoops(1);
        if ( sound->isFinished() )
            sound->play();
    }

}
void Main_window::TooHeighAlert()
{
    if ( ui_2->checkb_visual->isChecked() )
    {
        ui_2->label_7->setText(tr("Attention!\nToo high, sit lower"));
        ui_2->widget->show();
        slotNotifOpen();
    }

    if ( ui_2->checkb_voice->isChecked() )
    {
        sound->setLoops(1);
        if ( sound->isFinished() )
            sound->play();
    }
}


void Main_window::slotMinimizeWindow()
{
    this->showMinimized();
}

void Main_window::slotCloseWindow()
{
    this->close();
    qApp->exit();
}



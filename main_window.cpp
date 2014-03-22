#include "main_window.h"

const int Main_window::sleep_time = 5000;
bool Main_window::cam_busy = false;

int Main_window::calibration_numb = 0;
int Main_window::numb_fails = 0;
int Main_window::calibration_seconds = 0;
bool Main_window::block_popup_msg = false;
const int Main_window::max_numb_fails = 10;
const int Main_window::notif_pause = 3000;


Main_window::Main_window(CamStream* str) : ui(new Ui::Main_window),
     ui_2(new Ui::Form), stream ( str )
{
    ui->setupUi(this);
    ui_2->setupUi(this);

    ui_2->btn_accept->hide();
    ui_2->btn_notnow->hide();

    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowTitle("Viewaide");

    ui_2->widg_options->setParent(0);
    ui_2->widget_2->setParent(0);
    ui_2->widget_3->setParent(0);

    ui_2->widg_options->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    ui_2->widget_2->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    ui_2->widget_3->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    tray = new QSystemTrayIcon( QIcon (":/res/Logo_only.png"), this );

    menu = new QMenu(this);
    act_window = menu->addAction(tr("Show"));
    act_pause = menu->addAction(tr("Pause"));
    act_options = menu->addAction(tr("Options"));
    act_calibrate = menu->addAction(tr("Calibrate"));
    act_feedback = menu->addAction(tr("Send feedback"));
    act_logout = menu->addAction(tr("Logout"));
    act_exit = menu->addAction(tr("Exit"));
    tray->setContextMenu(menu);

    //ui->btn_minimize->setParent(ui->lbl_title);
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

    tray->setToolTip("Viewaide");

    #ifdef Q_OS_MAC
    ui->btn_close->move(ui->lbl_title->pos());
    ui_2->btn_close_2->move(ui_2->lbl_title->pos());
    ui_2->btn_close2->move(ui_2->label_6->pos());
    #endif

//    QDesktopWidget* desk = qApp->desktop();
//    int desk_w = desk->width();
//    int desk_h = desk->height();

//    ui_2->widg_options->setGeometry(desk_w / 2 - ui_2->widg_options->width() / 2, desk_h / 2 - ui_2->widg_options->height() / 2, ui_2->widg_options->width(), ui_2->widg_options->height());

//    ui_2->widget_2->setGeometry(desk_w / 2 - ui_2->widget_2->width() / 2, desk_h / 2 - ui_2->widget_2->height() / 2, ui_2->widget_2->width(), ui_2->widget_2->height());

//    ui_2->widget_3->setGeometry(desk_w / 2 - ui_2->widget_3->width() / 2, desk_h / 2 - ui_2->widget_3->height() / 2, ui_2->widget_3->width(), ui_2->widget_3->height());

    CenterToScreen(ui_2->widg_options);
    CenterToScreen(ui_2->widget_2);
    CenterToScreen(ui_2->widget_3);

    calibration_timer = new QTime;

    InitNotifAnim();
    SetNotifGeom();
    sound = new QSound ( ":/res/sound.wav", this );

    stream->log<<"Main window created object\n";
}

Main_window::~Main_window()
{
    stream->stop();
    stream->wait();
    delete stream;

    //for metrics
    if(save_metrics)
    {
        QString metrics;

        QString path_to_file = QDir::homePath();
        path_to_file += "/Viewaide/";
        path_to_file += "account.txt";
        QFile file(path_to_file);
        file.open(QIODevice::ReadWrite | QIODevice::Text);
        QTextStream file_stream;
        file_stream.setDevice(&file);
        QString login=file_stream.readLine();

        metrics=login+"\r\n";

        file.close();

        QSysInfo info;
        #ifdef Q_OS_WIN
        switch(info.windowsVersion())
        {
            case QSysInfo::WV_WINDOWS8: metrics+="Windows 8\r\n";break;
            case QSysInfo::WV_WINDOWS7: metrics+="Windows 7\r\n";break;
            case QSysInfo::WV_VISTA: metrics+="Windows Vista\r\n";break;
            case QSysInfo::WV_XP :metrics+="Windows XP\r\n";break;
            default: metrics+="Windows ??\n";break;
        }
        #endif

        #ifdef Q_OS_MAC
        switch(info.macVersion())
        {
            case QSysInfo::MV_10_5: metrics+="OS X 10.5\r\n";break;
            case QSysInfo::MV_10_6: metrics+="OS X 10.6\r\n";break;
            case QSysInfo::MV_10_7: metrics+="OS X 10.7\r\n";break;
            case QSysInfo::MV_10_8: metrics+="OS X 10.8\r\n";break;
            case QSysInfo::MV_10_9: metrics+="OS X 10.9\r\n";break;
            default: metrics+="OS X ??\r\n";break;
        }
        #endif

        path_to_file = QDir::homePath();
        path_to_file += "/Viewaide/";
        path_to_file += "log.txt";
        QFile file1(path_to_file);
        file1.open(QIODevice::ReadWrite | QIODevice::Text);
        QTextStream file_stream1;
        file_stream1.setDevice(&file1);

        metrics+=file_stream1.readLine()+"\r\n";
        metrics+=file_stream1.readAll();
        metrics+="\n[log]////////////\n\n";

        file1.close();

        path_to_file = QDir::homePath();
        path_to_file += "/Viewaide/";
        path_to_file += "options.txt";
        QFile file2(path_to_file);
        file2.open(QIODevice::ReadWrite | QIODevice::Text);
        QTextStream file_stream2;
        file_stream2.setDevice(&file2);

        metrics+=file_stream2.readAll();
        metrics+="\n[options]////////////\n";

        file2.close();

        path_to_file = QDir::homePath();
        path_to_file += "/Viewaide/";
        path_to_file += "settings.ini";
        QFile file3(path_to_file);
        file3.open(QIODevice::ReadWrite | QIODevice::Text);
        QTextStream file_stream3;
        file_stream3.setDevice(&file3);

        metrics+=file_stream3.readAll();
        metrics+="\n[settings]////////////\n";

        file3.close();

        //qDebug()<<metrics;
        ConnectWithServer obj;
        if(obj.netIsWorking())
            obj.uploadAllData(metrics,"http://viewaide.com/test.php","str");
    }
    delete ui;
    delete ui_2;
    delete sound;

}

void Main_window::CenterToScreen(QWidget* widget)
{
    if ( !widget )
        return;
    QDesktopWidget* m = QApplication::desktop();
    QRect desk_rect = m->screenGeometry(m->screenNumber(QCursor::pos()));
    int desk_x = desk_rect.width();
    int desk_y = desk_rect.height();
    int x = widget->width();
    int y = widget->height();
    widget->move(desk_x / 2 - x / 2 + desk_rect.left(), desk_y / 2 - y / 2 + desk_rect.top());
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
    slotSetDefaultWnd();
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
        stream->log<<"Calibration STAGE1\n";
        ui->lbl_rules->setText(tr("Put your face into the rectangle"));
        ui->lbl_pbar->setStyleSheet("border-image: url(:/res/pbar1.png)");
        emit sigSendCalibStage(STAGE1);
    }
    else if ( calibration_numb == 2 )
    {
        stream->log<<"Calibration STAGE2\n";
        ui_2->lbl_rules2->setText(tr("Make sure that your eyes are highlighted"));
        ui->lbl_pbar->setStyleSheet("border-image: url(:/res/pbar2.png)");
        emit sigSendCalibStage(STAGE2);
    }
    else if ( calibration_numb == 3 )
    {
        stream->log<<"Calibration STAGE3\n";
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

        if ( ( act_pause->text() == "Resume" ) || ( act_pause->text() == "Возобновить" ) )
            act_pause->setText(tr("Resume"));
        if ( ( act_pause->text() == "Pause" ) || ( act_pause->text() == "Пауза" ) )
            act_pause->setText(tr("Pause"));

        act_options->setText(tr("Options"));
        act_calibrate->setText(tr("Calibrate"));
        act_feedback->setText(tr("Send feedback"));
        act_logout->setText(tr("Logout"));
        act_exit->setText(tr("Exit"));
        ui_2->lbl_language->setText(tr("Language"));
        ui_2->lbl_notif->setText(tr("Notifications"));
        ui_2->lbl_webcam->setText(tr("Web-cam"));
        ui_2->checkb_visual->setText(tr("Visual"));
        ui_2->checkb_voice->setText(tr("Sound"));
        ui_2->label_3->setText(tr("Setup is complete"));
        ui_2->btn_start->setText(tr("Launch"));
        ui_2->btn_accept->setText(tr("Update"));
        ui_2->btn_notnow->setText(tr("Later"));
        ui_2->btn_check_update->setText(tr("Check update"));
        ui_2->checkb_autorun->setText(tr("Autorun"));
//        ui_3->checkBox->setText(tr("I agree with"));
//        ui_3->lineEdit->setPlaceholderText(tr("Your e-mail"));
//        ui_3->lineEdit_2->setPlaceholderText(tr("Password"));
//        ui_3->lineEdit_3->setPlaceholderText(tr("First name"));
//        ui_3->lineEdit_4->setPlaceholderText(tr("Last name"));
//        ui_3->lineEdit_5->setPlaceholderText(tr("Your e-mail"));
//        ui_3->lineEdit_6->setPlaceholderText(tr("Password"));
//        ui_3->pushButton->setText(tr("Log in"));
//        ui_3->pushButton_2->setText(tr("Registration"));
//        ui_3->pushButton_3->setText(tr("Can not log in?"));
//        ui_3->pushButton_4->setText(tr("Log in"));
//        ui_3->pushButton_5->setText(tr("Registration"));
//        ui_3->pushButton_6->setText(tr("Help with registration"));
//        ui_3->pushButton_7->setText(tr("Retry"));
//        ui_3->pushButton_8->setText(tr("Terms"));

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
    slotSetDefaultWnd();
    if ( !block_popup_msg )
        ui_2->label_7->setText(tr("Web-cam is busy"));
    ui->lbl_video->setText(tr("Web-cam is busy"));
    ui_2->lbl_video->setText(tr("Web-cam is busy"));
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
    slotSetDefaultWnd();
    if ( !block_popup_msg )
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
    QDesktopWidget* m = QApplication::desktop();
    QRect desk_rect = m->screenGeometry(m->screenNumber(QCursor::pos()));

    anim_come_out = new QPropertyAnimation ( ui_2->widget, "geometry" );
    anim_come_out->setDuration(1500);
    anim_come_out->setEasingCurve(QEasingCurve::OutQuad);
    anim_come_out->setEndValue(QRectF(desk_rect.width()-250, desk_rect.height() - desk_rect.height()*0.8, 250, 120));

    anim_come_in = new QPropertyAnimation ( ui_2->widget, "geometry" );
    anim_come_in->setDuration(1500);
    anim_come_in->setEasingCurve(QEasingCurve::OutQuad);
    anim_come_in->setEndValue(QRectF(desk_rect.width()+1, desk_rect.height() - desk_rect.height()*0.8, 250, 120));

    connect(this->anim_come_in, SIGNAL(finished()), ui_2->widget, SLOT(hide()));
    connect(this->anim_come_in, SIGNAL(finished()), this, SLOT(InitNotifAnim()));
}

void Main_window::SetNotifGeom()
{
    QDesktopWidget* m = QApplication::desktop();
    QRect desk_rect = m->screenGeometry(m->screenNumber(QCursor::pos()));

    int desk_x = desk_rect.width();
    int desk_y = desk_rect.height();
    int x = ui_2->widget->width();
    int y = ui_2->widget->height();
    ui_2->widget->move(desk_x + 1, desk_y - desk_y * 0.8);

//    QDesktopWidget* desk = qApp->desktop();
//    int desk_w = desk->width();
//    int desk_h = desk->height();

//    ui_2->widget->setGeometry(desk_w+1, desk_h - desk_h * 0.8, 250, 120);
}

void Main_window::slotNotifOpen()
{
    CamStream::is_popup_showed = true;
    anim_come_out->start();    
}

void Main_window::slotNotifClose()
{
    anim_come_in->start();

    CamStream::is_popup_showed = false;

    ui_2->btn_accept->hide();
    ui_2->btn_notnow->hide();
    //InitNotifAnim();

}

void Main_window::slotMakePause()
{
    QThread::msleep(notif_pause);
    emit sigDrawAnim();
}

void Main_window::slotEyesBlink()
{
    if ( !block_popup_msg )
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
}

void Main_window::slotBadLightness()
{
    if ( !block_popup_msg )
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
}

void Main_window::slotFirstMsg()
{
    ui_2->label_7->setText(tr("Notifications\nwill be here"));
    tray->showMessage( tr("Viewaide"), tr("Successfully launched!"), QSystemTrayIcon::Information, 1000);
}

void Main_window::slotDrawUpdWnd()
{
    slotBlockPopupMsg(true);
    ui_2->btn_accept->show();
    ui_2->btn_notnow->show();
    ui_2->label_7->setGeometry(0,5,250,61);
    ui_2->label_7->setText(tr("The new version is available\nUpgrade now?"));
    ui_2->btn_options->hide();
    ui_2->label_7->setStyleSheet("font-size: 16px;"
                                "font-family: Verdana;"
                                "text-align: center;"
                                "color: white;"
                                "background-color: rgb(204, 85, 0, 0);");


    ui_2->label->setStyleSheet("font-size: 16px;"
                               "font-family: Verdana;"
                               "text-align: center;"
                               "color: white;"
                               "background-color: rgb(204, 85, 0, 200);");
    ui_2->widget->show();
    slotNotifOpen();
}

void Main_window::slotBlockPopupMsg(bool show_msg)
{
    block_popup_msg = show_msg;
}

void Main_window::slotSetDefaultWnd()
{
    if ( !block_popup_msg )
    {
        ui_2->btn_options->show();
        ui_2->label_7->setGeometry(0,10,250,61);
        ui_2->label_7->setStyleSheet("font-size: 20px;"
                                    "font-family: Verdana;"
                                    "text-align: center;"
                                    "color: white;"
                                    "background-color: rgb(74, 98, 101,0);");


        ui_2->label->setStyleSheet("font-size: 20px;"
                                   "font-family: Verdana;"
                                   "text-align: center;"
                                   "color: white;"
                                   "background-color: rgb(74, 98, 101,200);");      
    }
}

void Main_window::slotWhatModeRun()
{
    if ( CheckOptions() )
        this->sigRunFirstTray();
    else
        this->sigRunGuiCalibrate();
    stream->start();
}

void Main_window::slotToStopOrResume()
{
    if ( ( menu->actions().at(1)->text() == "Pause" ) || ( menu->actions().at(1)->text() == "Пауза" ) )
    {
        stream->stop();
        stream->wait();
        if ( menu->actions().at(1)->text() == "Pause" )
            menu->actions().at(1)->setText("Resume");
        if ( menu->actions().at(1)->text() == "Пауза" )
            menu->actions().at(1)->setText("Возобновить");
        this->slotBusyCam();
    }
    else if ( ( menu->actions().at(1)->text() == "Resume" ) || ( menu->actions().at(1)->text() == "Возобновить" ) )
    {
        stream->start();
        if ( menu->actions().at(1)->text() == "Resume" )
            menu->actions().at(1)->setText("Pause");
        if ( menu->actions().at(1)->text() == "Возобновить" )
            menu->actions().at(1)->setText("Пауза");
    }
}

void Main_window::slotTellAboutDownloading()
{
    slotBlockPopupMsg(false);
    ui_2->btn_accept->hide();
    ui_2->btn_notnow->hide();
    ui_2->label_7->setGeometry(0,10,250,61);
    ui_2->label_7->setText("Downloading...\nPlease, wait");
}

void Main_window::slotSetSettings()
{
    QString path_to_file = QDir::homePath();
    path_to_file += "/Viewaide/";
    path_to_file += "settings.ini";
    QFile file(path_to_file);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString line = file.readAll();
    file.close();

    QStringList list = line.split(" ", QString::SkipEmptyParts);

    while ( !list.empty() )
    {
        QString str = list.first();
        list.pop_front();
        QString rez = list.first();
        list.pop_front();
        if ( str.startsWith("cmb_lang") )
            ui_2->cmb_lang->setCurrentIndex(rez.toInt());
        else if ( str.startsWith("cmb_webcam") )
            ui_2->cmb_webcam->setCurrentIndex(rez.toInt());
        else if ( str.startsWith("checkb_visual") )
            ui_2->checkb_visual->setChecked(rez.toInt());
        else if ( str.startsWith("checkb_voice") )
            ui_2->checkb_voice->setChecked(rez.toInt());
        else if ( str.startsWith("checkb_autorun") )
            ui_2->checkb_autorun->setChecked(rez.toInt());
    }
}

void Main_window::slotSendFeedback()
{
    QDesktopServices::openUrl(QUrl("http://viewaide.com/#contacts"));
}

void Main_window::slotHideNotifWidg()
{
    ui_2->widget->hide();
}

bool Main_window::CheckOptions()
{
    QString path_to_file = QDir::homePath();
    path_to_file += "/Viewaide/";
    path_to_file += "options.txt";
    QFile file ( path_to_file );
    if ( !file.open(QIODevice::ReadOnly) )
        return 0;
    return 1;
}

void Main_window::SquintAlert()
{
    if ( !block_popup_msg )
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
}

void Main_window::TooNearAlert()
{
    if ( !block_popup_msg )
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
}

void Main_window::TooLowAlert()
{
    if ( !block_popup_msg )
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

}
void Main_window::TooHeighAlert()
{
    if ( !block_popup_msg )
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

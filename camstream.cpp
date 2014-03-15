#include "camstream.h"

QFile CamStream::log_file;
QTextStream CamStream::log;
bool CamStream::is_popup_showed = true;

bool save_metrics=false;

CamStream::CamStream()
{
    capture=0;

    // system bool
    stop_thread=false;
    pause_thread=false;
    thread_is_paused=false;
    calibration_mode_on=false;


    squint_time_set=false;
    too_near_time_set=false;
    too_heigh_time_set=false;
    too_low_time_set=false;
    bad_lighting_time_set=false;
    alert_on=true;
    eyes_are_closed=false;

    face_refreshed=false;

    // alert cache
    alert_cache_size=6;
    alert_cache_squint=new int[alert_cache_size];
    for(int i=0;i<alert_cache_size;++i)
        alert_cache_squint[i]=0;
    alert_cache_blink=new int[alert_cache_size];
    for(int i=0;i<alert_cache_size;++i)
        alert_cache_blink[i]=2;

    alert_iteration_time_msecs=10000;
    alert_iteration_count=0;
    alert_iterations_for_statistics=2;

    squint_iteration_count=0;
    blink_iteration_count=0;

    dist_sum=0;
    dist_sum_count=0;
    blink_sum=0;
    blink_sum_count=0;

    alert_params.bad_monitor_distance=false;
    alert_params.few_blink=false;
    alert_params.eyes_are_squinted=false;
    alert_params.too_height_angle=false;
    alert_params.too_low_angle=false;
    alert_params.bad_lightness=false;

    alert_activ_count.bad_monitor_distance=0;
    alert_activ_count.few_blink=0;
    alert_activ_count.eyes_are_squinted=0;
    alert_activ_count.too_height_angle=0;
    alert_activ_count.too_low_angle=0;
    alert_activ_count.bad_lightness=0;

    // constants
    normal_squint_per_min=3;
    normal_blink_per_min=3;

    squint_time_msecs=3000;
    too_near_time_msecs=3000;
    too_heigh_time_msecs=3000;
    too_low_time_msecs=3000;
    bad_lighting_msecs=30000;

    min_iterations=5;

    iteration_error=5;
    iteration_error_count=0;

    cur_time_set=false;
    notification_time=3000;

    calibration_long=0;
    calibration_count=0;
    calibration_dist_sum=0;
    calibration_height_sum=0;
    calibration_centers_dist_sum=0;
    calibration_eyes_height_sum=0;

    calib_stage=STAGE1;

    min_dist_to_monitor_in_cm=40;

    options_cur.max_eyes_height=1000000;
    options_cur.min_eyes_height=-1;
    options_cur.eye_open_koeff=0;
    options_cur.normal_centers_dist=1000000;
    options_cur.centers_dist_in_cm=1000000;
    options_cur.too_near_based_on_calibration=false;

    options_new=options_cur;

    r=255;
    g=255;
    b=255;

    dist_to_monitor=0;

    cur_face_up_koeff=0;
    cur_face_down_koeff=0;
    cur_face_side_koeff=0;

    expand_koeff=2;
    eye_lenght_normalize_koeff=0.7;

    centers_dist_normalize_koeff=1.3;
    eyes_height_koeff=4.5;

    eye_lenght_error=15;
    false_squint_count=0;

    false_eyes_count=0;

    cam_index=0;

    cam_monitor_count=0;
    cam_monitor_max=3;

    max_false_eyes=5;

    find_eyes_refresh_iterations=3;
    find_eyes_refresh_count=0;

    K1=45;
    K2=30;
    K3=3;

    frame=0;
    old_frame=0;
    new_frame=0;
    grey_frame=0;

    face_is_found=false;
    left_eye_is_found=false;
    right_eye_is_found=false;

    QString path_to_dir = QDir::homePath();
    path_to_dir += "/Viewaide";
    QDir dir(path_to_dir);
    if (!dir.exists())
        dir.mkpath(path_to_dir);

    QString path_to_file = QDir::homePath();
    path_to_file += "/Viewaide/";
    path_to_file += "log.txt";

    slotAutoRun(true);

    log_file.setFileName(path_to_file);
    log_file.open(QIODevice::WriteOnly | QIODevice::Text);
    log.setDevice(&log_file);

    log<<"CamStream object created"<<"\n";
}

CamStream::~CamStream()
{
    log<<"CamStream object deleted"<<"\n";
    log_file.close();
}

QImage CamStream::ToQImage(IplImage* image)
{
    const uchar *qImageBuffer =(const uchar*)image->imageData;
    QImage img(qImageBuffer, image->width, image->height, image->widthStep, QImage::Format_RGB888);
    return img.rgbSwapped();
}

void CamStream::pause()
{
    log<<"Pause camstream thread"<<"\n";
    if(pause_thread)
        return;
    pause_thread=true;

    while(!thread_is_paused)QThread::msleep(1);;
}

void CamStream::slotAutoRun( bool active )
{
    #ifdef Q_OS_WIN
//    QString old_path = QCoreApplication::applicationDirPath();
//    old_path += "/Viewaide.exe";
//    QString path = QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation).at(0);
//    path += "/Startup";
//    path += "/Viewaide";
//    QFile::link(old_path,path);

    if ( active )
    {
        QString old_path = QCoreApplication::applicationDirPath();
        old_path += "/Viewaide.exe";
        QString path = QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation).at(0);
        path += "/Startup";
        path += "/Viewaide.lnk";
        QFile::link(old_path,path);
    }
    else
    {
        QString path = QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation).at(0);
        path += "/Startup";
        path += "/Viewaide.lnk";
        QFile(path).remove();

    }
    #endif
    #ifdef Q_OS_MAC
    QSettings *setting = new QSettings(QDir::homePath() + QDir::separator() + "Library/Preferences/loginwindow.plist",QSettings::NativeFormat);
    if ( active )
    {
        QDir dir(QCoreApplication::applicationDirPath());
        dir.cdUp();
        dir.cdUp();

        QVariantList lst = qvariant_cast<QVariantList >(setting->value("AutoLaunchedApplicationDictionary"));
        bool exist = false;
        for (int i = 0; i < lst.count(); ++i)
        {
            QVariantMap prop = qvariant_cast<QVariantMap >(lst[i]);
            if (prop["Path"] == dir.absolutePath())
            {
                exist = true;
                break;
            }
        }

        if (exist == false)
        {
            QVariantMap v;
            v["Path"] = dir.absolutePath();
            v["Hidden"] = false;
            lst.append(v);
            setting->setValue("AutoLaunchedApplicationDictionary",lst);
        }
    }
    else
    {
       setting->remove("AutoLaunchedApplicationDictionary");

    }
    #endif
}

void CamStream::slotSaveSettings(int state)
{
    QString sender_name = QObject::sender()->objectName();
    SaveSettings("settings.ini", sender_name, state);
}

void CamStream::slotSaveMetrics()
{
    save_metrics=true;
}

void CamStream::SaveSettings(QString filename, QString sender_name, int state)
{
    QString path_to_file = QDir::homePath();
    path_to_file += "/Viewaide/";
    path_to_file += filename;
    QFile file(path_to_file);
    file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append);
    file.seek(0);
    while ( !file.atEnd() )
    {
        QString str = file.readLine();
        QString line;
        line.push_back(QString::number(state));

        if ( str.startsWith(sender_name) )
        {
            str.clear();
            str.insert(0, line.toLower());
            file.write(str.toUtf8());
            break;
        }
    }
    if ( file.atEnd() )
    {
        QTextStream file_stream;
        file_stream.setDevice(&file);
        file_stream << sender_name << endl << state << endl;
    }
    file.close();
}

void CamStream::resume()
{
    log<<"Resume camstream thread"<<"\n";
    if(!pause_thread)
    {
        log<<"Thread was not paused\n";
        return;
    }
    pause_thread=false;
    thread_is_paused=false;
}

void CamStream::stop()
{
    log<<"Stop camstream thread"<<"\n";
    if(stop_thread)
        log<<"Thread is already stopped\n";
    stop_thread=true;
}

void CamStream::SetCalibrationMode(int msecs)
{
    this->pause();

    calibration_count=0;
    calibration_dist_sum=0;
    calibration_height_sum=0;
    calibration_mode_on=true;
    calibration_long=msecs;
    time=QTime::currentTime();

    this->resume();
}

void CamStream::AcceptNewOptions()
{
    log<<"Accepting new options\n";
    this->pause();
    options_cur=options_new;
    this->resume();
}

void CamStream::CheckEyes()
{
    if((((float)(left_eye_params.length+right_eye_params.length)/100)*(100-eye_lenght_error))/((float)(max(left_eye.height,right_eye.height)*2))<options_cur.eye_open_koeff)
    {
        false_open_count=0;

        if(eyes_are_closed)
        {
            eyes_are_closed=false;
            false_squint_count=min_iterations;
        }

        if(false_squint_count>=min_iterations)
        {
            false_squint_count=0;

            r=255;
            g=0;
            b=0;

            if(!squint_time_set)
            {
                squint_time=QTime::currentTime();
                squint_time_set=true;
            }
            else
                if(squint_time.elapsed()>squint_time_msecs)
                {
                    squint_time_set=false;
                    squint_iteration_count++;
                    alert_params.eyes_are_squinted=true;
                }
        }
        else
            false_squint_count++;
    }
    else
    {
        if(((((float)(left_eye_params.length+right_eye_params.length))/100)*(100+eye_lenght_error))/((float)(max(left_eye.height,right_eye.height)*2))>options_cur.eye_open_koeff)
        {
            false_squint_count=0;

            if(eyes_are_closed)
            {
                eyes_are_closed=false;
                false_open_count=min_iterations;
            }

            if(false_open_count>=5)
            {
                false_open_count=0;

                r=255;
                g=255;
                b=255;

                alert_params.eyes_are_squinted=false;

                squint_time_set=false;
            }
            else
                false_open_count++;
        }
    }

    if((left_eye_params.up_border>=left_eye.height/2) || (right_eye_params.up_border>=right_eye.height/2))
    {
        eyes_are_closed=true;

        blink_iteration_count++;

        alert_params.few_blink=true;

        r=0;
        g=0;
        b=0;
    }
}

void CamStream::CheckDist()
{
    if(options_cur.too_near_based_on_calibration)
     {
         if(GetDistanceBetweenEyes(left_eye,right_eye)>options_cur.normal_centers_dist*centers_dist_normalize_koeff)
         {
             if(!too_near_time_set)
             {
                 too_near_time=QTime::currentTime();
                 too_near_time_set=true;
             }
             else
                 if(too_near_time.elapsed()>too_near_time_msecs)
                 {
                     too_near_time_set=false;
                     emit EyesAreTooNear();
                     alert_params.bad_monitor_distance=true;
                 }
         }
         else
             too_near_time_set=false;
     }
     else
    {
         dist_to_monitor=DistanceToMonitor(left_eye,right_eye,options_cur.centers_dist_in_cm,frame);

         if(dist_sum<=0)
         {
             dist_sum=dist_to_monitor;
             dist_sum_count=1;
         }
         else
         {
             dist_sum+=dist_to_monitor;
             dist_sum_count++;
         }

         if(dist_to_monitor<min_dist_to_monitor_in_cm)
         {
             if(!too_near_time_set)
             {
                 too_near_time=QTime::currentTime();
                 too_near_time_set=true;
             }
             else
                 if(too_near_time.elapsed()>too_near_time_msecs)
                 {
                     too_near_time_set=false;
                     emit EyesAreTooNear();
                     alert_params.bad_monitor_distance=true;
                 }
         }
         else
             too_near_time_set=false;
     }
}

void CamStream::CheckBridgeLevel()
{
    if(((left_eye.y+left_eye.height/2)+(right_eye.y+right_eye.height/2))/2>options_cur.max_eyes_height)
    {
        DrawBridgeLevel(options_cur.max_eyes_height,frame);
        if(!too_low_time_set)
        {
            too_low_time=QTime::currentTime();
            too_low_time_set=true;
        }
        else
            if(too_low_time.elapsed()>too_low_time_msecs)
            {
                too_low_time_set=false;
                emit EyesAreTooLow();
                alert_params.too_low_angle=true;
            }
    }
    else
        too_low_time_set=false;

    if(((left_eye.y+left_eye.height/2)+(right_eye.y+right_eye.height/2))/2<options_cur.min_eyes_height)
    {
        DrawBridgeLevel(options_cur.min_eyes_height,frame);
        if(!too_heigh_time_set)
        {
            too_heigh_time=QTime::currentTime();
            too_heigh_time_set=true;
        }
        else
            if(too_heigh_time.elapsed()>too_heigh_time_msecs)
            {
                too_heigh_time_set=false;
                emit EyesAreTooHeigh();
                alert_params.too_height_angle=true;
            }
    }
    else
        too_heigh_time_set=false;
}

void CamStream::CheckLightness()
{
    back_img=cvCreateImage(cvGetSize(grey_frame),grey_frame->depth,1);
    cvCopy(grey_frame, back_img);

    cvSetImageROI(grey_frame,face);
    face_img=cvCreateImage(cvGetSize(grey_frame),grey_frame->depth,1);

    cvCopy( grey_frame, face_img);
    cvResetImageROI(grey_frame);

    cvSetImageROI(back_img,face);

    cvSub (  back_img, face_img, back_img );

    cvResetImageROI(back_img);

    face_dst=cvCreateImage(cvGetSize(face_img),face_img->depth,1);
    back_dst=cvCreateImage(cvGetSize(back_img),back_img->depth,1);

    cvCanny ( back_img, back_dst, K1, K2, K3 );
    cvSub ( back_img, back_dst, back_img );
    cvCanny ( face_img, face_dst, K1, K2, K3 );
    cvSub ( face_img, face_dst, face_img );

    cvReleaseImage(&back_dst);
    back_dst=cvCreateImage(cvGetSize(face_img),back_img->depth,1);
    cvResize(back_img,back_dst);

    int hist_size = 64;
    float range_0[]={1,256};
    float* ranges[] = { range_0 };
    int bin_w;

    CvHistogram *hist;
    IplImage *hist_image = cvCreateImage(cvSize(320,240), face_img->depth, 1);
    hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
    cvCalcHist( &face_img, hist, 0, NULL );
    bin_w = cvRound((double)hist_image->width/hist_size);
    cvSet( hist_image, cvScalarAll(255), 0 );


    CvHistogram *hist2;
    IplImage *hist_image2 = cvCreateImage(cvSize(320,240), back_img->depth, 1);
    hist2 = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
    cvCalcHist( &back_dst, hist2, 0, NULL );
    bin_w = cvRound((double)hist_image2->width/hist_size);
    cvSet( hist_image2, cvScalarAll(255), 0 );


    double sum_dark_pix_face = 0, sum_light_pix_face = 0, sum_dark_pix_back = 0, sum_light_pix_back = 0;

    for( int i = 0, j = hist_size - 1; i < hist_size / 2; i++, --j )
    {
        int y1_face, y2_face, y1_back, y2_back;

        y1_face = hist_image->height;
        y2_face = hist_image->height - cvRound(cvGetReal1D(hist->bins,i));
        sum_dark_pix_face += abs ( y1_face - y2_face );

        y1_back = hist_image2->height;
        y2_back = hist_image2->height - cvRound(cvGetReal1D(hist2->bins,i));
        sum_dark_pix_back += abs ( y1_back - y2_back );


        y1_face = hist_image->height;
        y2_face = hist_image->height - cvRound(cvGetReal1D(hist->bins,j));
        sum_light_pix_face += abs ( y1_face - y2_face );


        y1_back = hist_image2->height;
        y2_back = hist_image2->height - cvRound(cvGetReal1D(hist2->bins,j));
        sum_light_pix_back += abs ( y1_back - y2_back );
    }

    sum_dark_pix_face = sum_dark_pix_face / hist_size / 2;
    sum_dark_pix_back = sum_dark_pix_back / hist_size / 2;
    sum_light_pix_face = sum_light_pix_face / hist_size / 2;
    sum_light_pix_back = sum_light_pix_back / hist_size / 2;

//            qDebug() << "face: " << sum_dark_pix_face << sum_light_pix_face;
//            qDebug() << "back: " << sum_dark_pix_back << sum_light_pix_back;

    if ( ( ( sum_light_pix_back != 0 && sum_light_pix_face != 0 ) &&
        ( ( ( sum_light_pix_face / sum_light_pix_back > 1.7 ) && ( sum_dark_pix_back / sum_dark_pix_face > 1.7 ) )
             || sum_light_pix_face / sum_light_pix_back > 8 ) ) ||
    ( ( sum_light_pix_back <= 5 && sum_light_pix_face <= 5 && sum_dark_pix_face >= 110 && sum_dark_pix_back >= 110)) )
    {
        if(!bad_lighting_time_set)
        {
            bad_lighting_time=QTime::currentTime();
            bad_lighting_time_set=true;
        }
        else if(bad_lighting_time.elapsed()>bad_lighting_msecs)
        {
            bad_lighting_time_set=false;
            emit BadLightness();
            alert_params.bad_lightness=true;
        }
    }
    else
        bad_lighting_time_set=false;


    cvReleaseHist(&hist);
    cvReleaseHist(&hist2);

    cvReleaseImage(&hist_image);
    cvReleaseImage(&hist_image2);

    cvReleaseImage(&face_img);
    cvReleaseImage(&back_img);
    cvReleaseImage(&face_dst);
    cvReleaseImage(&back_dst);
}

void CamStream::CheckForDefects(IplImage* frame)
{
    CheckEyes();

    DrawRect(left_eye,r,g,b,frame);
    DrawRect(right_eye,r,g,b,frame);

    CheckDist();

    CheckBridgeLevel();

    CheckLightness();
}

void CamStream::ChangeCamIndex(int index)
{
    if(index>=0)
    {
        stop();
        wait();
        log<<"Cam index is set to "<<index<<"\n";
        cam_index=index;
        start();
    }
    else
        log<<"Cam index cannot be set to "<<index<<"\n";
}

void CamStream::SaveOptions(QString filename)
{
    log<<"Saving options"<<"\n";
    QFile file(filename);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream file_stream;
    file_stream.setDevice(&file);
    file_stream<<options_new.eye_open_koeff<<endl;
    if(options_new.too_near_based_on_calibration)
        file_stream<<1<<endl;
    else
        file_stream<<0<<endl;
    file_stream<<options_new.normal_centers_dist<<endl<<options_new.centers_dist_in_cm<<endl
               <<options_new.max_eyes_height<<endl<<options_new.min_eyes_height;
    file.close();
}

void CamStream::LoadOptions(QString filename)
{
    log<<"Loading options"<<"\n";
    QFile file(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream file_stream;
    file_stream.setDevice(&file);
    file_stream>>options_cur.eye_open_koeff;
    int temp;
    file_stream>>temp;
    if(temp==0)
        options_cur.too_near_based_on_calibration=false;
    else
        options_cur.too_near_based_on_calibration=true;
    file_stream>>options_cur.normal_centers_dist>>options_cur.centers_dist_in_cm
               >>options_cur.max_eyes_height>>options_cur.min_eyes_height;
    file.close();
}

void CamStream::SaveStatistics(alert_activations alert_count,QString filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadWrite | QIODevice::Text);

    QDate date=QDate::currentDate();

    QTextStream file_stream;
    file_stream.setDevice(&file);

    int p1=alert_iteration_count*(alert_iteration_time_msecs/1000);
    int p2=alert_activ_count.bad_monitor_distance;
    int p3=alert_activ_count.few_blink;
    int p4=alert_activ_count.eyes_are_squinted;
    int p5=alert_activ_count.too_height_angle;
    int p6=alert_activ_count.too_low_angle;
    int p7=alert_activ_count.bad_lightness;
    float p8=dist_sum/dist_sum_count;
    int p9=blink_sum;

    if(file.size()>0)
    {
        QStringList list;
        QString str;
        str=file_stream.readLine();
        while(!str.isNull())
        {
            list.push_back(str);
            str=file_stream.readLine();
        }
        str=list.at(list.size()-1);
        list.removeLast();

        if(list.size()>0)
            SendStatisticsToServer(list);

        QStringList parts = str.split(" ", QString::KeepEmptyParts);

        int day=parts[0].toInt();
        int month=parts[1].toInt();
        int year=parts[2].toInt();

        if((day==date.day()) && (month==date.month()) && (year==date.year()))
        {
            file.close();
            file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
            for(int i=0;i<list.size();++i)
                file_stream<<list.at(i)<<'\n';

            p1+=parts[3].toInt();
            p2+=parts[4].toInt();
            p3+=parts[5].toInt();
            p4+=parts[6].toInt();
            p5+=parts[7].toInt();
            p6+=parts[8].toInt();
            p7+=parts[9].toInt();
            p8=(parts[10].toFloat()*parts[3].toInt()+(p8*(p1-parts[3].toInt())))/p1;
            p9+=parts[11].toInt();
        }
    }
    file.close();
    file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text);
    file_stream<<QString::number(date.day())+" "+QString::number(date.month())+" "+QString::number(date.year())
               <<" "<<p1<<" "<<p2<<" "<<p3<<" "<<p4<<" "<<p5<<" "<<p6<<" "<<p7<<" "<<p8<<" "<<p9<<"\n";

    file.close();
}

void CamStream::SendStatisticsToServer(QStringList &list)
{
    QString path_to_file = QDir::homePath();
    path_to_file += "/Viewaide/";
    path_to_file += "account.txt";
    QFile file(path_to_file);
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    QTextStream file_stream;
    file_stream.setDevice(&file);
    QString login=file_stream.readLine();

    ConnectWithServer obj;
    if(obj.netIsWorking())
    {
        log<<"Send statistics\n";
        for(int i=0;i<list.size();++i)
            obj.uploadAllData(login+" "+list.at(i),"http://viewaide.com/getstats.php","stats");
        list.clear();
    }
    log<<"Internet problems while sending statistics\n";
}

void CamStream::slotAlertOn()
{
    alert_on=true;
}

void CamStream::slotAlertOff()
{
    alert_on=false;
}

QStringList CamStream::GetCamsList()
{
    QStringList list;

    list<<tr("Unavailable");

    #ifdef Q_OS_WIN
    int num=VI.listDevices();
    if(num>0)
        list.clear();
    log<<"Gets cams list with "<<num<<" devices:"<<"\n";
    for(int i=0;i<num;++i)
    {
        list<<VI.getDeviceName(i);
        log<<"  "<<VI.getDeviceName(i)<<"\n";
    }
    #endif

    return list;
}

void CamStream::CalibFace()
{
    if(left_eye_is_found && right_eye_is_found)
    {
        CvPoint center=GetEyesCenter(left_eye,right_eye);

        if((center.x>(frame->width/3)) && (center.x<(2*(frame->width/3))) &&
           (center.y>(frame->height/3)) && (center.y<(2*(frame->height/3))))
        {
            DrawRect(cvRect(frame->width/3,frame->height/3,frame->width/3,frame->height/3),0,255,0,frame);
            CheckCalibration(1);
        }
        else
        {
            DrawRect(cvRect(frame->width/3,frame->height/3,frame->width/3,frame->height/3),255,0,0,frame);
            CheckCalibration(0);
        }
    }
    else
    {
        DrawRect(cvRect(frame->width/3,frame->height/3,frame->width/3,frame->height/3),255,0,0,frame);
        CheckCalibration(0);
    }
}

void CamStream::CalibEyes()
{
    if(left_eye_is_found && right_eye_is_found)
    {
        DrawRect(left_eye,255,255,255,frame);
        DrawRect(right_eye,255,255,255,frame);
        options_cur.centers_dist_in_cm=(GetDistanceBetweenEyes(left_eye,right_eye))/((float)face.height/17.);
        options_new.centers_dist_in_cm=options_cur.centers_dist_in_cm;
        CheckCalibration(1);
    }
    else
        CheckCalibration(0);
}

void CamStream::CalibDistance()
{
    float dist=DistanceToMonitor(left_eye,right_eye,options_cur.centers_dist_in_cm,frame);
    DrawDistance(dist,frame);
    if((dist>=min_dist_to_monitor_in_cm) && left_eye_is_found && right_eye_is_found)
        CheckCalibration(1);
    else
        CheckCalibration(0);
}

bool CamStream::CamIsBusy(IplImage* frame)
{
    CvScalar sum=cvSum(frame);
    if(sum.val[0]+sum.val[1]+sum.val[2]==0)
    {
        log<<"Cam is busy"<<"\n";
        emit BusyCam();
        return true;
    }
    else
        return false;
}

void CamStream::ChangeCalibrationStage(calibration_stages stage)
{
    calib_stage=stage;
}

void CamStream::SaveFrame(IplImage* frame, QString filename)
{
    cvSaveImage(filename.toStdString().c_str(),frame);
}

void CamStream::AddToAlertCacheSquint(int n)
{
    for(int i=1;i<alert_cache_size;++i)
        alert_cache_squint[i-1]=alert_cache_squint[i];
    alert_cache_squint[alert_cache_size-1]=n;
}

void CamStream::AddToAlertCacheBlink(int n)
{
    for(int i=1;i<alert_cache_size;++i)
        alert_cache_blink[i-1]=alert_cache_blink[i];
    alert_cache_blink[alert_cache_size-1]=n;
}

bool CamStream::TooMuchSquintInAlertCache()
{
    int sum=0;
    for(int i=0;i<alert_cache_size;++i)
        sum+=alert_cache_squint[i];
    qDebug()<<"squint sum"<<sum;
    if(sum>normal_squint_per_min)
        return true;
    else
        return false;
}

bool CamStream::TooFewBlinkInAlertCache()
{
    int sum=0;
    for(int i=0;i<alert_cache_size;++i)
        sum+=alert_cache_blink[i];
    qDebug()<<"blink sum"<<sum;
    if(sum<normal_blink_per_min)
        return true;
    else
        return false;
}

void CamStream::RefreshSquintAlertCache()
{
    for(int i=0;i<alert_cache_size;++i)
        alert_cache_squint[i]=0;
}

void CamStream::RefreshBlinkAlertCache()
{
    for(int i=0;i<alert_cache_size;++i)
        alert_cache_blink[i]=2;
}

void CamStream::SaveCurFaceKoeffs()
{
    CvPoint center=GetEyesCenter(left_eye,right_eye);

    float centers_dist=GetDistanceBetweenEyes(left_eye,right_eye);

    cur_face_up_koeff=abs(center.y-face.y)/centers_dist;
    cur_face_down_koeff=abs(face.y+face.height-center.y)/centers_dist;
    cur_face_side_koeff=abs(center.x-face.x)/centers_dist;
}

void CamStream::run()
{
    emit UnlockCam();
    stop_thread=false;


    QString path_to_file = QDir::homePath();
    path_to_file += "/Viewaide/";
    path_to_file += "options.txt";

    LoadOptions(path_to_file);

    if (!(capture=cvCaptureFromCAM(cam_index)))
    {
        emit BusyCam();
        log<<"Error capturing cam index "<<cam_index<<"\n";
        log<<"Cam is busy\n";
        while(CamIsBusy(frame)&&(!stop_thread))
        {
            capture=cvCaptureFromCAM(cam_index);
            frame=cvQueryFrame(capture);
        }
    }
    emit UnlockCam();
    cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH,640);
    cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT,480);

    alert_activ_count.bad_monitor_distance=0;
    alert_activ_count.few_blink=0;
    alert_activ_count.eyes_are_squinted=0;
    alert_activ_count.too_height_angle=0;
    alert_activ_count.too_low_angle=0;
    alert_activ_count.bad_lightness=0;

    InitCascades();

    alert_iteration_time=QTime::currentTime();

    /////////////////////////////////////////////////

//    QString v1_path = QCoreApplication::applicationDirPath();
//    v1_path += "//capture_clean.avi";

//    const char* filename_clean = v1_path.toStdString().c_str();
//    qDebug()<<filename_clean;

//    QString v2_path = QCoreApplication::applicationDirPath();
//    v2_path += "//capture_alert.avi";

//    const char* filename_alert = v2_path.toStdString().c_str();

//    double fps = cvGetCaptureProperty (capture,CV_CAP_PROP_FPS);
//    CvSize size = cvSize((int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH),(int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT));

//    CvVideoWriter* writer_clean = cvCreateVideoWriter(filename_clean, CV_FOURCC('D','I','V','X'), 7, size, 1);
//    CvVideoWriter* writer_alert = cvCreateVideoWriter(filename_alert, CV_FOURCC('D','I','V','X'), 7, size, 1);

    /////////////////////////////////////////////////

    while(true)
    {
        QThread::msleep(5000);
        if ( !is_popup_showed )
        {
            emit sigCheckUpdate();
            break;
        }
    }

    while((!stop_thread))
    {
        if(thread_is_paused)
            continue;

        #ifdef Q_OS_WIN
        if(cam_monitor_count>=cam_monitor_max)
        {
            cam_monitor_count=0;
            temp_capture=cvCaptureFromCAM(cam_index);
            if(!temp_capture)
            {
                log<<"Error capturing cam index "<<cam_index<<"\n";
                stop_thread=true;
                log<<"Try to stop the thread"<<"\n";
            }
        }
        else
            cam_monitor_count++;
        #endif

        alert_params.bad_monitor_distance=false;
        alert_params.few_blink=false;
        alert_params.eyes_are_squinted=false;
        alert_params.too_height_angle=false;
        alert_params.too_low_angle=false;
        alert_params.bad_lightness=false;

        frame=cvQueryFrame(capture);

        if (!frame)
        {
            log<<"Error frame"<<"\n";
            continue;
        }

//        //////////
//        cvWriteFrame(writer_clean, frame);
//        //////////

        ///////
//        QTime t;
//        t.start();
//        face=Find(FACE,frame,face_is_found,0,0.5);
//        DrawRect(face,255,255,255,frame);
//        IplImage *small_frame=cvCreateImage(cvSize((int)(frame->width*0.5),(int)(frame->height*0.5)),frame->depth,frame->nChannels);
//        cvResize(frame,small_frame);
//        cvSaveImage("test.jpg",small_frame);
        ///////

        if(false_eyes_count<=0)
        {
            face_is_found=false;
            face=Find(FACE,frame,face_is_found,0,0.5);

            face_refreshed=true;
        }

        if(face_is_found)
        {
            if(find_eyes_refresh_count==0)
            {
                CvRect temp;

                left_eye_area=left_eye;
                if(false_eyes_count<=0)
                    left_eye_area=ClarifyArea(LEFT_EYE,&face);
                else
                    ExpandArea(left_eye_area,expand_koeff);
                temp=Find(LEFT_EYE,frame,left_eye_is_found,&left_eye_area);
                if(left_eye_is_found)
                    left_eye=temp;

                right_eye_area=right_eye;
                if(false_eyes_count<=0)
                    right_eye_area=ClarifyArea(RIGHT_EYE,&face);
                else
                    ExpandArea(right_eye_area,expand_koeff);
                temp=Find(RIGHT_EYE,frame,right_eye_is_found,&right_eye_area);
                if(right_eye_is_found)
                    right_eye=temp;
            }
            find_eyes_refresh_count++;

            if((left_eye_is_found && right_eye_is_found) && (!EyesCorrect(left_eye,right_eye,frame)))
            {
                left_eye_is_found=false;
                right_eye_is_found=false;
                false_eyes_count=1;
            }

            if(left_eye_is_found && right_eye_is_found)
                false_eyes_count=max_false_eyes;
            else
                false_eyes_count--;
        }

        grey_frame=cvCreateImage(cvSize(frame->width,frame->height),frame->depth,1);
        cvCvtColor(frame,grey_frame,CV_RGB2GRAY);

        if(alert_on)
            DrawDistance(dist_to_monitor,frame);

        if(left_eye_is_found && right_eye_is_found)
        {
            if(face_refreshed)
                SaveCurFaceKoeffs();
            else
                face=BuildFace(cur_face_up_koeff,cur_face_down_koeff,cur_face_side_koeff,left_eye,right_eye);

            if(alert_on)
                DrawRect(face,150,150,150,frame);

            int* eye_params;

            cvSetImageROI(grey_frame,left_eye);
            eye_params=GetEyeDist(grey_frame);
            left_eye_params.length=eye_params[0];
            left_eye_params.down_border=eye_params[1];
            left_eye_params.up_border=eye_params[2];
            cvResetImageROI(grey_frame);

            cvSetImageROI(frame,left_eye);
            if(alert_on)
                DrawRhombus(eye_params,0,255,0,left_eye.height,left_eye.width,frame);
            delete[] eye_params;
            cvResetImageROI(frame);

            cvSetImageROI(grey_frame,right_eye);
            eye_params=GetEyeDist(grey_frame);
            right_eye_params.length=eye_params[0];
            right_eye_params.down_border=eye_params[1];
            right_eye_params.up_border=eye_params[2];
            cvResetImageROI(grey_frame);

            cvSetImageROI(frame,right_eye);
            if(alert_on)
                DrawRhombus(eye_params,0,255,0,right_eye.height,right_eye.width,frame);
            delete[] eye_params;
            cvResetImageROI(frame);

            if(calibration_mode_on)
            {
                if(time.elapsed()<calibration_long)
                {
                    calibration_dist_sum+=(left_eye_params.length+right_eye_params.length)/2.;
                    calibration_height_sum+=max(left_eye.height,right_eye.height);
                    if(options_cur.too_near_based_on_calibration)
                        calibration_centers_dist_sum+=GetDistanceBetweenEyes(left_eye,right_eye);
                    calibration_eyes_height_sum+=((left_eye.y+left_eye.height/2)+(right_eye.y+right_eye.height/2))/2;
                    calibration_count++;
                }
                else
                {
                    options_new.eye_open_koeff=(calibration_dist_sum/calibration_height_sum)*eye_lenght_normalize_koeff;
                    options_new.normal_centers_dist=calibration_centers_dist_sum/calibration_count;
                    options_new.max_eyes_height=(((calibration_eyes_height_sum/calibration_count)+frame->height/eyes_height_koeff)>frame->height)?frame->height:((calibration_eyes_height_sum/calibration_count)+frame->height/eyes_height_koeff);
                    options_new.min_eyes_height=(((calibration_eyes_height_sum/calibration_count)-frame->height/eyes_height_koeff)<0)?0:((calibration_eyes_height_sum/calibration_count)-frame->height/eyes_height_koeff);
                    calibration_mode_on=false;
                    qDebug()<<"Calibration: OK";

                    path_to_file = QDir::homePath();
                    path_to_file += "/Viewaide/";
                    path_to_file += "options.txt";
                    SaveOptions(path_to_file);
                    options_cur=options_new;

                    calibration_dist_sum = 0;
                    calibration_height_sum = 0;
                    calibration_centers_dist_sum = 0;
                    calibration_eyes_height_sum = 0;
                    calibration_count = 0;
                }
            }
            else
            {
                if(alert_on)
                {

                    CheckForDefects(frame);

                    if(!cur_time_set)
                    {
                        cur_time=QTime::currentTime();
                        cur_time_set=true;
                    }
                    else
                        if(cur_time.elapsed()>notification_time)
                            cur_time_set=false;
                }
            }
        }
        else
        {
            squint_time_set=false;
            too_near_time_set=false;

            find_eyes_refresh_count=0;
        }

        if(find_eyes_refresh_count>find_eyes_refresh_iterations)
            find_eyes_refresh_count=0;

        if(CamIsBusy(frame))
        {
            log<<"Cam is busy\n";
            while(CamIsBusy(frame)&&(!stop_thread))
            {
                capture=cvCaptureFromCAM(cam_index);
                frame=cvQueryFrame(capture);
            }
            emit UnlockCam();
        }

        if(!alert_on)
        {
            switch(calib_stage)
            {
            case STAGE1:
                CalibFace();
                break;
            case STAGE2:
                CalibEyes();
                break;
            case STAGE3:
                CalibDistance();
                break;
            }
        }

        if(frame)
        {
            current_image=ToQImage(frame);
            emit ImageIsReady(current_image);
        }

//        //////////
//        cvWriteFrame(writer_alert, frame);
//        //////////
        if((left_eye_is_found && right_eye_is_found) && alert_on)
        {
            if(alert_iteration_time.elapsed()>alert_iteration_time_msecs)
            {
                alert_iteration_time=QTime::currentTime();
                qDebug()<<squint_iteration_count;
                qDebug()<<blink_iteration_count;
                AddToAlertCacheSquint(squint_iteration_count);
                AddToAlertCacheBlink(blink_iteration_count);

                if(blink_sum<=0)
                    blink_sum=blink_iteration_count;
                else
                    blink_sum+=blink_iteration_count;

                if(TooMuchSquintInAlertCache())
                {
                    TooMuchSquint();
                    alert_activ_count.eyes_are_squinted++;
                    RefreshSquintAlertCache();
                }
                if(TooFewBlinkInAlertCache())
                {
                    TooFewBlink();
                    alert_activ_count.few_blink++;
                    RefreshBlinkAlertCache();
                }
                squint_iteration_count=0;
                blink_iteration_count=0;

                alert_iteration_count++;
            }
        }
        else
        {
            if(iteration_error_count>=iteration_error)
            {
                alert_iteration_time=QTime::currentTime();
                squint_iteration_count=0;
                blink_iteration_count=0;
                iteration_error_count=0;
            }
            else
                iteration_error_count++;
        }

        if(alert_params.bad_monitor_distance)
            alert_activ_count.bad_monitor_distance++;
        if(alert_params.too_height_angle)
            alert_activ_count.too_height_angle++;
        if(alert_params.too_low_angle)
            alert_activ_count.too_low_angle++;
        if(alert_params.bad_lightness)
            alert_activ_count.bad_lightness++;

        if(alert_iteration_count>=alert_iterations_for_statistics)
        {
            path_to_file = QDir::homePath();
            path_to_file += "/Viewaide/";
            path_to_file += "statistics.txt";
            SaveStatistics(alert_activ_count,path_to_file);

            alert_iteration_count=0;

            alert_activ_count.bad_monitor_distance=0;
            alert_activ_count.few_blink=0;
            alert_activ_count.eyes_are_squinted=0;
            alert_activ_count.too_height_angle=0;
            alert_activ_count.too_low_angle=0;
            alert_activ_count.bad_lightness=0;
        }

        face_refreshed=false;

        msleep(75);

        if(grey_frame)
            cvReleaseImage(&grey_frame);
        if(old_frame)
            cvReleaseImage(&old_frame);
        if(new_frame)
            cvReleaseImage(&new_frame);

        if(pause_thread &&(!stop_thread))
            thread_is_paused=true;
    }

    #ifdef Q_OS_WIN
    cvReleaseCapture(&capture);
    cvReleaseCapture(&temp_capture);
    #endif

//    cvReleaseVideoWriter(&writer_clean);
//    cvReleaseVideoWriter(&writer_alert);

    log<<"Thread closes"<<"\n";
    return;
}

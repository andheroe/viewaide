#ifndef CAMSTREAM_H
#define CAMSTREAM_H

#include <QThread>
#include <QImage>
#include <QDebug>
#include <QTime>
#include <QStringList>
#include <QFile>
#include <QDate>

#ifdef Q_OS_WIN
#include "videoInput.h"
#endif

#include "opencv2/core/core.hpp"
#include "opencv/cv.h"

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "algorithms.h"
#include "updateapp.h"

using namespace cv;
using namespace std;

struct options_koeffs
{
    float eye_open_koeff;
    bool too_near_based_on_calibration;
    float centers_dist_in_cm;
    float normal_centers_dist;
    int max_eyes_height;
    int min_eyes_height;
};

struct eye_params
{
    int length;
    int up_border;
    int down_border;
};

struct alert_parametrs
{
    bool eyes_are_squinted;
    bool few_blink;
    bool bad_monitor_distance;
    bool too_height_angle;
    bool too_low_angle;
    bool bad_lightness;
};

struct alert_activations
{
    int eyes_are_squinted;
    int few_blink;
    int bad_monitor_distance;
    int too_height_angle;
    int too_low_angle;
    int bad_lightness;
};

enum calibration_stages
{
    STAGE1,
    STAGE2,
    STAGE3,
    STAGE4,
    STAGE5
};

class CamStream : public QThread
{
    Q_OBJECT

private:
    bool stop_thread;
    bool pause_thread;
    bool thread_is_paused;

    bool squint_time_set;
    bool too_near_time_set;
    bool too_heigh_time_set;
    bool too_low_time_set;
    bool bad_lighting_time_set;
    bool alert_on;
    bool compare_mode_on;
    bool eyes_are_closed;

    bool face_refreshed;

    alert_parametrs alert_params;
    alert_activations alert_activ_count;

    int normal_squint_per_min;
    int normal_blink_per_min;

    int alert_cache_size;
    int* alert_cache_squint;
    int* alert_cache_blink;

    float dist_sum;
    int dist_sum_count;
    int blink_sum;
    int blink_sum_count;

    QTime time;
    QTime squint_time;
    QTime too_near_time;
    QTime too_heigh_time;
    QTime too_low_time;
    QTime bad_lighting_time;


    int squint_time_msecs;
    int too_near_time_msecs;
    int too_heigh_time_msecs;
    int too_low_time_msecs;
    int bad_lighting_msecs;

    int min_iterations;

    int alert_iteration_time_msecs;
    int alert_iteration_count;
    int alert_iterations_for_statistics;

    QTime alert_iteration_time;

    int squint_iteration_count;
    int blink_iteration_count;

    int iteration_error;
    int iteration_error_count;

    QTime cur_time;
    bool cur_time_set;
    int notification_time;

    int calibration_long;
    int calibration_count;
    float calibration_dist_sum;
    float calibration_height_sum;
    float calibration_centers_dist_sum;
    float calibration_eyes_height_sum;

    calibration_stages calib_stage;

    int min_dist_to_monitor_in_cm;

    int cam_index;

    int cam_monitor_count;
    int cam_monitor_max;

    CvCapture* capture;
    CvCapture* temp_capture;

    CvRect face;
    CvRect left_eye_area;
    CvRect right_eye_area;
    CvRect left_eye;
    CvRect right_eye;

    eye_params left_eye_params;
    eye_params right_eye_params;

    int r;
    int g;
    int b;

    bool face_is_found;
    bool left_eye_is_found;
    bool right_eye_is_found;

    float dist_to_monitor;

    float expand_koeff;
    float eye_lenght_normalize_koeff;
    float centers_dist_normalize_koeff;
    float eyes_height_koeff;

    float cur_face_up_koeff;
    float cur_face_down_koeff;
    float cur_face_side_koeff;

    options_koeffs options_cur;
    options_koeffs options_new;

    int eye_lenght_error;
    int false_squint_count;
    int false_open_count;

    int false_eyes_count;

    int find_eyes_refresh_iterations;
    int find_eyes_refresh_count;

    int K1;
    int K2;
    int K3;

    IplImage* frame;
    IplImage* old_frame;
    IplImage* new_frame;
    IplImage* grey_frame;


    IplImage* face_img;
    IplImage* back_img;
    IplImage* face_dst;
    IplImage* back_dst;

    QImage current_image;

    int max_false_eyes;

    #ifdef Q_OS_WIN
    static videoInput VI;
    #endif

    static QFile log_file;
    static QTextStream log;



protected:
    QImage ToQImage(IplImage* image);
    void run();

public:
    bool calibration_mode_on;
    static bool is_popup_showed;
    CamStream();
    ~CamStream();


    void SetCalibrationMode(int msecs);
    void AcceptNewOptions();
    void CheckEyes();
    void CheckDist();
    void CheckBridgeLevel();
    void CheckLightness();
    void CheckForDefects(IplImage* frame);
    void ChangeCamIndex(int index);
    void SaveOptions(QString filename="options.txt");
    void LoadOptions(QString filename="options.txt");
    void SaveStatistics(alert_activations alert_count,QString filename);

    void CompareModeOn();
    void CompareModeOff();
    bool CamIsBusy(IplImage* frame);
    void SaveFrame(IplImage* frame, QString filename="sample_frame.jpg");
    static QStringList GetCamsList();

    inline void CalibFace();
    inline void CalibEyes();
    inline void CalibDistance();

    void AddToAlertCacheSquint(int n);
    void AddToAlertCacheBlink(int n);
    bool TooMuchSquintInAlertCache();
    bool TooFewBlinkInAlertCache();
    void RefreshSquintAlertCache();
    void RefreshBlinkAlertCache();

    void SaveCurFaceKoeffs();

signals:
    void ImageIsReady(QImage qimg);
    void OldImageIsReady(QImage qimg);
    void NewImageIsReady(QImage qimg);
    void EyesAreSquinted();
    void EyesAreTooNear();
    void EyesAreTooLow();
    void EyesAreTooHeigh();
    void TooMuchSquint();
    void TooFewBlink();
    void BadLightness();
    void BusyCam();
    void UnlockCam();
    void sigCheckUpdate();

    void CheckCalibration( bool checked );

public slots:
    void ChangeCalibrationStage(calibration_stages stage);
    void slotAlertOn();
    void slotAlertOff();
    void resume();
    void stop();
    void pause();
};

#endif // CAMSTREAM_H

#include "algorithms.h"

CvHaarClassifierCascade* face_cascade;
CvHaarClassifierCascade* left_eye_cascade;
CvHaarClassifierCascade* right_eye_cascade;

int ccc=0;//////////////////////////////////////////////////////////////////////

void DrawRect(CvRect rect,int r,int g,int b,IplImage* &frame)
{
    CvPoint pt1;
    CvPoint pt2;
    pt1.x=rect.x;
    pt1.y=rect.y;
    pt2.x=rect.x+rect.width;
    pt2.y=rect.y+rect.height;

    cvRectangle(frame,pt1,pt2,CV_RGB(r,g,b),3,4,0);
}

void DrawRhombus(int* params,int r,int g,int b,int height, int width,IplImage* &frame)
{
    cvLine(frame,cvPoint(0,height/2),cvPoint(width/2,params[2]-1),CV_RGB(r,g,b));
    cvLine(frame,cvPoint(0,height/2),cvPoint(width/2,params[1]+1),CV_RGB(r,g,b));
    cvLine(frame,cvPoint(width,height/2),cvPoint(width/2,params[2]-1),CV_RGB(r,g,b));
    cvLine(frame,cvPoint(width,height/2),cvPoint(width/2,params[1]+1),CV_RGB(r,g,b));
}

void DrawBridgeLevel(int y,IplImage* &frame)
{
    CvPoint pt1;
    CvPoint pt2;
    pt1.x=0;
    pt1.y=y;
    pt2.x=frame->width;
    pt2.y=y;

    cvLine(frame,pt1,pt2,CV_RGB(255,0,0),3,4,0);
}

void DrawDistance(float dist, IplImage *frame)
{
    cvLine(frame,cvPoint(frame->width/10,2*frame->height/10),cvPoint(frame->width/10,5.5*(frame->height/10)),CV_RGB(0,150,0),10);
    cvLine(frame,cvPoint(frame->width/10,5.5*frame->height/10),cvPoint(frame->width/10,6.5*(frame->height/10)),CV_RGB(255,150,0),10);
    cvLine(frame,cvPoint(frame->width/10,6.5*frame->height/10),cvPoint(frame->width/10,8*(frame->height/10)),CV_RGB(230,0,0),10);
    float dist_position;
    if(dist<10)
        dist_position=8;
    else
        if(dist>80)
            dist_position=2;
        else
            dist_position=((90.-dist+10)/10.);
    cvLine(frame,cvPoint(frame->width/10-frame->width/20,dist_position*frame->height/10),cvPoint(frame->width/10+frame->width/20,dist_position*frame->height/10),CV_RGB(0,0,0),2);
    dist_position=int(dist*100)/100.0;
    QString text;
    if((int)dist<10)
        text+=" ";
    if((int)dist<100)
        text+=" ";
    text=QString::number(dist_position);
    if(text.length()<5)
        text+="   ";
    else
        if(text.length()<6)
            text+="  ";
    text+=" sm";
    CvFont font=cvFont(1);
    cvPutText(frame,text.toStdString().c_str(),cvPoint(frame->width/10-frame->width/20,9*(frame->height/10)),&font,CV_RGB(255,255,255));
}

void InitCascades()
{
    QString path = QCoreApplication::applicationDirPath();
    #ifdef Q_OS_WIN
    path+="/../../";
    #endif
    #ifdef Q_OS_MAC
    path+="/../../../../";
    #endif
    #ifdef Q_OS_LINUX
    path+="/../";
    #endif
    path += "Viewaide/haarcascade_frontalface_alt_tree.xml";
    face_cascade=(CvHaarClassifierCascade*)cvLoad(path.toStdString().c_str(),0,0,0);

    path = QCoreApplication::applicationDirPath();
    #ifdef Q_OS_WIN
    path+="/../../";
    #endif
    #ifdef Q_OS_MAC
    path+="/../../../../";
    #endif
    #ifdef Q_OS_LINUX
    path+="/../";
    #endif
    path += "Viewaide/haarcascade_mcs_lefteye.xml";
    left_eye_cascade=(CvHaarClassifierCascade*)cvLoad(path.toStdString().c_str(),0,0,0);

    path = QCoreApplication::applicationDirPath();
    #ifdef Q_OS_WIN
    path+="/../../";
    #endif
    #ifdef Q_OS_MAC
    path+="/../../../../";
    #endif
    #ifdef Q_OS_LINUX
    path+="/../";
    #endif
    path +="Viewaide/haarcascade_mcs_righteye.xml";
    right_eye_cascade=(CvHaarClassifierCascade*)cvLoad(path.toStdString().c_str(),0,0,0);
}

CvRect Find(OBJECT obj,IplImage* frame,bool &state,CvRect* area,float resize_koeff)
{
    bool is_area=false;
    int x_shift=0,y_shift=0,width_shift=0,height_shift=0;
    CvMemStorage* storage=0;
    CvSeq* seq=0;
    CvRect rect;

    IplImage *small_frame=cvCreateImage(cvSize((int)(frame->width*resize_koeff),(int)(frame->height*resize_koeff)),frame->depth,frame->nChannels);
    cvResize(frame,small_frame);

    if(area && (area->x>=0) && (area->y>=0) && (area->x+area->width<=frame->width) && (area->y+area->height<=frame->height))
    {
        is_area=true;

        area->height*=resize_koeff;
        area->width*=resize_koeff;
        area->x*=resize_koeff;
        area->y*=resize_koeff;

        cvSetImageROI(small_frame,*area);

        x_shift=area->x;
        y_shift=area->y;
        width_shift=area->width;
        height_shift=area->height;
    }

    switch(obj)
    {
    case FACE:
        if(left_eye_cascade)
        {
            storage=cvCreateMemStorage(0);
            seq=cvHaarDetectObjects(small_frame,face_cascade,storage,1.2,3,CV_HAAR_DO_CANNY_PRUNING,cvSize(80.*resize_koeff,120.*resize_koeff),cvSize(320.*resize_koeff,480.*resize_koeff));
            if(seq->total>0)
            {
                rect=*((CvRect*)cvGetSeqElem(seq,0));
                state=true;
            }
            else
                state=false;
        }
        break;
    case LEFT_EYE:
        if(right_eye_cascade)
        {
            storage=cvCreateMemStorage(0);
            seq=cvHaarDetectObjects(small_frame,left_eye_cascade,storage,1.1,3,CV_HAAR_DO_CANNY_PRUNING,cvSize(30.*resize_koeff,20.*resize_koeff),cvSize(90.*resize_koeff,60.*resize_koeff));
            if(seq->total>0)
            {
                rect=*((CvRect*)cvGetSeqElem(seq,0));
                if(seq->total>1)
                {
                    CvRect temp_rect=*((CvRect*)cvGetSeqElem(seq,1));
                    if(temp_rect.y>rect.y)
                        rect=temp_rect;
                }
                state=true;
            }
            else
                state=false;
        }
        break;
    case RIGHT_EYE:
        if(face_cascade)
        {
            storage=cvCreateMemStorage(0);
            seq=cvHaarDetectObjects(small_frame,right_eye_cascade,storage,1.1,3,CV_HAAR_DO_CANNY_PRUNING,cvSize(30.*resize_koeff,20.*resize_koeff),cvSize(90.*resize_koeff,60.*resize_koeff));
            if(seq->total>0)
            {
                rect=*((CvRect*)cvGetSeqElem(seq,0));
                if(seq->total>1)
                {
                    CvRect temp_rect=*((CvRect*)cvGetSeqElem(seq,1));
                    if(temp_rect.y>rect.y)
                        rect=temp_rect;
                }
                state=true;
            }
            else
                state=false;
        }
        break;
    }

    if(storage)
    {
        cvClearMemStorage(storage);
        cvReleaseMemStorage(&storage);
    }


    if(is_area)
    {
        cvResetImageROI(frame);

        rect.x+=x_shift;
        rect.y+=y_shift;

        area->x=x_shift;
        area->y=y_shift;
        area->width=width_shift;
        area->height=height_shift;
    }

    cvReleaseImage(&small_frame);

    rect.height/=resize_koeff;
    rect.width/=resize_koeff;
    rect.x/=resize_koeff;
    rect.y/=resize_koeff;

    return rect;
}

CvRect ClarifyArea(OBJECT obj,CvRect* area)
{
    CvRect rect=*area;

    switch(obj)
    {
    case RIGHT_EYE:
        rect.width/=2.;
        rect.height*=2./3.;
        break;
    case LEFT_EYE:
        rect.width/=2.;
        rect.height*=2./3.;
        rect.x+=rect.width;
        break;
    }

    return rect;
}

void ExpandArea(CvRect &area, float koeff)
{
    area.x-=(area.width*koeff-area.width)/2;
    area.y-=(area.height*koeff-area.height)/2;
    area.width*=koeff;
    area.height*=koeff;
}

bool EyesCorrect(CvRect left_eye,CvRect right_eye,IplImage* frame)
{
    if(abs(left_eye.y-right_eye.y)>frame->height/8)
        return false;
    if(((left_eye.x-right_eye.x-right_eye.width)<0) || ((left_eye.x-right_eye.x-right_eye.width)>frame->width/4))
        return false;
    return true;
}

float DistanceToMonitor(CvRect left_eye, CvRect right_eye, int centers_coeff, IplImage *frame)
{
    return (1.6*((float)frame->height/((float)((left_eye.x+left_eye.width/2)-(right_eye.x+right_eye.width/2))/centers_coeff)));
}

int CalcEdgeSquare(int** mass,int i,int j,int z)
{
    if(mass[i][j]!=-1)
        return 0;

    mass[i][j]=z;
    return 1+
            CalcEdgeSquare(mass,i-1,j-1,z)+CalcEdgeSquare(mass,i,j-1,z)+CalcEdgeSquare(mass,i-1,j,z)+
            CalcEdgeSquare(mass,i+1,j+1,z)+CalcEdgeSquare(mass,i,j+1,z)+CalcEdgeSquare(mass,i+1,j,z)+
            CalcEdgeSquare(mass,i+1,j-1,z)+CalcEdgeSquare(mass,i-1,j+1,z);
}

CvPoint GetEyesCenter(CvRect left_eye, CvRect right_eye)
{
    CvPoint center;
    center.x=(left_eye.x+left_eye.width/2+right_eye.x+right_eye.width/2)/2;
    center.y=(left_eye.y+left_eye.height/2+right_eye.y+right_eye.height/2)/2;

    return center;
}

float GetDistanceBetweenEyes(CvRect left_eye, CvRect right_eye)
{
    return (left_eye.x+left_eye.width/2.)-(right_eye.x+right_eye.width/2.);
}

CvRect BuildFace(float cur_face_up_koeff,float cur_face_down_koeff,float cur_face_side_koeff,CvRect left_eye, CvRect right_eye)
{
    CvPoint center=GetEyesCenter(left_eye,right_eye);

    float centers_dist=GetDistanceBetweenEyes(left_eye,right_eye);

    return cvRect(abs(center.x-centers_dist*cur_face_side_koeff),abs(center.y-centers_dist*cur_face_up_koeff),centers_dist*cur_face_side_koeff*2,centers_dist*cur_face_up_koeff+centers_dist*cur_face_down_koeff);
}

int* GetEyeDist(IplImage* frame)
{   
    IplImage* eye;
    IplImage* dst;
    IplImage* dst_temp;

    //cvSaveImage(("C:/Users/andheroe/Desktop/Original/"+QString::number(ccc)+".jpg").toStdString().c_str(),frame);

    eye=cvCreateImage(cvGetSize(frame),frame->depth,1);
    cvCopy(frame,eye);
    dst=cvCreateImage(cvGetSize(eye),IPL_DEPTH_8U,1);

    dst_temp = cvCreateImage( cvGetSize(eye), IPL_DEPTH_16S, eye->nChannels);

    cvLaplace(eye, dst_temp, 9);
    cvConvertScale(dst_temp, dst);

    //cvSaveImage(("C:/Users/andheroe/Desktop/Stage1/"+QString::number(ccc)+".jpg").toStdString().c_str(),dst);

    uchar* ptr;
    int sum=0,counter=0;
    for(int y=frame->height*(1./5.);y<frame->height*(4./5.);++y)
    {
        ptr=(uchar*)(frame->imageData+y*frame->widthStep);

        for(int x=0;x<frame->width;++x)
        {
            sum+=ptr[x];
            counter++;
        }
    }

    int threshold=sum/counter;
    cvThreshold(dst,dst,threshold,255,CV_THRESH_BINARY);

    //cvSaveImage(("C:/Users/andheroe/Desktop/Stage2/"+QString::number(ccc)+".jpg").toStdString().c_str(),dst);

    for(int y=0;y<dst->height*(1./5.);++y)
    {
        ptr=(uchar*)(dst->imageData+y*dst->widthStep);

        for(int x=0;x<dst->width;++x)
            ptr[x]=150;
    }

    for(int y=dst->height*(4./5.);y<dst->height;++y)
    {
        ptr=(uchar*)(dst->imageData+y*dst->widthStep);

        for(int x=0;x<dst->width;++x)
            ptr[x]=150;
    }

    int h=dst->height*(4./5.)-dst->height*(1./5.)+1;
    int w=dst->width;
    int** mass=new int*[h];
    for(int i=0;i<h;++i)
        mass[i]=new int[w];

    for(int y=dst->height*(1./5.);y<=dst->height*(4./5.);++y)
    {
        ptr=(uchar*)(dst->imageData+y*dst->widthStep);

        for(int x=dst->width*(1./5.);x<dst->width*(4./5.)-3;++x)
            if((ptr[x]==255) && (ptr[x+3]==255))
            {
                ptr[x+1]=255;
                ptr[x+2]=255;
            }
    }

    for(int y=dst->height*(1./5.),i=0;y<=dst->height*(4./5.),i<h;++y,++i)
    {
        ptr=(uchar*)(dst->imageData+y*dst->widthStep);

        for(int x=0;x<dst->width;++x)
            if(ptr[x]==0)
                mass[i][x]=-2;
            else
                mass[i][x]=-1;
    }

    for(int i=0;i<h;++i)
    {
        mass[i][0]=-2;
        mass[i][w-1]=-2;
    }

    for(int i=0;i<w;++i)
    {
        mass[0][i]=-2;
        mass[h-1][i]=-2;
    }

    vector<edge> edges;
    int edge_count=0;

    for(int i=0;i<h;++i)
        for(int j=0;j<w;++j)
            if(mass[i][j]==-1)
            {
                edges.push_back(edge(edge_count,CalcEdgeSquare(mass,i,j,edge_count)));
                edge_count++;
            }

    if(edges.size()<=0)
        return 0;

    sort(edges.begin(), edges.end());

    for(int y=dst->height*(1./5.),i=0;y<=dst->height*(4./5.),i<h;++y,++i)
    {
        ptr=(uchar*)(dst->imageData+y*dst->widthStep);

        for(int x=0;x<dst->width;++x)
            if(mass[i][x]!=((edge)edges.at(0)).index)
                ptr[x]=0;
    }

    //cvSaveImage(("C:/Users/andheroe/Desktop/Stage3/"+QString::number(ccc)+".jpg").toStdString().c_str(),dst);

    bool is_line;
    int up=0,down=dst->height-1;
    for(up;up<dst->height*0.8;++up)
    {
        ptr=(uchar*)(dst->imageData+up*dst->widthStep);
        is_line=false;
        for(int x=dst->width/2;x<dst->width/2+1;++x)
            if(((int)ptr[x])==255)
            {
                is_line=true;
                break;
            }
        if(is_line)
            break;
    }

    for(down;down>dst->height*0.2;--down)
    {
        ptr=(uchar*)(dst->imageData+down*dst->widthStep);
        is_line=false;
        for(int x=dst->width/2;x<dst->width/2+1;++x)
            if(((int)ptr[x])==255)
            {
                is_line=true;
                break;
            }
        if(is_line)
            break;
    }

    //cvSaveImage(("C:/Users/andheroe/Desktop/Stage3/"+QString::number(ccc)+".jpg").toStdString().c_str(),dst);

    for ( int j = 0; j < h; ++j )
        delete []mass[j];
    delete []mass;

    cvReleaseImage(&eye);
    cvReleaseImage(&dst);
    cvReleaseImage(&dst_temp);

    int* res=new int[3];
    res[0]=down-up;
    res[1]=down;
    res[2]=up;

    return res;

}


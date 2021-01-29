// Proyecto1_fotografiar.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <EmergentCameraAPIs.h>
#include <emergentframe.h>
#include <EvtParamAttribute.h>
#include <gigevisiondeviceinfo.h>
#include <EmergentFrameSave.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>  

#include <string>   
#include <iomanip> 
#include <sstream>
#include <thread> 

#include<windows.h>

using namespace std;
using namespace Emergent;
using namespace cv;

#define SUCCESS 0
#define MAX_FRAMES 100

#define MAX_CAMERAS 4

#define TRUE  1
#define FALSE 0

#define GAIN_VAL      500
#define GAIN_VAL2      1000

#define OFFSET_VAL    0
#define AG_SET_VAL    512
#define AG_I_VAL      16

char* next_token;

void view_camera(CEmergentCamera* camera, CEmergentFrame* evtFrame, CEmergentCamera* camera2, CEmergentFrame* evtFrame2);
void showImages(const String& window_name, int rows, int cols, int size, std::initializer_list<const Mat*> images, int pad);
void getSquareImage(cv::InputArray img, cv::OutputArray dst, int size);



int main()
{
    printf("Librerias cargadas con exito \n");
    cout << "Cargando Variable" << endl << endl;

    CEmergentCamera camera, camera2;
    struct EvtParamAttribute attribute, attribute2;
    int ReturnVal = SUCCESS;
    unsigned int param_val_max, param_val_min, param_val_inc;
    unsigned int height_max, width_max;
    unsigned int height_final, width_final;
    CEmergentFrame evtFrame, evtFrame2;
    char filename[100];
    char user[50];
    struct GigEVisionDeviceInfo deviceInfo[MAX_CAMERAS];
    unsigned int count;
    EVT_ERROR err = EVT_SUCCESS;

    unsigned int listcam_buf_size = MAX_CAMERAS;
    EVT_ListDevices(deviceInfo, &listcam_buf_size, &count);

    if (count == 0)
    {
        printf("No camera found\n");
        return 0;
    }

    ReturnVal = EVT_CameraOpen(&camera, &deviceInfo[0]);

    if (ReturnVal != SUCCESS)
    {
        printf("Open Camera 1: \t\tError. Exiting program.\n");
        return ReturnVal;
    }
    else
    {
        printf("\t\tCAMERA 1 \n");
        printf("Open Camera 1: \t\tCamera Opened\n\n");
    }

    EVT_CameraGetUInt32ParamMax(&camera, "Height", &height_max);
    EVT_CameraGetUInt32ParamMax(&camera, "Width", &width_max);
    printf("Resolution: \t\t%d x %d\n", width_max, height_max);

    ReturnVal = EVT_CameraGetParamAttr(&camera, "Gain", &attribute);
       
    if (attribute.dataType == EDataTypeUnsupported)
    {
        printf("CameraGetParamAttr: Error\n");
        return ReturnVal;
    }
    else
    {
        EVT_CameraGetUInt32ParamMax(&camera, "Gain", &param_val_max);
        printf("Gain Max: \t\t%d\n", param_val_max);
        EVT_CameraGetUInt32ParamMin(&camera, "Gain", &param_val_min);
        printf("Gain Min: \t\t%d\n", param_val_min);
        if (GAIN_VAL >= param_val_min && GAIN_VAL <= param_val_max)
        {
            EVT_CameraSetUInt32Param(&camera, "Gain", GAIN_VAL);
            printf("Gain Set: \t\t%d\n", GAIN_VAL);
        }
    }

    ReturnVal = EVT_CameraGetParamAttr(&camera, "Offset", &attribute);

    if (attribute.dataType == EDataTypeUnsupported)
    {
        printf("CameraGetParamAttr: Error\n");
        return ReturnVal;
    }
    else
    {
        EVT_CameraGetUInt32ParamMax(&camera, "Offset", &param_val_max);
        printf("Offset Max: \t\t%d\n", param_val_max);
        EVT_CameraGetUInt32ParamMin(&camera, "Offset", &param_val_min);
        printf("Offset Min: \t\t%d\n", param_val_min);
        if (OFFSET_VAL >= param_val_min && OFFSET_VAL <= param_val_max)
        {
            EVT_CameraSetUInt32Param(&camera, "Offset", OFFSET_VAL);
            printf("Offset Set: \t\t%d\n", OFFSET_VAL);
        }
    }

    ReturnVal = EVT_CameraGetParamAttr(&camera, "AutoGain", &attribute);

    if (attribute.dataType == EDataTypeUnsupported)
    {
        printf("CameraGetParamAttr: Error\n");
        return ReturnVal;
    }
    else
    {
        EVT_CameraSetBoolParam(&camera, "AutoGain", FALSE);
        printf("AutoGain Set: \t\tDisabled.\n\n");
    }

    ReturnVal = EVT_CameraOpenStream(&camera);

    if (ReturnVal != SUCCESS)
    {
        printf("CameraOpenStream: Error\n");
        return ReturnVal;
    }

    evtFrame.size_x = width_max;
    evtFrame.size_y = height_max;
    evtFrame.pixel_type = GVSP_PIX_MONO8;
    EVT_AllocateFrameBuffer(&camera, &evtFrame, EVT_FRAME_BUFFER_ZERO_COPY);
    EVT_CameraQueueFrame(&camera, &evtFrame);


    // CAMARA 2

    ReturnVal = EVT_CameraOpen(&camera2, &deviceInfo[1]);

    if (ReturnVal != SUCCESS)
    {
        printf("Open Camera 2: \t\tError. Exiting program.\n");
        return ReturnVal;
    }
    else
    {
        printf("\t\tCAMERA 2 \n");
        printf("Open Camera 2: \t\tCamera Opened\n\n");
    }

    EVT_CameraGetUInt32ParamMax(&camera2, "Height", &height_max);
    EVT_CameraGetUInt32ParamMax(&camera2, "Width", &width_max);
    printf("Resolution: \t\t%d x %d\n", width_max, height_max);

    ReturnVal = EVT_CameraGetParamAttr(&camera2, "Gain", &attribute2);

    if (attribute2.dataType == EDataTypeUnsupported)
    {
        printf("CameraGetParamAttr: Error\n");
        return ReturnVal;
    }
    else
    {
        EVT_CameraGetUInt32ParamMax(&camera2, "Gain", &param_val_max);
        printf("Gain Max: \t\t%d\n", param_val_max);
        EVT_CameraGetUInt32ParamMin(&camera2, "Gain", &param_val_min);
        printf("Gain Min: \t\t%d\n", param_val_min);
        if (GAIN_VAL2 >= param_val_min && GAIN_VAL2 <= param_val_max)
        {
            EVT_CameraSetUInt32Param(&camera2, "Gain", GAIN_VAL2);
            printf("Gain Set: \t\t%d\n", GAIN_VAL2);
        }
    }

    ReturnVal = EVT_CameraGetParamAttr(&camera2, "Offset", &attribute2);

    if (attribute2.dataType == EDataTypeUnsupported)
    {
        printf("CameraGetParamAttr: Error\n");
        return ReturnVal;
    }
    else
    {
        EVT_CameraGetUInt32ParamMax(&camera2, "Offset", &param_val_max);
        printf("Offset Max: \t\t%d\n", param_val_max);
        EVT_CameraGetUInt32ParamMin(&camera2, "Offset", &param_val_min);
        printf("Offset Min: \t\t%d\n", param_val_min);
        if (OFFSET_VAL >= param_val_min && OFFSET_VAL <= param_val_max)
        {
            EVT_CameraSetUInt32Param(&camera2, "Offset", OFFSET_VAL);
            printf("Offset Set: \t\t%d\n", OFFSET_VAL);
        }
    }

    ReturnVal = EVT_CameraGetParamAttr(&camera2, "AutoGain", &attribute2);

    if (attribute2.dataType == EDataTypeUnsupported)
    {
        printf("CameraGetParamAttr: Error\n");
        return ReturnVal;
    }
    else
    {
        EVT_CameraSetBoolParam(&camera2, "AutoGain", FALSE);
        printf("AutoGain Set: \t\tEnabled.\n");
    }

    ReturnVal = EVT_CameraOpenStream(&camera2);
    if (ReturnVal != SUCCESS)
    {
        printf("CameraOpenStream: Error\n");
        return ReturnVal;
    }

    evtFrame2.size_x = width_max;
    evtFrame2.size_y = height_max;
    evtFrame2.pixel_type = GVSP_PIX_MONO8;
    EVT_AllocateFrameBuffer(&camera2, &evtFrame2, EVT_FRAME_BUFFER_ZERO_COPY);
    EVT_CameraQueueFrame(&camera2, &evtFrame2);

    unsigned int frames_recd = 0;
    // for (int frame_count = 0; frame_count < MAX_FRAMES; frame_count++)
    for (;;)

    {
        ReturnVal = EVT_CameraExecuteCommand(&camera, "AcquisitionStart");
        EVT_CameraExecuteCommand(&camera2, "AcquisitionStart");

        //Tell camera to start streaming
        if (ReturnVal != SUCCESS)
        {
            printf("CameraExecuteCommand: Error\n");
            return ReturnVal;
        }

        err = EVT_CameraGetFrame(&camera, &evtFrame, EVT_INFINITE);
        EVT_CameraGetFrame(&camera2, &evtFrame2, EVT_INFINITE);

        if (err)
            printf("EVT_CameraGetFrame Error!\n");
        else
            frames_recd++;

        EVT_CameraExecuteCommand(&camera, "AcquisitionStop");
        EVT_CameraExecuteCommand(&camera2, "AcquisitionStop");


        if (waitKey(10) == 27) break;

        view_camera(&camera,&evtFrame, &camera2, &evtFrame2);
        

}

}

void view_camera(CEmergentCamera* camera, CEmergentFrame* evtFrame, CEmergentCamera* camera2, CEmergentFrame* evtFrame2) {

    EVT_FrameSave(evtFrame, "D:/images/myimage_1.bmp", EVT_FILETYPE_BMP, EVT_ALIGN_NONE);
    EVT_FrameSave(evtFrame2, "D:/images/myimage_2.bmp", EVT_FILETYPE_BMP, EVT_ALIGN_NONE);

    EVT_CameraQueueFrame(camera, evtFrame);  //Now re-queue.
    EVT_CameraQueueFrame(camera2, evtFrame2);

    Mat fra;
    fra = imread("D:/images/myimage_1.bmp", IMREAD_COLOR);
   // resizeWindow("Camera 1", h, w);
    //namedWindow("Camera 1", WINDOW_NORMAL);
    //imshow("Camera 1", fra);
    Mat fra2;
    fra2 = imread("D:/images/myimage_2.bmp", IMREAD_COLOR);
    Mat fra33 = Mat::zeros(cv::Size(808, 606), CV_64FC1);
    imwrite("D:/images/myimage_3.bmp",fra33);
    Mat fra44 = Mat::zeros(cv::Size(808, 606), CV_64FC1);
    imwrite("D:/images/myimage_4.bmp",fra44);
    Mat fra3, fra4;
    fra3 = imread("D:/images/myimage_3.bmp", IMREAD_COLOR);
    fra4 = imread("D:/images/myimage_4.bmp", IMREAD_COLOR);



    showImages("Camaras", 2, 2, 646, { &fra, &fra2,&fra3,&fra4 }, 5);

   /* Mat fra2;
    fra2 = imread("D:/images/myimage_2.bmp", IMREAD_COLOR);
    resizeWindow("Camera 2", h, w);
    namedWindow("Camera 2", WINDOW_NORMAL);
    imshow("Camera 2", fra2);*/

    //waitKey(fps);


}


void showImages(const String& window_name, int rows, int cols, int size, std::initializer_list<const Mat*> images, int pad )
{

    int h = 1000;
    int w = 800;

    if (pad <= 0) pad = 0;

    int width = size * cols + ((cols + 1) * pad);
    int height = size * rows + ((rows + 1) * pad);

    Mat dst = Mat(height, width, CV_8UC3, Scalar::all(255));

    int x = 0, y = 0, cols_counter = 0, img_counter = 0;

    for (auto& img : images) {
        Mat roi = dst(Rect(x + pad, y + pad, size, size));

        getSquareImage(*img, roi, size);
        
        x += roi.cols + pad;


        if (++cols_counter == cols) {
            cols_counter = x = 0;
            y += roi.rows + pad;
        }

        if (++img_counter >= rows * cols) break;
    }
   
    int fps = 1000 / 25;
    resizeWindow(window_name, h, w);
    namedWindow(window_name, WINDOW_NORMAL);
    imshow(window_name, dst);
    waitKey(fps);

}


void getSquareImage(cv::InputArray img, cv::OutputArray dst, int size)
{
    if (size < 2) size = 2;
    int width = img.cols(), height = img.rows();

    cv::Mat square = dst.getMat();

    square.setTo(Scalar::all(0));

    int max_dim = (width >= height) ? width : height;
    float scale = ((float)size) / max_dim;

    cv::Rect roi;

    if (width >= height)
    {
        roi.width = size;
        roi.x = 0;
        roi.height = (int)(height * scale);
        roi.y = (size - roi.height) / 2;
    }
    else
    {
        roi.y = 0;
        roi.height = size;
        roi.width = (int)(width * scale);
        roi.x = (size - roi.width) / 2;
    }

    cv::resize(img, square(roi), roi.size());
}
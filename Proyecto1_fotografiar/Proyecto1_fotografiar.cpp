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
#define FRAME_RATE 30
#define FRAME_RATE_2 60

#define MAX_CAMERAS 4

#define TRUE  1
#define FALSE 0

#define GAIN_VAL      500
#define GAIN_VAL2      1000

#define OFFSET_VAL    0
#define AG_SET_VAL    512
#define AG_I_VAL      16

#define EXPOSURE_VAL 1000

char* next_token;

void view_camera(char const* numCam, const String& window_name, CEmergentCamera* camera, CEmergentFrame* evtFrame, int frameRate);
void configure_defaults(CEmergentCamera* camera);



int main()
{
    printf("Librerias cargadas con exito \n");
    cout << "Cargando Variable" << endl << endl;

    CEmergentCamera camera, camera2, camera3, camera4;
    struct EvtParamAttribute attribute, attribute2, attribute3, attribute4;
    int ReturnVal = SUCCESS;
    unsigned int param_val_max, param_val_min, param_val_inc;
    unsigned int height_max, width_max;
    unsigned int height_final, width_final;
    CEmergentFrame evtFrame, evtFrame2, evtFrame3, evtFrame4;
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

    configure_defaults(&camera);

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


    ///////////////
   /* EVT_CameraGetUInt32ParamMax(&camera, "Exposure", &param_val_max);
    printf("Exposure Max: \t\t%d\n", param_val_max);
    EVT_CameraGetUInt32ParamMin(&camera, "Exposure", &param_val_min);
    printf("Exposure Min: \t\t%d\n", param_val_min);
    if (EXPOSURE_VAL >= param_val_min && EXPOSURE_VAL <= param_val_max)
    {
        EVT_CameraSetUInt32Param(&camera, "Exposure", EXPOSURE_VAL);
        printf("Exposure Set: \t\t%d\n", EXPOSURE_VAL);
    }*/
    ////////////////

    ReturnVal = EVT_CameraOpenStream(&camera);

    if (ReturnVal != SUCCESS)
    {
        printf("CameraOpenStream: Error\n");
        return ReturnVal;
    }

    evtFrame.size_x = width_max;
    evtFrame.size_y = height_max;
    evtFrame.pixel_type = GVSP_PIX_BAYGB8;
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

    //////////////////////////

   /* EVT_CameraGetUInt32ParamMax(&camera2, "Exposure", &param_val_max);
    printf("Exposure Max: \t\t%d\n", param_val_max);
    EVT_CameraGetUInt32ParamMin(&camera2, "Exposure", &param_val_min);
    printf("Exposure Min: \t\t%d\n", param_val_min);
    if (EXPOSURE_VAL >= param_val_min && EXPOSURE_VAL <= param_val_max)
    {
        EVT_CameraSetUInt32Param(&camera2, "Exposure", EXPOSURE_VAL);
        printf("Exposure Set: \t\t%d\n", EXPOSURE_VAL);
    }*/

    ////////////////////////////

    ReturnVal = EVT_CameraOpenStream(&camera2);
    if (ReturnVal != SUCCESS)
    {
        printf("CameraOpenStream: Error\n");
        return ReturnVal;
    }

    evtFrame2.size_x = width_max;
    evtFrame2.size_y = height_max;
    evtFrame2.pixel_type = GVSP_PIX_BGR8;
    EVT_AllocateFrameBuffer(&camera2, &evtFrame2, EVT_FRAME_BUFFER_ZERO_COPY);
    EVT_CameraQueueFrame(&camera2, &evtFrame2);

    // CAMARA 3

    ReturnVal = EVT_CameraOpen(&camera3, &deviceInfo[2]);

    if (ReturnVal != SUCCESS)
    {
        printf("Open Camera 3: \t\tError. Exiting program.\n");
        return ReturnVal;
    }
    else
    {
        printf("\t\tCAMERA 3 \n");
        printf("Open Camera 3: \t\tCamera Opened\n\n");
    }

    EVT_CameraGetUInt32ParamMax(&camera3, "Height", &height_max);
    EVT_CameraGetUInt32ParamMax(&camera3, "Width", &width_max);
    printf("Resolution: \t\t%d x %d\n", width_max, height_max);

    ReturnVal = EVT_CameraGetParamAttr(&camera3, "Gain", &attribute3);

    if (attribute3.dataType == EDataTypeUnsupported)
    {
        printf("CameraGetParamAttr: Error\n");
        return ReturnVal;
    }
    else
    {
        EVT_CameraGetUInt32ParamMax(&camera3, "Gain", &param_val_max);
        printf("Gain Max: \t\t%d\n", param_val_max);
        EVT_CameraGetUInt32ParamMin(&camera3, "Gain", &param_val_min);
        printf("Gain Min: \t\t%d\n", param_val_min);
        if (GAIN_VAL2 >= param_val_min && GAIN_VAL2 <= param_val_max)
        {
            EVT_CameraSetUInt32Param(&camera3, "Gain", GAIN_VAL2);
            printf("Gain Set: \t\t%d\n", GAIN_VAL2);
        }
    }

    ReturnVal = EVT_CameraGetParamAttr(&camera3, "Offset", &attribute3);

    if (attribute2.dataType == EDataTypeUnsupported)
    {
        printf("CameraGetParamAttr: Error\n");
        return ReturnVal;
    }
    else
    {
        EVT_CameraGetUInt32ParamMax(&camera3, "Offset", &param_val_max);
        printf("Offset Max: \t\t%d\n", param_val_max);
        EVT_CameraGetUInt32ParamMin(&camera3, "Offset", &param_val_min);
        printf("Offset Min: \t\t%d\n", param_val_min);
        if (OFFSET_VAL >= param_val_min && OFFSET_VAL <= param_val_max)
        {
            EVT_CameraSetUInt32Param(&camera3, "Offset", OFFSET_VAL);
            printf("Offset Set: \t\t%d\n", OFFSET_VAL);
        }
    }

    ReturnVal = EVT_CameraGetParamAttr(&camera3, "AutoGain", &attribute3);

    if (attribute3.dataType == EDataTypeUnsupported)
    {
        printf("CameraGetParamAttr: Error\n");
        return ReturnVal;
    }
    else
    {
        EVT_CameraSetBoolParam(&camera3, "AutoGain", FALSE);
        printf("AutoGain Set: \t\tEnabled.\n");
    }

    //////////////////////////

   /* EVT_CameraGetUInt32ParamMax(&camera2, "Exposure", &param_val_max);
    printf("Exposure Max: \t\t%d\n", param_val_max);
    EVT_CameraGetUInt32ParamMin(&camera2, "Exposure", &param_val_min);
    printf("Exposure Min: \t\t%d\n", param_val_min);
    if (EXPOSURE_VAL >= param_val_min && EXPOSURE_VAL <= param_val_max)
    {
        EVT_CameraSetUInt32Param(&camera2, "Exposure", EXPOSURE_VAL);
        printf("Exposure Set: \t\t%d\n", EXPOSURE_VAL);
    }*/

    ////////////////////////////

    ReturnVal = EVT_CameraOpenStream(&camera3);
    if (ReturnVal != SUCCESS)
    {
        printf("CameraOpenStream: Error\n");
        return ReturnVal;
    }

    evtFrame3.size_x = width_max;
    evtFrame3.size_y = height_max;
    evtFrame3.pixel_type = GVSP_PIX_BGR8;
    EVT_AllocateFrameBuffer(&camera3, &evtFrame3, EVT_FRAME_BUFFER_ZERO_COPY);
    EVT_CameraQueueFrame(&camera3, &evtFrame3);

    //CAMARA 4
    ReturnVal = EVT_CameraOpen(&camera4, &deviceInfo[3]);

    if (ReturnVal != SUCCESS)
    {
        printf("Open Camera 4: \t\tError. Exiting program.\n");
        return ReturnVal;
    }
    else
    {
        printf("\t\tCAMERA 4 \n");
        printf("Open Camera 4: \t\tCamera Opened\n\n");
    }

    EVT_CameraGetUInt32ParamMax(&camera4, "Height", &height_max);
    EVT_CameraGetUInt32ParamMax(&camera4, "Width", &width_max);
    printf("Resolution: \t\t%d x %d\n", width_max, height_max);

    ReturnVal = EVT_CameraGetParamAttr(&camera4, "Gain", &attribute4);

    if (attribute4.dataType == EDataTypeUnsupported)
    {
        printf("CameraGetParamAttr: Error\n");
        return ReturnVal;
    }
    else
    {
        EVT_CameraGetUInt32ParamMax(&camera4, "Gain", &param_val_max);
        printf("Gain Max: \t\t%d\n", param_val_max);
        EVT_CameraGetUInt32ParamMin(&camera4, "Gain", &param_val_min);
        printf("Gain Min: \t\t%d\n", param_val_min);
        if (GAIN_VAL2 >= param_val_min && GAIN_VAL2 <= param_val_max)
        {
            EVT_CameraSetUInt32Param(&camera4, "Gain", GAIN_VAL2);
            printf("Gain Set: \t\t%d\n", GAIN_VAL2);
        }
    }

    ReturnVal = EVT_CameraGetParamAttr(&camera4, "Offset", &attribute4);

    if (attribute4.dataType == EDataTypeUnsupported)
    {
        printf("CameraGetParamAttr: Error\n");
        return ReturnVal;
    }
    else
    {
        EVT_CameraGetUInt32ParamMax(&camera4, "Offset", &param_val_max);
        printf("Offset Max: \t\t%d\n", param_val_max);
        EVT_CameraGetUInt32ParamMin(&camera4, "Offset", &param_val_min);
        printf("Offset Min: \t\t%d\n", param_val_min);
        if (OFFSET_VAL >= param_val_min && OFFSET_VAL <= param_val_max)
        {
            EVT_CameraSetUInt32Param(&camera4, "Offset", OFFSET_VAL);
            printf("Offset Set: \t\t%d\n", OFFSET_VAL);
        }
    }

    ReturnVal = EVT_CameraGetParamAttr(&camera4, "AutoGain", &attribute4);

    if (attribute4.dataType == EDataTypeUnsupported)
    {
        printf("CameraGetParamAttr: Error\n");
        return ReturnVal;
    }
    else
    {
        EVT_CameraSetBoolParam(&camera4, "AutoGain", FALSE);
        printf("AutoGain Set: \t\tEnabled.\n");
    }

    //////////////////////////

   /* EVT_CameraGetUInt32ParamMax(&camera2, "Exposure", &param_val_max);
    printf("Exposure Max: \t\t%d\n", param_val_max);
    EVT_CameraGetUInt32ParamMin(&camera2, "Exposure", &param_val_min);
    printf("Exposure Min: \t\t%d\n", param_val_min);
    if (EXPOSURE_VAL >= param_val_min && EXPOSURE_VAL <= param_val_max)
    {
        EVT_CameraSetUInt32Param(&camera2, "Exposure", EXPOSURE_VAL);
        printf("Exposure Set: \t\t%d\n", EXPOSURE_VAL);
    }*/

    ////////////////////////////

    ReturnVal = EVT_CameraOpenStream(&camera4);
    if (ReturnVal != SUCCESS)
    {
        printf("CameraOpenStream: Error\n");
        return ReturnVal;
    }

    evtFrame4.size_x = width_max;
    evtFrame4.size_y = height_max;
    evtFrame4.pixel_type = GVSP_PIX_BGR8;
    EVT_AllocateFrameBuffer(&camera4, &evtFrame4, EVT_FRAME_BUFFER_ZERO_COPY);
    EVT_CameraQueueFrame(&camera4, &evtFrame4);
    

    EVT_CameraSetUInt32Param(&camera, "FrameRate", FRAME_RATE);
    EVT_CameraSetUInt32Param(&camera2, "FrameRate", FRAME_RATE);
    EVT_CameraSetUInt32Param(&camera3, "FrameRate", FRAME_RATE);
    EVT_CameraSetUInt32Param(&camera4, "FrameRate", FRAME_RATE);

    /*printf("Trigger Mode: \t\tContinuous, HardwareTrigger, GPI4\n");
    printf("Grabbing Frames...\n");

    EVT_CameraSetEnumParam(&camera, "TriggerMode", "On");
    //pagina 9 attribute_manual
    EVT_CameraSetEnumParam(&camera, "TriggerSource", "Hardware");
    //pagina 10 attribute_manual

    //Set the GPI hardware triggering mode to use GPI_4 and select rising edge to start exp and falling edge 
    //to end exposure. Error check omitted for clarity.
    EVT_CameraSetEnumParam(&camera, "GPI_Start_Exp_Mode", "GPI_4");
    EVT_CameraSetEnumParam(&camera, "GPI_Start_Exp_Event", "Rising_Edge");
    EVT_CameraSetEnumParam(&camera, "GPI_End_Exp_Mode", "GPI_4");
    EVT_CameraSetEnumParam(&camera, "GPI_End_Exp_Event", "Falling_Edge");

    EVT_CameraSetEnumParam(&camera, "GPO_0_Mode", "Test_Generator");
    EVT_CameraSetUInt32Param(&camera, "TG_Frame_Time", 33333); //30fps
    EVT_CameraSetUInt32Param(&camera, "TG_High_Time", 1000);   //1000us

    EVT_CameraSetUInt32Param(&camera, "Trigger_Delay", 1000);   //1000us*/


  EVT_CameraSetEnumParam(&camera2, "TriggerMode", "On");
    //pagina 9 attribute_manual
    EVT_CameraSetEnumParam(&camera2, "TriggerSource", "Hardware");
    //pagina 10 attribute_manual

    //Set the GPI hardware triggering mode to use GPI_4 and select rising edge to start exp and falling edge 
    //to end exposure. Error check omitted for clarity.
    EVT_CameraSetEnumParam(&camera2, "GPI_Start_Exp_Mode", "GPI_4");
    EVT_CameraSetEnumParam(&camera2, "GPI_Start_Exp_Event", "Rising_Edge");
    EVT_CameraSetEnumParam(&camera2, "GPI_End_Exp_Mode", "GPI_4");
    EVT_CameraSetEnumParam(&camera2, "GPI_End_Exp_Event", "Falling_Edge");

    EVT_CameraSetEnumParam(&camera2, "GPO_0_Mode", "Test_Generator");
    EVT_CameraSetUInt32Param(&camera2, "TG_Frame_Time", 33333); //30fps
    EVT_CameraSetUInt32Param(&camera2, "TG_High_Time", 1000);   //1000us

    EVT_CameraSetUInt32Param(&camera, "Trigger_Delay", 1000);   //1000us
  

    unsigned int frames_recd = 0;
     int MAX_FRAMES = 500;

  for  (int frames = 0; frames < MAX_FRAMES; frames++)
//for (;;)
    {

    
        char first[] = "D:/images/myimage_1.bmp";
        char second[] = "D:/images/myimage_2.bmp";
        char three[] = "D:/images/myimage_3.bmp";
        char four[] = "D:/images/myimage_4.bmp";

        ReturnVal = EVT_CameraExecuteCommand(&camera, "AcquisitionStart");
        EVT_CameraExecuteCommand(&camera2, "AcquisitionStart");
        EVT_CameraExecuteCommand(&camera3, "AcquisitionStart");
        EVT_CameraExecuteCommand(&camera4, "AcquisitionStart");

        //Tell camera to start streaming
        if (ReturnVal != SUCCESS)
        {
            printf("CameraExecuteCommand: Error\n");
            return ReturnVal;
        }

        err = EVT_CameraGetFrame(&camera, &evtFrame, EVT_INFINITE);
        EVT_CameraGetFrame(&camera2, &evtFrame2, EVT_INFINITE);
        EVT_CameraGetFrame(&camera3, &evtFrame3, EVT_INFINITE);
        EVT_CameraGetFrame(&camera4, &evtFrame4, EVT_INFINITE);

        if (err)
            printf("EVT_CameraGetFrame Error!\n");
        else
            frames_recd++;

        EVT_CameraExecuteCommand(&camera, "AcquisitionStop");
        EVT_CameraExecuteCommand(&camera2, "AcquisitionStop");
        EVT_CameraExecuteCommand(&camera3, "AcquisitionStop");
        EVT_CameraExecuteCommand(&camera4, "AcquisitionStop");

      //if(waitKey(10) == 27) break;

       /*view_camera(first,"Camara 1",&camera,&evtFrame,FRAME_RATE);
       view_camera(second,"Camara 2",&camera2, &evtFrame2,FRAME_RATE);
       view_camera(three, "Camara 3", &camera3, &evtFrame3, FRAME_RATE);
       view_camera(four, "Camara 4", &camera4, &evtFrame4, FRAME_RATE);
       */


       sprintf_s(filename, "D:/files/myimage_%d.tif", frames_recd);
       EVT_FrameSave(&evtFrame, filename, EVT_FILETYPE_TIF, EVT_ALIGN_NONE);
       EVT_CameraQueueFrame(&camera, &evtFrame);  //Now re-queue.
       sprintf_s(filename, "D:/files2/myimage_%d.tif", frames_recd);
       EVT_FrameSave(&evtFrame2, filename, EVT_FILETYPE_TIF, EVT_ALIGN_NONE);
       EVT_CameraQueueFrame(&camera2, &evtFrame2);  //Now re-queue.

       sprintf_s(filename, "D:/files3/myimage_%d.tif", frames_recd);
       EVT_FrameSave(&evtFrame3, filename, EVT_FILETYPE_TIF, EVT_ALIGN_NONE);
       EVT_CameraQueueFrame(&camera3, &evtFrame3);  //Now re-queue.
       sprintf_s(filename, "D:/files4/myimage_%d.tif", frames_recd);
       EVT_FrameSave(&evtFrame4, filename, EVT_FILETYPE_TIF, EVT_ALIGN_NONE);
       EVT_CameraQueueFrame(&camera4, &evtFrame4);  //Now re-queue.

       printf(".");
       
    }
}





void view_camera(char const* path, const String& window_name, CEmergentCamera* camera, CEmergentFrame* evtFrame, int frameRate) {

    short h = 646;
    short w = 485;

    EVT_FrameSave(evtFrame, path, EVT_FILETYPE_BMP, EVT_ALIGN_NONE);
    EVT_CameraQueueFrame(camera, evtFrame);  //Now re-queue.

    int fps = 1000 / frameRate;

    Mat fra;

    fra = imread(path, IMREAD_COLOR);
    resizeWindow(window_name, h, w);
    namedWindow(window_name, WINDOW_NORMAL);
    imshow(window_name, fra);

    waitKey(fps);


}


void configure_defaults(CEmergentCamera* camera)
{
    unsigned int width_max, height_max, param_val_max;
    const unsigned long enumBufferSize = 1000;
    unsigned long enumBufferSizeReturn = 0;
    char enumBuffer[enumBufferSize];

    //Order is important as param max/mins get updated.
    EVT_CameraGetEnumParamRange(camera, "PixelFormat", enumBuffer, enumBufferSize, &enumBufferSizeReturn);
    char* enumMember = strtok_s(enumBuffer, ",", &next_token);
    EVT_CameraSetEnumParam(camera, "PixelFormat", enumMember);

    EVT_CameraSetUInt32Param(camera, "FrameRate", 30);

    EVT_CameraSetUInt32Param(camera, "OffsetX", 0);
    EVT_CameraSetUInt32Param(camera, "OffsetY", 0);

    EVT_CameraGetUInt32ParamMax(camera, "Width", &width_max);
    EVT_CameraSetUInt32Param(camera, "Width", width_max);

    EVT_CameraGetUInt32ParamMax(camera, "Height", &height_max);
    EVT_CameraSetUInt32Param(camera, "Height", height_max);

    EVT_CameraSetEnumParam(camera, "AcquisitionMode", "Continuous");
    EVT_CameraSetUInt32Param(camera, "AcquisitionFrameCount", 1);
    EVT_CameraSetEnumParam(camera, "TriggerSelector", "AcquisitionStart");
    EVT_CameraSetEnumParam(camera, "TriggerMode", "Off");
    EVT_CameraSetEnumParam(camera, "TriggerSource", "Software");
    EVT_CameraSetEnumParam(camera, "BufferMode", "Off");
    EVT_CameraSetUInt32Param(camera, "BufferNum", 0);

    EVT_CameraGetUInt32ParamMax(camera, "GevSCPSPacketSize", &param_val_max);
    EVT_CameraSetUInt32Param(camera, "GevSCPSPacketSize", param_val_max);

    EVT_CameraSetUInt32Param(camera, "Gain", 256);
    EVT_CameraSetUInt32Param(camera, "Offset", 0);

    EVT_CameraSetBoolParam(camera, "LUTEnable", false);
    EVT_CameraSetBoolParam(camera, "AutoGain", false);
}

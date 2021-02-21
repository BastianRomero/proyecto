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

#include <iomanip> 
#include <sstream>
#include <thread> 

#include<windows.h>

using namespace std;
using namespace Emergent;
using namespace cv;

#define CAMERAS 1
#define MAX_CAMERAS 6
#define BUFF 1000
#define SUCCESS 0
#define FRAME_RATE 30

#define TRUE  1
#define FALSE 0

#define GAIN_VAL      5000

#define AG_SET_VAL    512
#define AG_I_VAL      16

#define EXPOSURE_VAL 30000

char* next_token;

void view_camera(char const* numCam, const String& window_name, CEmergentCamera* camera, CEmergentFrame* evtFrame, int frame_rate);
void configure_defaults(CEmergentCamera* camera);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow) {

    MessageBox(HWND_DESKTOP, _T("Hola"), _T("Hello"), MB_OK);

    return 0;
}

int main()
{

    printf("Librerias cargadas con exito \n");
    cout << "Cargando Variable" << endl << endl;
    unsigned int cam = 0;
    CEmergentCamera camera[CAMERAS];
    struct EvtParamAttribute attribute;
    int ReturnVal = SUCCESS;
    unsigned int param_val_max, param_val_min;
    unsigned int height_max, width_max;
    unsigned int height, width;
    unsigned int frame_rate_max, frame_rate_min, frame_rate_inc;
    int frame_rate = 0;
    int temp = 0;
    CEmergentFrame evtFrame[BUFF];
    CEmergentFrame evtFrame2[BUFF];
    CEmergentFrame evtFrame3[BUFF];
    char filename[100];
    struct GigEVisionDeviceInfo deviceInfo[MAX_CAMERAS];
    unsigned int count;
    EVT_ERROR err = EVT_SUCCESS;

    unsigned int listcam_buf_size = MAX_CAMERAS;

    EVT_ListDevices(deviceInfo, &listcam_buf_size, &count);

    for (cam = 0; cam < CAMERAS; cam++)
    {
        ReturnVal = EVT_CameraOpen(&camera[cam], &deviceInfo[cam]);
        if (ReturnVal != SUCCESS)
        {
            printf("Open Camera: \t\tError. Exiting program.\n");
            return ReturnVal;
        }
        else
        {
            printf("Open Camera: \t\tCamera Opened: %s\n", deviceInfo[cam].modelName);
        }

        ReturnVal = EVT_CameraGetInt32Param(&camera[cam],"SensTemp",&temp);

        if (ReturnVal != SUCCESS)
        {
            printf("Open Camera: \t\tError. Exiting program.\n");
            return ReturnVal;
        }
        else
        {
            printf("El sensor %d esta a: %d grados. \n", cam + 1, temp);
        }
    }
    printf("\n");

    printf("Ingrese la cantidad de frames \n");
    cin >> frame_rate;
    printf("Se han ingresado %d frames \n", frame_rate);

    for (cam = 0; cam < CAMERAS; cam++)
    {
        printf("Camera %d:\n", cam+1);

        configure_defaults(&camera[cam]);
        //configure_now(&camera[cam]);

        //Get resolution.
        
        // No sirve -- EVT_CameraSetBoolParam(&camera[cam], "FlipX", false);
        // EVT_CameraSetEnumParam(&camera[cam], "SubSample", "8x8");
        EVT_CameraSetUInt32Param(&camera[cam], "Height", 440);
        EVT_CameraSetUInt32Param(&camera[cam], "Width", 576);

        EVT_CameraGetUInt32Param(&camera[cam], "Height", &height_max);
        EVT_CameraGetUInt32Param(&camera[cam], "Width", &width_max);
        printf("Resolution: \t\t%d x %d\n", width_max, height_max);

       /* EVT_CameraGetUInt32ParamMin(&camera[cam], "Width", &width_min);
        printf("Width Min: \t\t%d\n", width_min);
        EVT_CameraGetUInt32ParamMin(&camera[cam], "Height", &height_min);
        printf("Height Min: \t\t%d\n", height_min);*/

        //Frame rate
        EVT_CameraGetUInt32ParamMax(&camera[cam], "FrameRate", &frame_rate_max);
        printf("FrameRate Max: \t\t%d\n", frame_rate_max);

        //frame_rate = frame_rate_max;
       
        EVT_CameraSetUInt32Param(&camera[cam], "FrameRate", frame_rate);
        printf("FrameRate Set: \t\t%d\n", frame_rate);

        ReturnVal = EVT_CameraGetParamAttr(&camera[cam], "Gain", &attribute);

        if (attribute.dataType == EDataTypeUnsupported)
        {
            printf("CameraGetParamAttr: Error\n");
            return ReturnVal;
        }
        else
        {
            EVT_CameraGetUInt32ParamMax(&camera[cam], "Gain", &param_val_max);
            printf("Gain Max: \t\t%d\n", param_val_max);
            EVT_CameraGetUInt32ParamMin(&camera[cam], "Gain", &param_val_min);
            printf("Gain Min: \t\t%d\n", param_val_min);
            if (GAIN_VAL >= param_val_min && GAIN_VAL <= param_val_max)
            {
                EVT_CameraSetUInt32Param(&camera[cam], "Gain", GAIN_VAL);
                printf("Gain Set: \t\t%d\n", GAIN_VAL);
            }
        }
        ReturnVal = EVT_CameraGetParamAttr(&camera[cam], "AutoGain", &attribute);

        if (attribute.dataType == EDataTypeUnsupported)
        {
            printf("CameraGetParamAttr: Error\n");
            return ReturnVal;
        }
        else
        {
            EVT_CameraSetBoolParam(&camera[cam], "AutoGain", FALSE);
            printf("AutoGain Set: \t\tDisabled.\n\n");
        }

        EVT_CameraGetUInt32ParamMax(&camera[cam], "Exposure", &param_val_max);
        printf("Exposure Max: \t\t%d\n", param_val_max);
        EVT_CameraGetUInt32ParamMin(&camera[cam], "Exposure", &param_val_min);
        printf("Exposure Min: \t\t%d\n", param_val_min);
        if (EXPOSURE_VAL >= param_val_min && EXPOSURE_VAL <= param_val_max)
        {
            EVT_CameraSetUInt32Param(&camera[cam], "Exposure", EXPOSURE_VAL);
            printf("Exposure Set: \t\t%d\n", EXPOSURE_VAL);
        }
        printf("\n");
    }

    printf("Configuracion Terminada \n");

    for (cam = 0; cam < CAMERAS; cam++) {

        ReturnVal = EVT_CameraOpenStream(&camera[cam]);

        if (ReturnVal != SUCCESS)
        {
            printf("CameraOpenStream: Error\n");
            return ReturnVal;
        }

        evtFrame[cam].size_x = 576;
        evtFrame[cam].size_y = 440;
        evtFrame[cam].pixel_type = GVSP_PIX_BAYGB8;
        EVT_AllocateFrameBuffer(&camera[cam], &evtFrame[cam], EVT_FRAME_BUFFER_ZERO_COPY);
        EVT_CameraQueueFrame(&camera[cam], &evtFrame[cam]);

    }

   
    unsigned int frames_recd = 0;
    int MAX_FRAMES = 1000;

    printf("1 --> Ver cameras \n");
    printf("2 --> Capturar fotogramas\n");
    printf("3 --> Salir del programa\n");

    int option = 0;
    cin >> option;

    if (option == 1) {
        for (;;){
            char nameCamera[20];
            char path[100];
       
            for (cam = 0; cam < CAMERAS; cam++) {
                EVT_CameraQueueFrame(&camera[cam], &evtFrame[cam]);
                ReturnVal = EVT_CameraExecuteCommand(&camera[cam], "AcquisitionStart");
                if (ReturnVal != SUCCESS)
                {
                    printf("CameraExecuteCommand: Error\n");
                    return ReturnVal;
                }
                err = EVT_CameraGetFrame(&camera[cam], &evtFrame[cam], EVT_INFINITE);
                if (err)
                    printf("EVT_CameraGetFrame Error!\n");
                else
                    frames_recd++;

                EVT_CameraExecuteCommand(&camera[cam], "AcquisitionStop");
            }


            if (waitKey(10) == 27) {
                for (cam = 0; cam < CAMERAS; cam++)
                {
                    EVT_CameraClose(&camera[cam]);
                    printf("\nClose Camera: \t\tCamera Closed");
                }
                printf("\n");
                break;
            }

            for (cam = 0; cam < CAMERAS; cam++) {
                sprintf_s(path, "D:/images/myimage_%d.bmp", cam + 1);
                sprintf_s(nameCamera, "Camara_%d", cam + 1);
                view_camera(path, nameCamera, &camera[cam], &evtFrame[cam],frame_rate);
            }
        }
        printf("SUCCESS");
    }

    if (option == 2) {
        printf("Trigger Mode: \t\tContinuous, HardwareTrigger, GPI4\n");
        printf("Grabbing Frames...\n");



        // EVT_CameraSetEnumParam(&camera[0], "TriggerSelector", "AcquisitionStart");
        //_CameraSetEnumParam(&camera[0], "AcquisitionMode", "Continuous");
         //EVT_CameraSetUInt32Param(&camera[0], "AcquisitionFrameCount", MAX_FRAMES);
        // EVT_CameraSetEnumParam(&camera[0], "TriggerMode", "On");
         //EVT_CameraSetEnumParam(&camera[0], "TriggerSource", "Hardware");

         //EVT_CameraSetEnumParam(&camera[0], "BufferMode", "On");
         //EVT_CameraSetUInt32Param(&camera[0], "BufferNum", 30);


        printf("Iniciando captura \n");

        for (int frames = 0; frames < MAX_FRAMES; frames++) {

                evtFrame[frames].size_x = 576;
                evtFrame[frames].size_y = 440;
                evtFrame[frames].pixel_type = GVSP_PIX_BAYGB8;
                EVT_AllocateFrameBuffer(&camera[0], &evtFrame[frames], EVT_FRAME_BUFFER_ZERO_COPY);
                EVT_CameraQueueFrame(&camera[0], &evtFrame[frames]);

                for (cam = 0; cam < CAMERAS; cam++) {

                    ReturnVal = EVT_CameraExecuteCommand(&camera[cam], "AcquisitionStart");
                    if (ReturnVal != SUCCESS)
                    {
                        printf("CameraExecuteCommand: Error\n");
                        return ReturnVal;
                    }
                    err = EVT_CameraGetFrame(&camera[cam], &evtFrame[frames], EVT_INFINITE);
                    if (err)
                        printf("EVT_CameraGetFrame Error!\n");
                    else
                        frames_recd++;

                    EVT_CameraExecuteCommand(&camera[cam], "AcquisitionStop");

                }
             //   printf("%d", frames);
        }


    /*    for (int frames = 0; frames < 200; frames++) {

            evtFrame2[frames].size_x = 576;
            evtFrame2[frames].size_y = 440;
            evtFrame2[frames].pixel_type = GVSP_PIX_BAYGB8;
            EVT_AllocateFrameBuffer(&camera[0], &evtFrame2[frames], EVT_FRAME_BUFFER_ZERO_COPY);
            EVT_CameraQueueFrame(&camera[0], &evtFrame2[frames]);

            for (cam = 0; cam < CAMERAS; cam++) {

                ReturnVal = EVT_CameraExecuteCommand(&camera[cam], "AcquisitionStart");
                if (ReturnVal != SUCCESS)
                {
                    printf("CameraExecuteCommand: Error\n");
                    return ReturnVal;
                }
                err = EVT_CameraGetFrame(&camera[cam], &evtFrame2[frames], EVT_INFINITE);
                if (err)
                    printf("EVT_CameraGetFrame Error!\n");
                else
                    frames_recd++;

                EVT_CameraExecuteCommand(&camera[cam], "AcquisitionStop");               
            }
        }
        

        for (int frames = 0; frames < 200; frames++) {

            evtFrame3[frames].size_x = 576;
            evtFrame3[frames].size_y = 440;
            evtFrame3[frames].pixel_type = GVSP_PIX_BAYGB8;
            EVT_AllocateFrameBuffer(&camera[0], &evtFrame3[frames], EVT_FRAME_BUFFER_ZERO_COPY);
            EVT_CameraQueueFrame(&camera[0], &evtFrame3[frames]);

            for (cam = 0; cam < CAMERAS; cam++) {

                ReturnVal = EVT_CameraExecuteCommand(&camera[cam], "AcquisitionStart");
                if (ReturnVal != SUCCESS)
                {
                    printf("CameraExecuteCommand: Error\n");
                    return ReturnVal;
                }
                err = EVT_CameraGetFrame(&camera[cam], &evtFrame3[frames], EVT_INFINITE);
                if (err)
                    printf("EVT_CameraGetFrame Error!\n");
                else
                    frames_recd++;

                EVT_CameraExecuteCommand(&camera[cam], "AcquisitionStop");
            }
        }*/

    
        cam = 0;

        for (int frames = 0; frames < MAX_FRAMES; frames++) {
            sprintf_s(filename, "C:/Users/OptoLab/Documents/camera/%dfps_%d/camera%d_%d.bmp", frame_rate, cam + 1, cam + 1, frames+1);
            EVT_FrameSave(&evtFrame[frames], filename, EVT_FILETYPE_BMP, EVT_ALIGN_NONE);
            EVT_ReleaseFrameBuffer(&camera[cam], &evtFrame[frames]);

                }

      /* for (int frames = 0; frames < MAX_FRAMES; frames++) {
            sprintf_s(filename, "C:/Users/OptoLab/Documents/camera/%dfps_%d/camera%d_%d.bmp", frame_rate, cam + 1, cam + 1, frames+200);
            EVT_FrameSave(&evtFrame2[frames], filename, EVT_FILETYPE_BMP, EVT_ALIGN_NONE);
            EVT_ReleaseFrameBuffer(&camera[0], &evtFrame2[frames]);
        }

       for (int frames = 0; frames < MAX_FRAMES; frames++) {
           sprintf_s(filename, "C:/Users/OptoLab/Documents/camera/%dfps_%d/camera%d_%d.bmp", frame_rate, cam + 1, cam + 1, frames + 400);
           EVT_FrameSave(&evtFrame3[frames], filename, EVT_FILETYPE_BMP, EVT_ALIGN_NONE);
           EVT_ReleaseFrameBuffer(&camera[0], &evtFrame3[frames]);
       }*/
        printf("Imagenes guardadas");
  }

    if(option == 3)
    {
        printf("Programa Finalizado");
    }

}

void view_camera(char const* path, const String& window_name, CEmergentCamera* camera, CEmergentFrame* evtFrame, int frame_rate) {
    
    Mat fra;
    int fps = 1000 / frame_rate;

    short h = 646;
    short w = 485;

    EVT_FrameSave(evtFrame, path, EVT_FILETYPE_BMP, EVT_ALIGN_NONE);

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

    EVT_CameraGetEnumParamRange(camera, "PixelFormat", enumBuffer, enumBufferSize, &enumBufferSizeReturn);
    char* enumMember = strtok_s(enumBuffer, ",", &next_token);
    EVT_CameraSetEnumParam(camera, "PixelFormat", enumMember);

    EVT_CameraSetUInt32Param(camera, "FrameRate", 24);

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

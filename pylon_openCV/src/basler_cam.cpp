#include "basler_cam.h"



basler_cam::basler_cam(std::string calibrationPath)
{

}

basler_cam::basler_cam(std::string calibrationPath, int exposure)
{
    path = calibrationPath;
    myExposure = exposure;
}

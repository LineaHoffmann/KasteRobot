#ifndef XCONTROLLER_H
#define XCONTROLLER_H

#ifndef LOG_DEFINES
#define LOG_DEFINES 1
#define logstd wxLogMessage
#define logwar wxLogWarning
#define logerr wxLogError
#endif

#include <utility>
#include <mutex>

#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"

#include "wx/wx.h"

#include "xbaslercam.hpp"
#include "xurcontrol.hpp"
#include "xgripperclient.hpp"
#include "xexceptions.hpp"

// Not sure where to place this, both xController and cMain need access
enum GUI_BUTTONS {
    ROBOT_CONNECT,
    ROBOT_DISCONNECT,
    CAMERA_CONNECT,
    CAMERA_DISCONNECT,
    CAMERA_STOP
};

class xController
{
public:
    xController();
    ~xController();

    void setCamera(std::shared_ptr<xBaslerCam> camera);
    void setRobot(std::shared_ptr<xUrControl> robot);
    void setGripper(std::shared_ptr<xGripperClient> gripper);


    // ********* //
    // GUI CALLS //
    // ********* //
    // For GUI when grabbing from camera
    bool hasNewImage();
    cv::Mat getImage();
    // For GUI when updating info tree
    void fillInfo(struct treeInfo); // TODO: Perhaps a struct to keep all info? Who knows
    // For GUI button calls. Don't actually do stuff here, only post the tasks!
    // The GUI thread does not like working
    void guiButtonPressed(GUI_BUTTONS); // A little enum to keep buttons in? See top

private:
    std::shared_ptr<xBaslerCam> mCamera;
    std::shared_ptr<xUrControl> mRobot;
    std::shared_ptr<xGripperClient> mGripper;

    std::mutex mMtx;
};

#endif // XCONTROLLER_H

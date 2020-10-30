#ifndef XCONTROLLER_H
#define XCONTROLLER_H

#ifndef LOG_DEFINES
#define LOG_DEFINES 1
#define logstd wxLogMessage
#define logwar wxLogWarning
#define logerr wxLogError
#endif

#include <utility>
#include <type_traits>
#include <mutex>


#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"

#include "wx/wx.h"

#include "xbaslercam.hpp"
#include "xurcontrol.hpp"
#include "xgripperclient.hpp"
#include "xexceptions.hpp"
#include "../gui/cidbindings.hpp"
#include "xcollisiondetector.h"

#include "xmath.hpp"

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
    // The GUI thread does not like working in here
    template<typename T = bool>
    void guiButtonPressed(BINDING_ID id, T data = false)
    {
        if (id == BINDING_ID::ID_BTN_TESTING_XYZ_VVA) {
            logstd("XYZ_VVA from xController");
            try {
                if constexpr (std::is_same_v<T, std::array<double,7>>) {
                    static_cast<std::array<double,7>>(data);
                    std::array<double, 3> xyz{ data[0], data[1], data[2] };
                    std::array<double, 2> aLims{ data[3], data[4] };
                    std::array<double, 2> vLims{ data[5], data[6] };
                    std::array<double, 3> res = xMath::distance3d_to_v0_xyAngle_czAngle(xyz, aLims, vLims);
                    std::stringstream s; s << "V0: " << res[0] << " | Theta_xy: " << res[1] << " | Theta_cz: " << res[2];
                    logstd(s.str().c_str());
                    return;
                }
            } catch ( const std::exception &e ) {
                logwar(e.what());
                return;
            }
        }
        throw x_err::error(x_err::what::NO_IMPLEMENTATION);
    }

private:
    std::shared_ptr<xBaslerCam> mCamera;
    std::shared_ptr<xUrControl> mRobot;
    std::shared_ptr<xGripperClient> mGripper;
    std::shared_ptr<xCollisionDetector> mCollisionDetector;

    std::mutex mMtx;
};

#endif // XCONTROLLER_H

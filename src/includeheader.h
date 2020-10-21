#ifndef INCLUDEHEADER_H
#define INCLUDEHEADER_H

/*
 *  All libraries and definitions can be
 *  collected here for readability's sake.
 */

// Redefining logging function names
// cout is still legal, but will only show in terminal
#ifndef LOG_DEFINES
#define LOG_DEFINES 1
#define logstd wxLogMessage
#define logwar wxLogWarning
#define logerr wxLogError
#endif

// Includes from standard libraries
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <iostream>
#include <cmath>
#include <exception>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

// wxWidgets includes
#include "wx/wx.h"
#include "wx/app.h"
#include "wx/frame.h"
#include "wx/menu.h"
#include "wx/sizer.h"
#include "wx/splitter.h"
#include "wx/textctrl.h"
#include "wx/notebook.h"
#include "wx/treelist.h"
#include "wx/aboutdlg.h"
#include "wx/artprov.h"

// A few sample images for wxWidgets
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../resources/sample.xpm"
#endif

// OpenCV includes
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"

// Pylon includes
#include "pylon/PylonIncludes.h"

// UR RTDE includes
#include "ur_rtde/script_client.h"
#include "ur_rtde/rtde_control_interface.h"
#include "ur_rtde/rtde_receive_interface.h"

// URStruct include
#include "logic/xur_control.h"

//Exceptions
#include "logic/xrobotexceptions.h"


#endif // INCLUDEHEADER_H

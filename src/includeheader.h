#ifndef INCLUDEHEADER_H
#define INCLUDEHEADER_H

/*
 *  All libraries and definitions can be
 *  collected here for readability's sake.
 */

// Redefining logging function names
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
    #include "gui/sample.xpm"
#endif

// OpenCV and Pylon includes
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/types_c.h"       // Noooo (for Term Criteria defines)
#include "opencv2/calib3d/calib3d_c.h"  // More noooo (for Calib 3D defines)
//#include "pylon/PylonIncludes.h"
// The types_c.h and calib3d_c.h are not ideal. Very old OpenCV indeed. Also not C++ ಠ_ಠ
// TODO: Same defines should (are) be available in the .hpp versions of those headers (new names though)



#endif // INCLUDEHEADER_H

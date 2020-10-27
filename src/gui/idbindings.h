#ifndef IDBINDINGS_H
#define IDBINDINGS_H
#pragma once

#endif // IDBINDINGS_H

enum BINDING_ID {
    ID_TIMER_VIEW1_UPDATE,      // Updating for view 1
    ID_TIMER_VIEW2_UPDATE,      // Not used at the moment
    ID_TIMER_INFOTREE_UPDATE,   // Updating the information tree

    ID_MENU_SAVE_LOG,           // Menu buttons
    ID_MENU_SAVE_SNAPSHOT,
    ID_MENU_EXIT,
    ID_MENU_ABOUT,

    ID_BTN_ROBOT_CONNECT,       // Robot Notebook panel buttons
    ID_BTN_ROBOT_DISCONNECT,
    ID_BTN_ROBOT_UPDATE,
    ID_BTN_ROBOT_SEND_CMD,
    ID_BTN_ROBOT_SEND_POS,

    ID_BTN_GRIPPER_CONNECT,     // Gripper Notebook panel buttons
    ID_BTN_GRIPPER_DISCONNECT,

    ID_BTN_CAMERA_CONNECT,      // Camera Notebook panel buttons
    ID_BTN_CAMERA_DISCONNECT,

    ID_BTN_DATABASE_CONNECT,    // Database Notebook panel buttons
    ID_BTN_DATABASE_DISCONNECT
};

#ifndef PHIDGETPOLLINGDEVICE_H
#define PHIDGETPOLLINGDEVICE_H

#include "phidget21.h"

class MainWindow;

class PhidgetPollingDevice
{
public:
    PhidgetPollingDevice(MainWindow *mainWindow = 0);
    bool init();
//    int AttachHandler(CPhidgetHandle handle, void *userptr);
//    int DetachHandler(CPhidgetHandle handle, void *userptr);
//    int ErrorHandler(CPhidgetHandle handle, void *userptr,
//                     int ErrorCode, const char *unknown);

private:
    MainWindow *m_mainWindow;
    CPhidgetInterfaceKitHandle m_handle;
};

#endif // PHIDGETPOLLINGDEVICE_H

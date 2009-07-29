#ifndef PHIDGETPOLLINGDEVICE_H
#define PHIDGETPOLLINGDEVICE_H

#include "phidget21.h"

class MainWindow;

class PhidgetPollingDevice
{
public:
    PhidgetPollingDevice(MainWindow *mainWindow = 0);
    ~PhidgetPollingDevice();
    bool init();
    int pollDevice(int index);

private:
    void displayStats();

    MainWindow *m_mainWindow;
    CPhidgetInterfaceKitHandle m_handle;
};

#endif // PHIDGETPOLLINGDEVICE_H

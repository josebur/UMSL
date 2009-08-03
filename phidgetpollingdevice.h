#ifndef PHIDGETPOLLINGDEVICE_H
#define PHIDGETPOLLINGDEVICE_H

#include "phidget21.h"

#include <QtGlobal>

class MainWindow;

class PhidgetPollingDevice
{
public:
    PhidgetPollingDevice(MainWindow *mainWindow = 0);
    ~PhidgetPollingDevice();
    bool init();
    qreal pollDevice(int index);

private:
    void displayStats();
    qreal mapNumber(int value);

    MainWindow *m_mainWindow;
    CPhidgetInterfaceKitHandle m_handle;
};

#endif // PHIDGETPOLLINGDEVICE_H

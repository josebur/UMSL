#include "phidgetpollingdevice.h"

#include "phidget21.h"

#include <QDebug>

int AttachHandler(CPhidgetHandle handle, void *userptr);
int DetachHandler(CPhidgetHandle handle, void *userptr);
int ErrorHandler(CPhidgetHandle handle, void *userptr, int ErrorCode, const char *unknown);

PhidgetPollingDevice::PhidgetPollingDevice(MainWindow *mainWindow)
{
    m_handle = 0;
    m_mainWindow = mainWindow;
}

bool PhidgetPollingDevice::init()
{
    CPhidgetInterfaceKit_create(&m_handle);
    CPhidget_set_OnAttach_Handler((CPhidgetHandle)m_handle, AttachHandler, NULL);
    CPhidget_set_OnDetach_Handler((CPhidgetHandle)m_handle, DetachHandler, NULL);
    CPhidget_set_OnError_Handler((CPhidgetHandle)m_handle, ErrorHandler, NULL);

    qDebug() << CPhidget_open((CPhidgetHandle)m_handle, -1);
    return true;
}

int AttachHandler(CPhidgetHandle handle, void *userptr)
{
    int serialNumber;
    const char *name;

    CPhidget_getDeviceName(handle, &name);
    CPhidget_getSerialNumber(handle, &serialNumber);

    qDebug() << name << " " << serialNumber << " attached";

    return 0;
}

int DetachHandler(CPhidgetHandle handle, void *userptr)
{
    int serialNumber;
    const char *name;

    CPhidget_getDeviceName(handle, &name);
    CPhidget_getSerialNumber(handle, &serialNumber);

    qDebug() << name << " " << serialNumber << " detached";

    return 0;
}

int ErrorHandler(CPhidgetHandle handle, void *userptr, int ErrorCode, const char *unknown)
{
    qDebug() << "Error handled. " << ErrorCode << " " << unknown;
    return 0;
}

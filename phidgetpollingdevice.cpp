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
    displayStats();
    return true;
}

void PhidgetPollingDevice::displayStats()
{
    int serialNumber = 0;
    int version = 0;
    int numSensors = 0;
    const char *name = "";

    CPhidget_getDeviceType((CPhidgetHandle)m_handle, &name);
    CPhidget_getSerialNumber((CPhidgetHandle)m_handle, &serialNumber);
    CPhidget_getDeviceVersion((CPhidgetHandle)m_handle, &version);
    CPhidgetInterfaceKit_getSensorCount(m_handle, &numSensors);

    qDebug() << name;
    qDebug() << "Serial Number: " << serialNumber << " Version: " << version;
    qDebug() << "# Sensors: " << numSensors;
}

// For some reason, these needed to be in the global space to work.
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

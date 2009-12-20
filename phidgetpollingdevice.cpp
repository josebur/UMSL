/*
 *   Copyright 2009 by Joe Burns <josebur86@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

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

PhidgetPollingDevice::~PhidgetPollingDevice()
{
    CPhidget_close((CPhidgetHandle)m_handle);
    CPhidget_delete((CPhidgetHandle)m_handle);
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

qreal PhidgetPollingDevice::pollDevice(int index)
{
    int sensorCount;
    int value = 0;
    CPhidgetInterfaceKit_getSensorCount(m_handle, &sensorCount);
    if (index >= 0 && index <= sensorCount) {
        CPhidgetInterfaceKit_getSensorValue(m_handle, index, &value);
        value = 1000 - value;
        qDebug() << "Phidget Value = " << value;
    }

    return mapNumber(value);
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

qreal PhidgetPollingDevice::mapNumber(int value)
{
    qreal newValue = (double)value / 100;
    int precision = (newValue < 1.0) ? 1 : 2;
    QString v = QString::number(newValue, 'g', precision);
    return v.toDouble();
}

// For some reason, these needed to be in the global space to work.
int AttachHandler(CPhidgetHandle handle, void */*userptr*/)
{
    int serialNumber;
    const char *name;

    CPhidget_getDeviceName(handle, &name);
    CPhidget_getSerialNumber(handle, &serialNumber);

    qDebug() << name << " " << serialNumber << " attached";

    return 0;
}

int DetachHandler(CPhidgetHandle handle, void */*userptr*/)
{
    int serialNumber;
    const char *name;

    CPhidget_getDeviceName(handle, &name);
    CPhidget_getSerialNumber(handle, &serialNumber);

    qDebug() << name << " " << serialNumber << " detached";

    return 0;
}

int ErrorHandler(CPhidgetHandle /*handle*/, void */*userptr*/, int ErrorCode, const char *unknown)
{
    qDebug() << "Error handled. " << ErrorCode << " " << unknown;
    return 0;
}

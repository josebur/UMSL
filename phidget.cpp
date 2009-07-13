#include "phidget.h"

#include "phidget21.h"

#include <QDebug>

Phidget *Phidget::m_instance = 0;

Phidget::Phidget()
{
    qDebug() << "Phidget Constructor";
}

Phidget::~Phidget()
{
    if (m_instance) {
        delete m_instance;
    }
}

Phidget *Phidget::self()
{
    if (m_instance == 0) {
        m_instance = new Phidget();
    }

    return m_instance;
}

bool Phidget::init()
{
    qDebug() << "init()";
    return true;
}

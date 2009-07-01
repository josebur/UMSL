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

#include "scene.h"

#include <QDebug>

#include <QString>
#include <QTime>
#include <QTimer>

AbstractScene::AbstractScene(const QString name, const int length,
                             const bool polling)
    : QObject(), m_name(name), m_length(length), m_polling(polling)
{
    m_timer = new QTimer(this);
    m_currentSecond = 0;
}

AbstractScene::~AbstractScene()
{
    delete m_timer;
}


QString AbstractScene::name() const
{
    return m_name;
}

void AbstractScene::setName(const QString name)
{
    m_name = name;
}

int AbstractScene::length() const
{
    return m_length;
}

void AbstractScene::setLength(const int length)
{
    m_length = length;
}

bool AbstractScene::pollDuringScene() const
{
    return m_polling;
}

void AbstractScene::setPollDuringScene(bool polling)
{
    m_polling = polling;
}

void AbstractScene::start()
{
    m_currentSecond = 0;
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(slotTimerTick()));
    m_timer->start(1000);
    emit sceneStarted(this);
}

void AbstractScene::end()
{
    //qDebug() << "Scene " << m_name << " ended";
    m_timer->stop();
    emit sceneEnded(this);
}

void AbstractScene::timeout()
{
    QTime time;
    m_currentSecond++;
    qDebug() << "Scene " << m_name << " is at second " << m_currentSecond
             << " of " << m_length << "at " << time.currentTime().toString()
             << " polling is on = " << m_polling;
    if (m_currentSecond == m_length) {
        end();
    }
}

void AbstractScene::slotTimerTick()
{
    emit secondTick(this);
}

Scene::Scene(const QString name, const int length)
    : AbstractScene(name, length, true)
{
}

BreakScene::BreakScene(QString name, int length)
    : AbstractScene(name, length, false)
{
}

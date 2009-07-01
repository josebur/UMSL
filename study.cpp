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

#include "study.h"

#include "scene.h"

#include <QDebug>
#include <QList>
#include <QString>

Study::Study(const QString name, int pollingInterval)
    : m_name(name), m_pollingInterval(pollingInterval),
      m_currentSceneIndex(-1), m_paused(false)
{

}

Study::~Study()
{
    qDeleteAll(m_scenes);
    m_scenes.clear();
}

QString Study::name() const
{
    return m_name;
}

void Study::setName(const QString name)
{
    m_name = name;
}

int Study::pollingInterval() const
{
    return m_pollingInterval;
}

void Study::setPollingInterval(int interval)
{
    m_pollingInterval = interval;
}

void Study::addScene(AbstractScene *scene)
{
    m_scenes.append(scene);
    listScenes();
}

void Study::removeScene(const QString name)
{
    int index = 0;
    foreach (const AbstractScene *scene, m_scenes) {
        if (scene->name() == name) {
            m_scenes.removeAt(index);
        }
        index++;
    }
    listScenes();
}

void Study::removeScene(AbstractScene *scene)
{
    removeScene(scene->name());
}

bool Study::isPaused() const
{
    return m_paused;
}

void Study::setIsPaused(bool paused)
{
    m_paused = paused;
}

void Study::start()
{
    if (m_scenes.empty()) {
        qDebug() << "No Scenes to start";
        emit studyEnded(this);
        return;
    }

    m_currentSceneIndex = 0;
    AbstractScene *currentScene = m_scenes.at(m_currentSceneIndex);
    connect(currentScene, SIGNAL(sceneEnded(AbstractScene *)),
            this, SLOT(startNextScene()));
    connect(currentScene, SIGNAL(secondTick(AbstractScene *)),
            this, SLOT(pollScene(AbstractScene *)));
    currentScene->start();
    emit studyStarted(this);
}

void Study::pause()
{
    AbstractScene *currentScene = m_scenes.at(m_currentSceneIndex);
    currentScene->pause();
    m_paused = true;
    emit studyPaused(this);
}

void Study::resume()
{
    if (m_paused) {
        AbstractScene *currentScene = m_scenes.at(m_currentSceneIndex);
        currentScene->resume();
        m_paused = false;
    }
}

void Study::startNextScene()
{
    if (m_currentSceneIndex + 1 == m_scenes.size()) {
        qDebug() << "Study Over";
        emit studyEnded(this);
        return;
    }
    else {
        m_currentSceneIndex++;
        AbstractScene *currentScene = m_scenes.at(m_currentSceneIndex);
        connect(currentScene, SIGNAL(sceneEnded(AbstractScene *)),
                this, SLOT(startNextScene()));
        connect(currentScene, SIGNAL(secondTick(AbstractScene *)),
            this, SLOT(pollScene(AbstractScene *)));
        currentScene->start();
    }
}

void Study::pollScene(AbstractScene *scene)
{
    static int currentInterval = 0;
    static qreal total = 0.0;
    //qDebug() << "Scene: " << scene->name();
    if (!scene->pollDuringScene()) {
        qDebug() << "No Polling";
    } else {
        currentInterval++;
        total += 1.0;       // FIXME: actually get values
        if (currentInterval == m_pollingInterval) {
            // calculate the average
            qDebug() << total << "/" << m_pollingInterval
                     << " = " << QString::number(total / m_pollingInterval, 'f', 1);
            currentInterval = 0;
            total = 0.0;
        }
    }
}

void Study::listScenes() const
{
    qDebug() << "Listing this studies scene names,"
                " lengths and if they are set to poll";
    if (m_scenes.empty()) {
        qDebug() << "<empty>";
        return;
    }
    foreach (const AbstractScene *scene, m_scenes) {
        qDebug() << scene->name() << "  " << scene->length()
                 << " " << scene->pollDuringScene();
    }
}

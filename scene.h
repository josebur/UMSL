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

#ifndef SCENE_H
#define SCENE_H

#include <QObject>
#include <QString>

class QTimer;

/**
 * @class AbstractScene UMSL/scene.h
 * @short Abstract base class for a scene.
 */
class AbstractScene : public QObject
{
    Q_OBJECT
public:
    AbstractScene(const QString name = QString(), const int length = 0, const bool polling = false);
    ~AbstractScene();

    /**
     * Returns the name of the scene if one is set,
     * otherwise it returns an empty string.
     *
     * @return QString
     */
    QString name() const;

    /**
     * Set the name of the scene.
     */
    void setName(const QString name);

    /**
     * Returns the length of the scene.
     *
     * @return int
     */
    int length() const;

    /**
     * Set the length of the scene.
     */
    void setLength(const int length);

    /**
     * Returns whether polling should occur while this scene is playing.
     *
     * @return bool
     */
    bool pollDuringScene() const;

protected:
    /**
     * Set whether polling should occure while this scene is playing.
     */
    void setPollDuringScene(bool polling);

public Q_SLOTS:
    /**
     * Start the scene.
     */
    void start();

    /**
     * Pause the scene.
     */
    void pause();

    /**
     * Resume the scene if it has been paused.
     */
    void resume();

    /**
     * Call when the scene ends.
     * (should be private?)
     */
    void end();

private Q_SLOTS:
    /**
     * Called every second.
     */
    void timeout();

Q_SIGNALS:
    /**
     * Signal indicates when a scene has started.
     */
    void sceneStarted(AbstractScene *scene);

    /**
     * Signal indicates when a scene has ended.
     */
    void sceneEnded(AbstractScene *scene);

    /**
     * Signal to indicate when a second has passed
     */
    void secondTick(AbstractScene *scene);

private Q_SLOTS:
    void slotTimerTick();

private:
    QString m_name;
    int m_length;
    QTimer *m_timer;
    bool m_polling;
    int m_currentSecond;
};


/**
 * @class Scene UMSL/scene.h
 * @short Scene class with polling active.
 */
class Scene : public AbstractScene
{
public:
    Scene(const QString name = QString(), const int length = 0);
};

/**
 * @class BreakScene UMSL/scene.h
 * @short An AbstractScene class with polling inactive.
 */
class BreakScene : public AbstractScene
{
public:
    BreakScene(const QString name = QString(), const int length = 0);
};

#endif // SCENE_H

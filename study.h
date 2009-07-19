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

#ifndef STUDY_H
#define STUDY_H

#include "scene.h"

#include <QList>
#include <QObject>
#include <QString>

/**
 * @class Study UMSL/study.h
 * @short A class Study which contains a collection of
 *        AbstractScenes and handles polling from the dials
 */
class Study : public QObject
{
    Q_OBJECT
public:
    Study(const QString name = QString());
    ~Study();

    /**
     * Returns the name of the study.
     *
     * @returns QString
     */
    QString name() const;

    /**
     * Set the name of the Study
     */
    void setName(const QString name);

    /**
     * Returns a list of scenes.
     */
    QList<AbstractScene *> scenes() const;

    /**
     * Adds a new scene to the end of the studies
     * collection of scenes.
     */
    void addScene(AbstractScene *scene);

    /**
     * Removes the scene from the studies collection
     * of scenes with the name;
     */
    void removeScene(const QString name);

    /**
     * Removes the scene from the studies collection
     * of the scenes.
     */
    void removeScene(AbstractScene *scene);

    /**
     * Returns true if study is currently paused, false otherwise
     */
    bool isPaused() const;

    /**
     * Set if the study is currently paused.
     */
    void setIsPaused(bool paused);

    /**
     * Returns the total length of the scene.
     */
    int length() const;

    /**
     * Get the current scene that the study is playing, if any.
     */
    AbstractScene *currentScene() const;

public Q_SLOTS:
    /**
     * Start the timer on the Study. The scenes are
     * played for their length in the order that they
     * in the collection.
     */
    void start();

    /**
     * Pause the study on its current scene.
     */
    void pause();

    /**
     * Resume the study if it has been paused.
     */
    void resume();

Q_SIGNALS:
    /**
     * Emitted when the study is started.
     */
    void studyStarted(Study *);

    /**
     * Emitted when the all of the studies scenes
     * have finished.
     */
    void studyEnded(Study *);

    /**
     * Emitted when the study is paused.
     */
    void studyPaused(Study *);

    /**
     * Emitted when a second of the study has passed.
     */
    void studyTick();

private Q_SLOTS:
    void startNextScene();

private:
    void listScenes() const;

    QString m_name;
    QList<AbstractScene *> m_scenes;
    int m_currentSceneIndex;
    int m_length;
    bool m_paused;
};

#endif // STUDY_H

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

#ifndef STUDYTIMELINE_H
#define STUDYTIMELINE_H

#include <QRectF>
#include <QVector>
#include <QWidget>

class Study;

class StudyTimeLine : public QWidget
{
public:
    StudyTimeLine(QWidget *parent = 0);

    Study *study() const;
    void setStudy(Study *study);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

private:
    void calculateRects();

    Study *m_study;
    QVector<QRectF> rects;
};

#endif // STUDYTIMELINE_H

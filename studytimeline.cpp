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

#include "studytimeline.h"

#include "scene.h"
#include "study.h"

#include <QDebug>
#include <QPainter>
#include <QRectF>
#include <QWidget>

StudyTimeLine::StudyTimeLine(QWidget *parent)
    : QWidget(parent)
{
    m_study = 0;
    setAutoFillBackground(true);
}

Study *StudyTimeLine::study() const
{
    return m_study;
}

void StudyTimeLine::setStudy(Study *study)
{
    m_study = study;
    calculateRects();
    update();
}

void StudyTimeLine::paintEvent(QPaintEvent *event)
{
   calculateRects();
   qDebug() << "paintEvent";
   QPainter painter(this);
   painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing,
                          true);
   painter.setPen(Qt::SolidLine);
   painter.setBrush(Qt::green);
   painter.save();
   painter.drawRects(rects);
   painter.restore();
}

void StudyTimeLine::calculateRects()
{
    const qreal height = (qreal)QWidget::height();
    const qreal width = (qreal)QWidget::width();

    const int totalSeconds = m_study->length();
    const qreal pixelsPerSecond = (qreal)width/totalSeconds;

    qreal start = -1.0;

    foreach (const AbstractScene *scene, m_study->scenes()) {
        start += 1.0;
        qreal scenePixelWidth = scene->length() * pixelsPerSecond;
        QRectF rect(start, 0, scenePixelWidth, height);
        rects.append(rect);
        start += scenePixelWidth;
    }
}

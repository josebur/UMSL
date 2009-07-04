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
#include <QFontMetricsF>
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
    if (m_study && m_study->length() != 0) {
       calculateRects();
       qDebug() << "paintEvent";
       QPainter painter(this);
       painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing,
                              true);
       QPen pen;
       pen.setStyle(Qt::SolidLine);
       pen.setWidthF(2.5);
       pen.setJoinStyle(Qt::RoundJoin);
       painter.setPen(pen);

       int i = 0;
       foreach (const QRectF &rect, m_rects) {
           AbstractScene *scene = m_study->scenes().at(i);
           QFontMetricsF fm(font());
           if (scene->pollDuringScene()) {
               painter.setBrush(Qt::green);
           }
           else {
               painter.setBrush(Qt::red);
           }

           QRectF boundingRect = fm.boundingRect(scene->name());
           painter.drawRect(rect);
           if (rect.size().width() >= boundingRect.width()) {
               painter.drawText(rect, Qt::AlignCenter,
                                scene->name(), &boundingRect);
           }
           ++i;
       }


       qDebug() << "number of rects: " << m_rects.count()
                << " number of scenes: " << m_study->scenes().count();
    }    
    else {
        QRect rect(0, 0, geometry().width(), geometry().height());
        QPainter painter(this);
        painter.setPen(Qt::SolidLine);
        painter.setBrush(Qt::white);
        painter.drawRect(rect);
        if (m_study && m_study->length() == 0) {
            painter.drawText(rect, Qt::AlignCenter, "Study Length is 0");
        }
        else {
            painter.drawText(rect, Qt::AlignCenter, "No Study Selected");
        }
    }
}

void StudyTimeLine::calculateRects()
{
    m_rects.clear();
    const qreal height = (qreal)QWidget::height();
    const qreal width = (qreal)QWidget::width();

    const int totalSeconds = m_study->length();
    const qreal pixelsPerSecond = (qreal)width/totalSeconds;

    qreal start = 0.0;

    foreach (const AbstractScene *scene, m_study->scenes()) {
        qreal scenePixelWidth = scene->length() * pixelsPerSecond;
        QRectF rect(start, 0, scenePixelWidth, height);
        m_rects.append(rect);
        start += scenePixelWidth;
    }
}

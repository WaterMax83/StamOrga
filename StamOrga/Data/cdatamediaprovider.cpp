/*
*	This file is part of StamOrga
*   Copyright (C) 2017 Markus Schneider
*
*	This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 3 of the License, or
*   (at your option) any later version.
*
*	StamOrga is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.

*    You should have received a copy of the GNU General Public License
*    along with StamOrga.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtCore/QDebug>

#include "cdatamediaprovider.h"

cDataMediaProvider::cDataMediaProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
}


QImage cDataMediaProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
    qInfo() << QString("Request Image with %1 ").arg(id) << requestedSize;

    //    QImage image(requestedSize.width() > 0 ? requestedSize.width() : 50,
    //                 requestedSize.height() > 0 ? requestedSize.height() : 50, QImage::Format_RGB32);
    QImage image;


    qInfo() << image.load(":/images/+material/help.png");

    qInfo() << image.size();
    *size = image.size();

    return image;
}

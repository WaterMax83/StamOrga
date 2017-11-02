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

import QtQuick 2.8
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Material.impl 2.1

import com.watermax.demo 1.0

Button {
    id: control

        implicitWidth: Math.max(background ? background.implicitWidth : 0,
                                contentItem.implicitWidth + leftPadding + rightPadding)
        implicitHeight: Math.max(background ? background.implicitHeight : 0,
                                 contentItem.implicitHeight + topPadding + bottomPadding)
        baselineOffset: contentItem.y + contentItem.baselineOffset

        // external vertical padding is 6 (to increase touch area)
        padding: 12
        leftPadding: padding - 4
        rightPadding: padding - 4

        Material.elevation: flat ? control.down || control.hovered ? 2 : 0
                                 : control.down ? 8 : 2
        Material.background: flat ? "transparent" : undefined

        contentItem: Text {
            text: control.text
            font: control.font
            color: !control.enabled ? control.Material.hintTextColor :
                control.flat && control.highlighted ? control.Material.accentColor :
                control.highlighted ? control.Material.primaryHighlightedTextColor : control.Material.foreground
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }

        // TODO: Add a proper ripple/ink effect for mouse/touch input and focus state
        background: Rectangle {
            implicitWidth: 64
            implicitHeight: 48

            // external vertical padding is 6 (to increase touch area)
            y: 6
            width: parent.width
            height: parent.height - 12
            radius: 5
            color: !control.enabled ? control.Material.buttonDisabledColor :
                    control.highlighted ? control.Material.highlightedButtonColor : control.Material.buttonColor
            border.color: control.down ? "#03A9F4" : "#2196F3"
            border.width: 2

            PaddedRectangle {
                y: parent.height - 4
                width: parent.width
                height: 4
                radius: 2
                topPadding: -2
                clip: true
                visible: control.checkable && (!control.highlighted || control.flat)
                color: control.checked && control.enabled ? control.Material.accentColor : control.Material.secondaryTextColor
            }

            Behavior on color {
                ColorAnimation {
                    duration: 400
                }
            }

            // The layer is disabled when the button color is transparent so you can do
            // Material.background: "transparent" and get a proper flat button without needing
            // to set Material.elevation as well
            layer.enabled: control.enabled && control.Material.buttonColor.a > 0
            layer.effect: ElevationEffect {
                elevation: control.Material.elevation
            }

            Ripple {
                clipRadius: 2
                width: parent.width
                height: parent.height
                pressed: control.pressed
                anchor: control
                active: control.down || control.visualFocus || control.hovered
                color: control.Material.rippleColor
            }
        }
}

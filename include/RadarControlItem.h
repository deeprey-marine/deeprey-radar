/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Radar Plugin
 * Author:   David Register
 *           Dave Cowell
 *           Kees Verruijt
 *           Douwe Fokkema
 *           Sean D'Epagnier
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register              bdbcat@yahoo.com *
 *   Copyright (C) 2012-2013 by Dave Cowell                                *
 *   Copyright (C) 2012-2016 by Kees Verruijt         canboat@verruijt.net *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************
 */

#ifndef _RADAR_CONTROL_ITEM_H_
#define _RADAR_CONTROL_ITEM_H_

#include "pi_common.h"
#include "RadarControlState.h"

PLUGIN_BEGIN_NAMESPACE
class radar_pi;
PLUGIN_END_NAMESPACE

#include "RadarControlItem_api.h"

PLUGIN_BEGIN_NAMESPACE
/*
 * RadarRange is a little different. Since the 'Auto' range is computed
 * by the plugin and not a state coming from the radar any range change
 * coming from the radar should not be interpreted as being a change back
 * to manual state (RCS_MANUAL.)
 */
class RadarRangeControlItem : public RadarControlItem {
public:
    RadarRangeControlItem();

    void Update(int v);
};

PLUGIN_END_NAMESPACE


#endif

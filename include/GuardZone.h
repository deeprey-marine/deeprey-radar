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

#ifndef _GUARDZONE_H_
#define _GUARDZONE_H_

#include "radar_pi_api.h"
#include <cstdint>

namespace RadarPlugin {

class radar_pi;
class RadarInfo;

class GuardZone {
public:
    GuardZoneType m_type;
    AngleDegrees m_start_bearing;
    AngleDegrees m_end_bearing;
    int m_inner_range; // start in meters
    int m_outer_range; // end   in meters
    int m_alarm_on;
    int m_arpa_on;
    time_t m_show_time;
    wxLongLong m_arpa_update_time[SPOKES_MAX];

    void ResetBogeys();

    virtual void SetType(GuardZoneType type);
    virtual void SetStartBearing(SpokeBearing start_bearing);
    virtual void SetEndBearing(SpokeBearing end_bearing);
    virtual void SetInnerRange(int inner_range);
    virtual void SetOuterRange(int outer_range);
    virtual void SetArpaOn(int arpa);
    virtual void SetAlarmOn(int alarm);

    /*
     * Check if data is in this GuardZone, if so update bogeyCount
     */
    void ProcessSpoke(
        SpokeBearing angle, uint8_t* data, uint8_t* hist, size_t len);

    // Find targets inside the zone
    void SearchTargets();

    int GetBogeyCount();

    GuardZone(radar_pi* pi, RadarInfo* ri, int zone);

    ~GuardZone();

private:
    radar_pi* m_pi;
    RadarInfo* m_ri;

    wxString m_log_name;
    bool m_last_in_guard_zone;
    SpokeBearing m_last_angle;
    int m_bogey_count; // complete cycle
    int m_running_count; // current swipe

    void UpdateSettings();
};

} // namespace

#endif /* _GUARDZONE_H_ */

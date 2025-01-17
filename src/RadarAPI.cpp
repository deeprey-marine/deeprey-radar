#include <vector>

#include "radar_pi.h"
#include "RadarAPI.h"

#include "RadarInfo.h"


PLUGIN_BEGIN_NAMESPACE

RadarAPI::RadarAPI(radar_pi* radarPlugin) : m_radarPlugin(radarPlugin), m_overlay(nullptr) {
  // constructor
}

RadarAPI::~RadarAPI() {
  // destructor
}

void RadarAPI::SetRadarRangeNM(double range_nm) {
  //if (m_radarPlugin && m_radarPlugin->m_settings.radar_count > 0) {
    // convert nm → meters if needed
    //int meters = nmToMeters(range_nm);
    // e.g. set range on the first radar
    //m_radarPlugin->m_radar[0]->m_control->SetRange(meters);
  //}
}

double RadarAPI::GetRadarRangeNM() const { 
    return m_radarPlugin->m_radar[0]->m_range.GetValue();
}

void RadarAPI::SetGain(int gain) {
  // Some code to update actual hardware gain
}

int RadarAPI::GetGain() const { 
    // return the radar gain 

    return m_radarPlugin->m_radar[0]->m_gain.GetValue();
}

void RadarAPI::RegisterOverlayRenderer(IRadarOverlay* overlay) { m_overlay = overlay; }

void RadarAPI::UnregisterOverlayRenderer(IRadarOverlay* overlay) {
  if (m_overlay == overlay) {
        m_overlay = nullptr;
    }
}

PLUGIN_END_NAMESPACE
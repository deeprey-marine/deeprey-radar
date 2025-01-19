#include <vector>

#include "radar_pi.h"
#include "RadarAPI.h"

#include "RadarInfo.h"


PLUGIN_BEGIN_NAMESPACE
RadarAPI::RadarAPI(radar_pi* radarPlugin)
  : m_pi(radarPlugin), m_overlay(nullptr) {
  // constructor
}

RadarAPI::~RadarAPI() {
  // destructor
}


bool RadarAPI::SetControl(const wxString& controlName, const wxVariant& value) {


  switch (StringToControlType(controlName)) {

    case CT_RANGE: {
      int rangeMeters = value.GetInteger();
      if (rangeMeters < m_pi->m_radar[0]->m_range.GetMin())
        rangeMeters = m_pi->m_radar[0]->m_range.GetMin();
      if (rangeMeters > m_pi->m_radar[0]->m_range.GetMax())
        rangeMeters = m_pi->m_radar[0]->m_range.GetMax();
      // Apply
      if (m_pi->m_radar[0]->m_control) {
        m_pi->m_radar[0]->m_control->SetRange(rangeMeters);
        // Update locally
        m_pi->m_radar[0]->m_range.Update(rangeMeters);
      }
      break;
    }

    case CT_OVERLAY_CANVAS: {
      int val = value.GetBool();
      if (controlName == "overlayCanvas_0") {
          m_pi->m_radar[0]->m_overlay_canvas[0].Update(val);
      } else if (controlName == "overlayCanvas_1") {
          m_pi->m_radar[0]->m_overlay_canvas[1].Update(val);
      }
      break;
    }

    case CT_GAIN: {
      int gainPercent = value.GetInteger();
      if (gainPercent < m_pi->m_radar[0]->m_gain.GetMin())
        gainPercent = m_pi->m_radar[0]->m_gain.GetMin();
      if (gainPercent > m_pi->m_radar[0]->m_gain.GetMax())
        gainPercent = m_pi->m_radar[0]->m_gain.GetMax();
      m_pi->m_radar[0]->m_gain.Update(gainPercent, RCS_MANUAL);
      m_pi->m_radar[0]->SetControlValue(CT_GAIN, m_pi->m_radar[0]->m_gain, nullptr);
      break;
    }
  }


  return true;
}

wxVariant RadarAPI::GetControl(const wxString& controlName)  {

    switch (StringToControlType(controlName)) {

      case CT_RANGE: {
        return m_pi->m_radar[0]->m_range.GetValue();
      }

      case CT_OVERLAY_CANVAS: {
        if (controlName == "overlayCanvas_0") {
          return m_pi->m_radar[0]->m_overlay_canvas[0].GetValue();
        } else if (controlName == "overlayCanvas_1") {
          return m_pi->m_radar[0]->m_overlay_canvas[1].GetValue();
        }
        
      }

      case CT_GAIN: {
        return m_pi->m_radar[0]->m_gain.GetValue();
      }
    }

  return wxVariant();
}

void RadarAPI::SetRadarRangeNM(double range_nm) {
  //if (m_radarPlugin && m_radarPlugin->m_settings.radar_count > 0) {
  // convert nm → meters if needed
  //int meters = nmToMeters(range_nm);
  // e.g. set range on the first radar
  //m_radarPlugin->m_radar[0]->m_control->SetRange(meters);
  //}
}

bool RadarAPI::Transmit(bool enable) {
  if (enable) {
    m_pi->m_radar[0]->RequestRadarState(RADAR_TRANSMIT);
  } else {
    m_pi->m_radar[0]->RequestRadarState(RADAR_STANDBY);
  }

  int state = m_pi->m_radar[0]->m_state.GetValue();

  return state == RADAR_TRANSMIT;
}

double RadarAPI::GetRadarRangeNM() const {
  return m_pi->m_radar[0]->m_range.GetValue();
}

void RadarAPI::SetGain(int gain) {
  // Some code to update actual hardware gain
}

int RadarAPI::GetGain() const {
  // return the radar gain 

  return m_pi->m_radar[0]->m_gain.GetValue();
}

void RadarAPI::RegisterOverlayRenderer(IRadarOverlay* overlay) { m_overlay = overlay; }

void RadarAPI::UnregisterOverlayRenderer(IRadarOverlay* overlay) {
  if (m_overlay == overlay) {
    m_overlay = nullptr;
  }
}


bool RadarAPI::ProcessMessage(const wxString& message_id, const wxString& message_body) {
  if (message_id != "DP_GUI_TO_RADAR") {
    return false;
  }

  // Parse JSON
  wxJSONReader reader;
  wxJSONValue root;
  if (reader.Parse(message_body, &root) != 0) {
    LOG_INFO("RadarAPI: JSON parse error for message: %s", message_body);
    return false;
  }

  wxString messageType = root["type"].AsString();
  if (messageType == "ping") {
    SendPongMessage();
    return true;
  }

  return true;
}


void RadarAPI::SendMessageToDp(std::initializer_list<std::pair<const wxString, wxVariant>> values) {
  wxJSONValue root;

  for (auto& kv : values) {
    const wxString& key = kv.first;
    const wxVariant& val = kv.second;

    if (val.GetType() == "bool") {
      root[key] = val.GetBool();
    } else if (val.GetType() == "long") {
      root[key] = (int)val.GetLong();
    } else if (val.GetType() == "double") {
      root[key] = val.GetDouble();
    } else if (val.GetType() == "string") {
      root[key] = val.GetString();
    } else {
      root[key] = val.MakeString();
    }
  }

  wxJSONWriter writer;
  wxString serializedMessage;
  writer.Write(root, serializedMessage);

  SendPluginMessage("RADAR_API_TO_DP_GUI", serializedMessage);
}


bool RadarAPI::SelectRadarType(int type) {
  // Select radar type
  return m_pi->SelectRadarType(type);
}

void RadarAPI::SendPongMessage() {
  SendMessageToDp({{"type", "pong"}});
}



ControlType RadarAPI::StringToControlType(const wxString& controlTypeStr) {
  const auto& controlTypeMap = GetControlTypeMap();
  auto it = controlTypeMap.find(controlTypeStr);
  if (it != controlTypeMap.end()) {
    return it->second;
  }
  return ControlType::CT_MAX;
}

const std::unordered_map<wxString, ControlType>& RadarAPI::GetControlTypeMap() {
  static const std::unordered_map<wxString, ControlType> controlTypeMap = {
    // Software
    {wxT("antennaForward"),            ControlType::CT_ANTENNA_FORWARD},
    {wxT("antennaStarboard"),          ControlType::CT_ANTENNA_STARBOARD},
    {wxT("mainBangSize"),              ControlType::CT_MAIN_BANG_SIZE},
    {wxT("orientation"),               ControlType::CT_ORIENTATION},
    {wxT("centerView"),                ControlType::CT_CENTER_VIEW},
    {wxT("overlayCanvas_0"),           ControlType::CT_OVERLAY_CANVAS},
    {wxT("overlayCanvas_1"),           ControlType::CT_OVERLAY_CANVAS},
    {wxT("targetOnPPI"),               ControlType::CT_TARGET_ON_PPI},
    {wxT("refreshRate"),               ControlType::CT_REFRESHRATE},
    {wxT("targetTrails"),              ControlType::CT_TARGET_TRAILS},
    {wxT("threshold"),                 ControlType::CT_THRESHOLD},
    {wxT("timedIdle"),                 ControlType::CT_TIMED_IDLE},
    {wxT("timedRun"),                  ControlType::CT_TIMED_RUN},
    {wxT("trailsMotion"),              ControlType::CT_TRAILS_MOTION},

    // Hardware
    {wxT("accentLight"),               ControlType::CT_ACCENT_LIGHT},
    {wxT("antennaHeight"),             ControlType::CT_ANTENNA_HEIGHT},
    {wxT("bearingAlignment"),          ControlType::CT_BEARING_ALIGNMENT},
    {wxT("gain"),                      ControlType::CT_GAIN},
    {wxT("interferenceRejection"),     ControlType::CT_INTERFERENCE_REJECTION},
    {wxT("localInterferenceRejection"),ControlType::CT_LOCAL_INTERFERENCE_REJECTION},
    {wxT("noiseRejection"),            ControlType::CT_NOISE_REJECTION},
    {wxT("noTransmitStart1"),          ControlType::CT_NO_TRANSMIT_START_1},
    {wxT("noTransmitStart2"),          ControlType::CT_NO_TRANSMIT_START_2},
    {wxT("noTransmitStart3"),          ControlType::CT_NO_TRANSMIT_START_3},
    {wxT("noTransmitStart4"),          ControlType::CT_NO_TRANSMIT_START_4},
    {wxT("noTransmitEnd1"),            ControlType::CT_NO_TRANSMIT_END_1},
    {wxT("noTransmitEnd2"),            ControlType::CT_NO_TRANSMIT_END_2},
    {wxT("noTransmitEnd3"),            ControlType::CT_NO_TRANSMIT_END_3},
    {wxT("noTransmitEnd4"),            ControlType::CT_NO_TRANSMIT_END_4},
    {wxT("rain"),                      ControlType::CT_RAIN},
    {wxT("range"),                     ControlType::CT_RANGE},
    {wxT("scanSpeed"),                 ControlType::CT_SCAN_SPEED},
    {wxT("sea"),                       ControlType::CT_SEA},
    {wxT("seaState"),                  ControlType::CT_SEA_STATE},
    {wxT("ftc"),                       ControlType::CT_FTC},
    {wxT("mode"),                      ControlType::CT_MODE},
    {wxT("allToAuto"),                 ControlType::CT_ALL_TO_AUTO},
    {wxT("sideLobeSuppression"),       ControlType::CT_SIDE_LOBE_SUPPRESSION},
    {wxT("targetBoost"),               ControlType::CT_TARGET_BOOST},
    {wxT("targetExpansion"),           ControlType::CT_TARGET_EXPANSION},
    {wxT("targetSeparation"),          ControlType::CT_TARGET_SEPARATION},
    {wxT("transparency"),              ControlType::CT_TRANSPARENCY},
    {wxT("doppler"),                   ControlType::CT_DOPPLER},
    {wxT("dopplerThreshold"),          ControlType::CT_DOPPLER_THRESHOLD},
    {wxT("dopplerAutoTrack"),          ControlType::CT_AUTOTTRACKDOPPLER},

    // Raymarine
    {wxT("stc"),                       ControlType::CT_STC},
    {wxT("tuneFine"),                  ControlType::CT_TUNE_FINE},
    {wxT("tuneCoarse"),                ControlType::CT_TUNE_COARSE},
    {wxT("stcCurve"),                  ControlType::CT_STC_CURVE},
    {wxT("displayTiming"),             ControlType::CT_DISPLAY_TIMING},
    {wxT("mainBangSuppression"),       ControlType::CT_MAIN_BANG_SUPPRESSION},
    {wxT("colorGain"),                 ControlType::CT_COLOR_GAIN},
    {wxT("rangeAdjustment"),           ControlType::CT_RANGE_ADJUSTMENT}
  };

  return controlTypeMap;
}


PLUGIN_END_NAMESPACE
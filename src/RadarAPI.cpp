#include <vector>

#include "radar_pi.h"
#include "RadarAPI.h"

#include "RadarInfo.h"
#include "MessageBox.h"
#include "TrailBuffer.h"


PLUGIN_BEGIN_NAMESPACE
RadarAPI::RadarAPI(radar_pi* radarPlugin)
  : m_pi(radarPlugin), m_overlay(nullptr), m_nextCallbackID(1) {
  // constructor
}

RadarAPI::~RadarAPI() {
  // destructor
}


bool RadarAPI::SetControl(ControlType controlType, int value, int controlIndex) {

  RadarControlItem* controlItem = GetControlItem(controlType, controlIndex);
  if (controlItem) {
    SetControl(controlType, value, *controlItem);

    if (controlType == CT_OVERLAY_CANVAS) {
      for (auto callback : m_canvasOverlayEnabledChangeCallbacks) {
        callback.second(controlIndex);
      }
    }
  }
  return true;
}

RadarControlItem* RadarAPI::GetControlItem(ControlType controlType, int controlIndex)
{
  RadarControlItem* result = nullptr;
  switch (controlType) {
    case CT_OVERLAY_CANVAS: {
      result = &m_pi->m_radar[0]->m_overlay_canvas[controlIndex];
      break;
    }
    case CT_GAIN: {
      result = &m_pi->m_radar[0]->m_gain;
      break;
    }
    case CT_SEA: {
      result = &m_pi->m_radar[0]->m_sea;
      break;
    }
    case CT_RAIN: {
      result = &m_pi->m_radar[0]->m_rain;
      break;
    }
    case CT_RANGE: {
      result = &m_pi->m_radar[0]->m_range;
      break;
    }
    case CT_ORIENTATION: {
      result = &m_pi->m_radar[0]->m_orientation;
      break;
    }
    case CT_BEARING_RELATIVE: {
      result = &m_pi->m_radar[0]->m_bearing_relative;
      break;
    }
    case CT_TRAILS_MOTION: {
      result = &m_pi->m_radar[0]->m_trails_motion;
      break;
    }
    case CT_NOISE_REJECTION: {
      result = &m_pi->m_radar[0]->m_noise_rejection;
      break;
    }
    case CT_THRESHOLD: {
      result = &m_pi->m_radar[0]->m_threshold;
      break;
    }
    case CT_TARGET_EXPANSION: {
      result = &m_pi->m_radar[0]->m_target_expansion;
      break;
    }
    case CT_INTERFERENCE_REJECTION: {
      result = &m_pi->m_radar[0]->m_interference_rejection;
      break;
    }
    case CT_TARGET_SEPARATION: {
      result = &m_pi->m_radar[0]->m_target_separation;
      break;
    }
    case CT_SCAN_SPEED: {
      result = &m_pi->m_radar[0]->m_scan_speed;
      break;
    }
    case CT_TARGET_BOOST: {
      result = &m_pi->m_radar[0]->m_target_boost;
      break;
    }
    case CT_TARGET_ON_PPI: {
      result = &m_pi->m_radar[0]->m_target_on_ppi;
      break;
    }
    case CT_TARGET_TRAILS: {
      result = &m_pi->m_radar[0]->m_target_trails;
      break;
    }
    case CT_BEARING_ALIGNMENT: {
      result = &m_pi->m_radar[0]->m_bearing_alignment;
      break;
    }
    case CT_RANGE_ADJUSTMENT: {
      result = &m_pi->m_radar[0]->m_range_adjustment;
      break;
    }
    case CT_DISPLAY_TIMING: {
      result = &m_pi->m_radar[0]->m_display_timing;
      break;
    }
    case CT_MAIN_BANG_SUPPRESSION: {
      result = &m_pi->m_radar[0]->m_main_bang_suppression;
      break;
    }
    case CT_NO_TRANSMIT_START_1:
    case CT_NO_TRANSMIT_START_2: 
    case CT_NO_TRANSMIT_START_3: 
    case CT_NO_TRANSMIT_START_4: {
      result = &m_pi->m_radar[0]->m_no_transmit_start[controlType - CT_NO_TRANSMIT_START_1];
      break;
    }
    case CT_NO_TRANSMIT_END_1:
    case CT_NO_TRANSMIT_END_2:
    case CT_NO_TRANSMIT_END_3:
    case CT_NO_TRANSMIT_END_4: {
      result = &m_pi->m_radar[0]->m_no_transmit_end[controlType - CT_NO_TRANSMIT_END_1];
      break;
    }
    case CT_ANTENNA_HEIGHT: {
      result = &m_pi->m_radar[0]->m_antenna_height;
      break;
    }
    case CT_ANTENNA_FORWARD: {
      result = &m_pi->m_radar[0]->m_antenna_forward;
      break;
    }
    case CT_ANTENNA_STARBOARD: {
      result = &m_pi->m_radar[0]->m_antenna_starboard;
      break;
    }
    case CT_LOCAL_INTERFERENCE_REJECTION: {
      result = &m_pi->m_radar[0]->m_local_interference_rejection;
      break;
    }
    case CT_SIDE_LOBE_SUPPRESSION: {
      result = &m_pi->m_radar[0]->m_side_lobe_suppression;
      break;
    }
    case CT_MAIN_BANG_SIZE: {
      result = &m_pi->m_radar[0]->m_main_bang_size;
      break;
    }
    case CT_ACCENT_LIGHT: {
      result = &m_pi->m_radar[0]->m_accent_light;
      break;
    }
  }

  return result;
}

void RadarAPI::SetControlWithState(ControlType controlType, int value, RadarControlState state, int controlIndex) {
  RadarControlItem* controlItem = GetControlItem(controlType, controlIndex);
  if (controlItem) {
    controlItem->Update(value, state);
    m_pi->m_radar[0]->SetControlValue(controlType, *controlItem, nullptr);
  }
}

void RadarAPI::SetControlState(ControlType controlType, RadarControlState state) {
  RadarControlItem* controlItem = GetControlItem(controlType);
  if (controlItem)
  {
    controlItem->UpdateState(state);
    m_pi->m_radar[0]->SetControlValue(controlType, *controlItem, nullptr);
  }
}

void RadarAPI::SetControl(ControlType controlType, int value, RadarControlItem& controlItem) {
  ControlInfo& ci = m_pi->m_radar[0]->m_ctrl[controlType];

  if (value < ci.minValue) value = ci.minValue;
  if (value > ci.maxValue) value = ci.maxValue;
  controlItem.Update(value, RCS_MANUAL);
  m_pi->m_radar[0]->SetControlValue(controlType, controlItem, nullptr);
}

bool RadarAPI::GetControl(ControlType controlType, int* value, RadarControlState* state, int controlIndex) {

    RadarControlItem* controlItem = GetControlItem(controlType, controlIndex); 
    return controlItem ? controlItem->GetButton(value, state) : false;
}

int RadarAPI::GetControl(ControlType controlType, int controlIndex)
{
  int value;
  GetControl(controlType, &value, nullptr, controlIndex);
  return value;
}

void RadarAPI::SetRadarRangeNM(double range_nm) {
  //if (m_radarPlugin && m_radarPlugin->m_settings.radar_count > 0) {
  // convert nm → meters if needed
  //int meters = nmToMeters(range_nm);
  // e.g. set range on the first radar
  //m_radarPlugin->m_radar[0]->m_control->SetRange(meters);
  //}
}

void RadarAPI::Transmit(bool enable) {
  if (enable) {
    m_pi->m_radar[0]->RequestRadarState(RADAR_TRANSMIT);
  } else {
    m_pi->m_radar[0]->RequestRadarState(RADAR_STANDBY);
  }  
}

double RadarAPI::GetRadarRangeNM() const {
  return m_pi->m_radar[0]->m_range.GetValue();
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
    } else if (val.GetType() == "ulonglong") {
      root[key] = val.GetULongLong().GetValue();
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
  bool result = m_pi->SelectRadarType(type);
  if (m_radarTypeChangeCallback) m_radarTypeChangeCallback();
  return result;
}

int RadarAPI::GetRadarType() { return m_pi->m_radar[0]->m_radar_type; }

void RadarAPI::SetRadarTypeChangeCallback(std::function<void()> callback) { m_radarTypeChangeCallback = callback; }

uint64_t RadarAPI::AddCanvasOverlayEnabledChangeCallback(std::function<void(int)> callback) {
  uint64_t id = m_nextCallbackID++;
  m_canvasOverlayEnabledChangeCallbacks[id] = callback;
  return id;
}

void RadarAPI::RemoveCanvasOverlayEnabledChangeCallback(uint64_t callbackID)
{
  m_canvasOverlayEnabledChangeCallbacks.erase(callbackID);
}

ControlInfo* RadarAPI::GetRadarControls() { return m_pi->m_radar[0]->m_ctrl; }

GuardZone** RadarAPI::GetGuardZones() { return m_pi->m_radar[0]->m_guard_zone; }

double RadarAPI::GetRangeUnitsToMeters() { return RangeUnitsToMeters[m_pi->m_settings.range_units]; }

void RadarAPI::SendPongMessage() {
  SendMessageToDp({{"type", "pong"}, {"api", wxULongLong((wxULongLong_t)this)}});
}

RadarState RadarAPI::GetRadarState() { return (RadarState)m_pi->m_radar[0]->m_state.GetButton(); }

void RadarAPI::AdjustRange(int adjustment) {  m_pi->m_radar[0]->AdjustRange(adjustment); }

bool RadarAPI::HasHeadingSource() { return m_pi->GetHeadingSource() != HEADING_NONE; }

void RadarAPI::ShowInfoDialog() {
  if (m_pi->m_pMessageBox) {
    m_pi->m_pMessageBox->UpdateMessage(true);
  }
}

void RadarAPI::ClearTrails() { m_pi->m_radar[0]->m_trails->ClearTrails(); }

PersistentSettings* RadarAPI::GetSettings() { return &m_pi->m_settings; }

PLUGIN_END_NAMESPACE
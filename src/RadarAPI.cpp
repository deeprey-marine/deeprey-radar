﻿#include <vector>

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


bool RadarAPI::SetControl(ControlType controlType, int value, int controlIndex) {

  RadarControlItem* controlItem = GetControlItem(controlType, controlIndex);
  if (controlItem)
    SetControl(controlType, value, *controlItem);  

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
  return m_pi->SelectRadarType(type);
}

ControlInfo* RadarAPI::GetRadarControls() { return m_pi->m_radar[0]->m_ctrl; }

void RadarAPI::SendPongMessage() {
  SendMessageToDp({{"type", "pong"}, {"api", wxULongLong((wxULongLong_t)this)}});
}

RadarState RadarAPI::GetRadarState() { return (RadarState)m_pi->m_radar[0]->m_state.GetButton(); }

void RadarAPI::AdjustRange(int adjustment) {  m_pi->m_radar[0]->AdjustRange(adjustment); }

bool RadarAPI::HasHeadingSource() { return m_pi->GetHeadingSource() != HEADING_NONE; }

PLUGIN_END_NAMESPACE
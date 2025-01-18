#include "DpRadarCommand.h"
#include <wx/log.h>

#include "radar_pi.h"

#include "RadarInfo.h"

#include "GuardZone.h"

PLUGIN_BEGIN_NAMESPACE

DpRadarCommand::DpRadarCommand(radar_pi* plugin)
    : m_pi(plugin)
{
   initActions();
}

DpRadarCommand::~DpRadarCommand()
{

}



bool DpRadarCommand::ProcessMessage(const wxString &message_id, const wxString &message_body)
{
    if (message_id != "DP_UI_CONFIG_RADAR") {
        return false;
    }

    // Parse JSON
    wxJSONReader reader;
    wxJSONValue root;
    if (reader.Parse(message_body, &root) != 0) {
        LOG_INFO("DpRadarCommand: JSON parse error for message: %s", message_body);
        return false;
    }


    // {
    //   "RadarIndex": 0,
    //   "Commands": [
    //       { "Name": "Transmit", "Value": true },
    //       { "Name": "Range",    3000  },
    //       { "Name": "Gain",     40    },
    //       ...
    //   ]
    // }
    int radarIndex = root["RadarIndex"].AsInt();
    if (radarIndex < 0 || radarIndex >= (int)m_pi->m_settings.radar_count) {
        LOG_INFO("DpRadarCommand: invalid RadarIndex=%d", radarIndex);
        return false;
    }

    wxJSONValue commands = root["Commands"];
    if (!commands.IsArray()) {
        LOG_INFO("DpRadarCommand: 'Commands' is not an array");
        return false;
    }

    for (int i = 0; i < (int)commands.Size(); i++) {
        wxJSONValue cmd = commands[i];
        wxString name   = cmd["Name"].AsString();  // "Transmit", "Range", "Gain", ...
        wxJSONValue val = cmd["Value"];           

        HandleCommand(name, val, radarIndex);
    }

    return true; 
}

void DpRadarCommand::HandleCommand(const wxString &name, const wxJSONValue &value, const int radarIndex)
{

    if (!m_pi->m_radar[radarIndex]) {
        return;
    }
    RadarInfo* ri = m_pi->m_radar[radarIndex];
    if (!ri) {
        LOG_INFO("DpRadarCommand::HandleCommand: no radar info at index %d", radarIndex);
        return;
    }

    auto it = m_actions.find(name);
    if (it != m_actions.end()) {
        it->second(ri, value);
    } else {
        LOG_INFO("DpRadarCommand: invalid command: %s", name.c_str());
    }
 
}

void DpRadarCommand::initActions() {
    m_actions = {
        {
            "SetRadarType",
            [this](RadarInfo* ri, const wxJSONValue& val) {
               m_pi->SelectRadarType(val.AsInt()) ;
            }
        },
        {
            "ReloadRadar",
            [this](RadarInfo* ri, const wxJSONValue& val) {
               m_pi->SelectRadarType(val.AsInt(), true) ;
            }
        },
        {
            "GetRadarType",
            [this](RadarInfo* ri, const wxJSONValue& val) {
                SendToDp(ri,
                    {
                        {"RadarType", (int)ri->m_radar_type}
                    }
                );
               
            }
        },
        {
            "GetRadarInfos",
            [this](RadarInfo* ri, const wxJSONValue& val) {
                SendToDp(ri,
                    {
                        {"RadarType",     (int)ri->m_radar_type},
                        {"Transmit",      (int)ri->m_state.GetValue()},
                        {"range",         (int)ri->m_range.GetValue()},
                        {"gain",          (int)ri->m_gain.GetValue()},
  
                    }
                );
            }
        },
        {
            "SetOverlay_0",
            [this](RadarInfo* ri, const wxJSONValue& val) {
                if (val.IsBool()) {
                    bool activeOverlay = val.AsBool();
                    if (activeOverlay) {
                        ri->m_overlay_canvas[0].Update(1);
                    } else {
                        ri->m_overlay_canvas[0].Update(0);
                    }
                }
            }
        },
        {
            "SetOverlay_1",
            [this](RadarInfo* ri, const wxJSONValue& val) {
                 if (val.IsBool()) {
                    bool activeOverlay = val.AsBool();
                    if (activeOverlay) {
                        ri->m_overlay_canvas[1].Update(1);
                    } else {
                        ri->m_overlay_canvas[1].Update(0);
                    }
                }
            }
        },
        {
            "Transmit",
            [this](RadarInfo* ri, const wxJSONValue& val) {
                bool transmit = false;
                if (val.IsBool()) {
                    transmit = val.AsBool();
                } else {
                    LOG_INFO("DpRadarCommand: 'Transmit' value is not bool/int => ignoring");
                    return;
                }

                if (transmit) {
                    ri->RequestRadarState(RADAR_TRANSMIT);
                } else {
                    ri->RequestRadarState(RADAR_STANDBY);
                }

                SendToDp(ri,
                    {
                        {"Transmit", (int)ri->m_state.GetValue()}
                    }
                );
            }
        },
        {
            "Range",
            [this](RadarInfo* ri, const wxJSONValue& val) {
                double rangeMeters = val.IsDouble() ? val.AsDouble() : val.AsInt();
                int range = (int)rangeMeters;
                if (range < ri->m_range.GetMin())   range = ri->m_range.GetMin();
                if (range > ri->m_range.GetMax())   range = ri->m_range.GetMax();

                // Appliquer
                if (ri->m_control) {
                    ri->m_control->SetRange(range);
                    // Mettre à jour localement
                    ri->m_range.Update(range);
                }
            }
        }, 
        {
            "Gain",
            [this](RadarInfo* ri, const wxJSONValue& val) {
                double gainVal = val.IsDouble() ? val.AsDouble() : val.AsInt();
                int gainPercent = (int)gainVal;
                if (gainPercent < ri->m_gain.GetMin())   gainPercent = ri->m_gain.GetMin();
                if (gainPercent > ri->m_gain.GetMax())   gainPercent = ri->m_gain.GetMax();
                ri->m_gain.Update(gainPercent, RCS_MANUAL);
                ri->SetControlValue(CT_GAIN, ri->m_gain, nullptr);

            }
        },
        {
            "SeaClutter",
            [this](RadarInfo* ri, const wxJSONValue& val) {
              double seaVal = val.IsDouble() ? val.AsDouble() : val.AsInt();
              int sea = (int)seaVal;
              if (sea < ri->m_sea.GetMin()) sea = ri->m_sea.GetMin();
              if (sea > ri->m_sea.GetMax()) sea = ri->m_sea.GetMax();
              ri->m_sea.Update(sea, RCS_MANUAL);
              ri->SetControlValue(CT_SEA, ri->m_sea, nullptr);

              SendToDp(ri, { {"SeaClutter", sea} });
            }
        },
        {
            "ClearTrails",
            [this](RadarInfo* ri, const wxJSONValue& val) {
                ri->ClearTrails();

                SendToDp(ri, { {"TrailsCleared", true} });
            }
        },
        {
            "Orientation",
            [this](RadarInfo* ri, const wxJSONValue& val) {
                int orientation = val.AsInt();  //     0 => ORIENTATION_HEAD_UP, 1 => ORIENTATION_STABILIZED_UP, 
                                                //     2 => ORIENTATION_NORTH_UP, 3 => ORIENTATION_COG_UP , 4 => ORIENTATION_NUMBER
                if (orientation < 0 || orientation >= ORIENTATION_NUMBER) {
                    LOG_INFO("DpRadarCommand: 'Orientation' value is not in range ");
                    return;
                }
                ri->m_orientation.Update(orientation);
                // On appelle SetControlValue
                ri->SetControlValue(CT_ORIENTATION, ri->m_orientation, nullptr);
                SendToDp(ri, { {"Orientation", orientation} });
            }
        },
        {
            "RainClutter",
            [this](RadarInfo* ri, const wxJSONValue& val) {
                double rainVal = val.IsDouble() ? val.AsDouble() : val.AsInt();
                int rain = (int)rainVal;
                if (rain < ri->m_rain.GetMin())     rain = ri->m_rain.GetMin();
                if (rain > ri->m_rain.GetMax())     rain = ri->m_rain.GetMax();

                ri->m_rain.Update(rain, RCS_MANUAL);
                ri->SetControlValue(CT_RAIN, ri->m_rain, nullptr);
                SendToDp(ri, {{"RainClutter", rain}});
            }
        },
        {
            "FTC",
            [this](RadarInfo* ri, const wxJSONValue& val) {
                double ftcVal = val.IsDouble() ? val.AsDouble() : val.AsInt();
                int ftc = (int)ftcVal;
                if (ftc < ri->m_ftc.GetMin()) ftc = ri->m_ftc.GetMin();
                if (ftc > ri->m_ftc.GetMax()) ftc = ri->m_ftc.GetMax();

                ri->m_ftc.Update(ftc, RCS_MANUAL);
                ri->SetControlValue(CT_FTC, ri->m_ftc, nullptr);
                SendToDp(ri, {{"FTC", ftc}});
            }
        },
        {
            "ColorGain",
            [this](RadarInfo* ri, const wxJSONValue& val) {
                double cgVal = val.IsDouble() ? val.AsDouble() : val.AsInt();
                int colorGain = (int)cgVal;
                colorGain = wxMax(wxMin(colorGain, ri->m_color_gain.GetMax()), ri->m_color_gain.GetMin());

                ri->m_color_gain.Update(colorGain, RCS_MANUAL);
                ri->SetControlValue(CT_COLOR_GAIN, ri->m_color_gain, nullptr);
                SendToDp(ri, {{"ColorGain", colorGain}});
            }
        },
        {
            "Mode", // Halo / Quantum / etc..
            [this](RadarInfo* ri, const wxJSONValue& val) {
                int mode = val.AsInt();  // Mode , harbor 0, coastal 1, offshore 2, weather 3
                if (mode < 0 || mode >= 4) {
                    LOG_INFO("DpRadarCommand: 'Mode' value is not in range ");
                    return;
                }
                ri->m_mode.Update(mode, RCS_MANUAL);
                ri->SetControlValue(CT_MODE, ri->m_mode, nullptr);
                SendToDp(ri, {{"Mode", mode}});
            }
        },
        {
            "AllToAuto",
            [this](RadarInfo* ri, const wxJSONValue& val) {
                // Bool ou simple "activer"
                bool setAllAuto = val.AsBool();
                if (setAllAuto) {
                    ri->m_all_to_auto.Update(1);
                } else {
                    ri->m_all_to_auto.Update(0, RCS_MANUAL);
                }
                ri->SetControlValue(CT_ALL_TO_AUTO, ri->m_all_to_auto, nullptr);
                SendToDp(ri, {{"AllToAuto", setAllAuto}});
            }
        },
        {
            "InterferenceRejection",
            [this](RadarInfo* ri, const wxJSONValue& val) {
                int rej = val.AsInt();
                if (rej < ri->m_interference_rejection.GetMin())    rej = ri->m_interference_rejection.GetMin();
                if (rej > ri->m_interference_rejection.GetMax())    rej = ri->m_interference_rejection.GetMax();

                ri->m_interference_rejection.Update(rej, RCS_MANUAL);
                ri->SetControlValue(CT_INTERFERENCE_REJECTION, ri->m_interference_rejection, nullptr);
                SendToDp(ri, {{"InterferenceRejection", rej}});
            }
        },
        {
            "BearingAlignment",
            [this](RadarInfo* ri, const wxJSONValue& val) {
                int align = val.AsInt(); // Generalement en degres (0..359 ?)
                ri->m_bearing_alignment.Update(align, RCS_MANUAL);
                ri->SetControlValue(CT_BEARING_ALIGNMENT, ri->m_bearing_alignment, nullptr);
                SendToDp(ri, {{"BearingAlignment", align}});
            }
        },
        {
            "TimedIdle",
            [this](RadarInfo* ri, const wxJSONValue& val) {
                // Generalement un “nombre d’unites” => 1..10
                int idle = val.AsInt(); 
                ri->m_timed_idle.Update(idle, RCS_MANUAL);
                ri->SetControlValue(CT_TIMED_IDLE, ri->m_timed_idle, nullptr);
                SendToDp(ri, {{"TimedIdle", idle}});
            }
        },
        {
            "TimedRun",
            [this](RadarInfo* ri, const wxJSONValue& val) {
                int run = val.AsInt();
                ri->m_timed_run.Update(run, RCS_MANUAL);
                ri->SetControlValue(CT_TIMED_RUN, ri->m_timed_run, nullptr);
                SendToDp(ri, {{"TimedRun", run}});
            }
        },
        {
            "Doppler",
            [this](RadarInfo* ri, const wxJSONValue& val) {
                int dop = val.AsInt();
                ri->m_doppler.Update(dop, RCS_MANUAL);
                ri->SetControlValue(CT_DOPPLER, ri->m_doppler, nullptr);
                SendToDp(ri, {{"Doppler", dop}});
            }
        },
        {
            "DopplerThreshold",
            [this](RadarInfo* ri, const wxJSONValue& val) {
                double th = val.IsDouble() ? val.AsDouble() : val.AsInt();
                // Contrôle éventuel 0..100
                int threshold = (int)th;
                ri->m_doppler_threshold.Update(threshold, RCS_MANUAL);
                ri->SetControlValue(CT_DOPPLER_THRESHOLD, ri->m_doppler_threshold, nullptr);
                SendToDp(ri, {{"DopplerThreshold", threshold}});
            }
        },
        {
            "AutoTrackDoppler",
            [this](RadarInfo* ri, const wxJSONValue& val) {
                bool autoDopp = val.AsBool();
                ri->m_autotrack_doppler.Update(autoDopp ? 1 : 0, RCS_MANUAL);
                ri->SetControlValue(CT_AUTOTTRACKDOPPLER, ri->m_autotrack_doppler, nullptr);
                SendToDp(ri, {{"AutoTrackDoppler", autoDopp}});
            }
        },
        {
            "TargetTrails",
            [this](RadarInfo* ri, const wxJSONValue& val) {
                int tt = val.AsInt(); // 0=OFF, 1=15s, 2=30s, 3=1min, etc.
                ri->m_target_trails.Update(tt, RCS_MANUAL);
                ri->SetControlValue(CT_TARGET_TRAILS, ri->m_target_trails, nullptr);
                SendToDp(ri, {{"TargetTrails", tt}});
            }
        },
        {/*
            "GuardZoneInnerRange",
            [this](RadarInfo* ri, const wxJSONValue& val) {
                int z = val["ZoneIndex"].AsInt();
                if (z < 0 || z >= GUARD_ZONES) return;
    
                GuardZone* gz = ri->m_guard_zone[z];
                int newInner = val["Value"].AsInt(); // (votre “Value”)
                gz->SetInnerRange(newInner);

                // Optionnel : renvoyer la valeur
                SendToDp(ri, {{"GuardZoneInnerRange", newInner}, {"ZoneIndex", z}});
            }
            */
        },
    
    };
}

void DpRadarCommand::SendToDp(RadarInfo* ri, std::initializer_list<std::pair<const wxString, wxVariant>> values)
{
    wxJSONValue root;

    root["RadarIndex"] = ri->m_radar;
  
    for (auto& kv : values) {
        const wxString& key = kv.first;
        const wxVariant& val = kv.second;

        if (val.GetType() == "bool") {
            root[key] = val.GetBool();
        }
        else if (val.GetType() == "long") {
            root[key] = (int)val.GetLong();
        }
        else if (val.GetType() == "double") {
            root[key] = val.GetDouble();
        }
        else if (val.GetType() == "string") {
            root[key] = val.GetString();
        }
        else {
            root[key] = val.MakeString();
        }
    }

    wxJSONWriter writer;
    wxString serializedMessage;
    writer.Write(root, serializedMessage);

    SendPluginMessage("DP_RADAR_PI", serializedMessage);
}

void DpRadarCommand::SendNewRadarInfo() {
  for (size_t r = 0; r < m_pi->m_settings.radar_count; r++) {
    if (m_pi->m_radar[r]) {
      SendToDp(m_pi->m_radar[r], {
                                     {"RadarType", (int)m_pi->m_radar[r]->m_radar_type},
                                     {"Transmit", (int)m_pi->m_radar[r]->m_state.GetValue()},
                                     {"range", (int)m_pi->m_radar[r]->m_range.GetValue()},
                                     {"gain", (int)m_pi->m_radar[r]->m_gain.GetValue()},
                                     {"RefreshWind", true}
                                 });
    }
  }
}



PLUGIN_END_NAMESPACE

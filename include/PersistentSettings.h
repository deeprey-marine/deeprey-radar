#pragma once

#define RADARS                                                                 \
    (1) // Arbitrary limit, anyone running this many is already crazy!

namespace RadarPlugin {
/**
 * The data that is stored in the opencpn.ini file. Most of this is set in the
 * OptionsDialog, some of it is 'secret' and can only be set by manipulating the
 * ini file directly.
 */
struct PersistentSettings {
    size_t radar_count; // How many radars we have
    RadarControlItem overlay_transparency; // How transparent is the radar
                                           // picture over the chart
    int range_index; // index into range array, see RadarInfo.cpp
    int verbose; // Loglevel 0..4.
    int guard_zone_threshold; // How many blobs must be sent by radar before we
                              // fire alarm
    int guard_zone_render_style; // 0 = Shading, 1 = Outline, 2 = Shading +
                                 // Outline
    int guard_zone_timeout; // How long before we warn again when bogeys are
                            // found
    bool guard_zone_on_overlay; // Show the guard zone on chart overlay?
    bool trails_on_overlay; // Show radar trails on chart overlay?
    bool overlay_on_standby; // Show guard zone when radar is in standby?
    int guard_zone_debug_inc; // Value to add on every cycle to guard zone
                              // bearings, for testing.
    double skew_factor; // Set to -1 or other value to correct skewing
    RangeUnits range_units; // See enum
    int max_age; // Scans older than this in seconds will be removed
    RadarControlItem refreshrate; // How quickly to refresh the display
    int menu_auto_hide; // 0 = none, 1 = 10s, 2 = 30s
    int drawing_method; // VertexBuffer, Shader, etc.
    bool developer_mode; // Readonly from config, allows head up mode
    bool show; // whether to show any radar (overlay or window)
    bool show_radar[RADARS]; // whether to show radar window
    bool dock_radar[RADARS]; // whether to dock radar window
    bool show_radar_control[RADARS]; // whether to show radar menu (control)
                                     // window
    int dock_size; // size of the docked radar
    bool transmit_radar[RADARS]; // whether radar should be transmitting
                                 // (persistent)
    bool pass_heading_to_opencpn; // Pass heading coming from radar as NMEA data
                                  // to OpenCPN
    bool enable_cog_heading; // Allow COG as heading. Should be taken out back
                             // and shot.
    bool ignore_radar_heading; // For testing purposes
    bool reverse_zoom; // false = normal, true = reverse
    bool show_extreme_range; // Show red ring at extreme range and center
    bool reset_radars; // True on exit of OptionsDialog when reset of radars is
                       // pressed
    int threshold_red; // Radar data has to be this strong to show as STRONG
    int threshold_green; // Radar data has to be this strong to show as
                         // INTERMEDIATE
    int threshold_blue; // Radar data has to be this strong to show as WEAK
    int threshold_multi_sweep; // Radar data has to be this strong not to be
                               // ignored in multisweep
    int threshold; // Radar pixels below this value are ignored (0..100% in
                   // steps of 10%)
    int type_detection_method; // 0 = default, 1 = ignore reports
    int AISatARPAoffset; // Rectangle side where to search AIS targets at ARPA
                         // position
    wxPoint control_pos[RADARS]; // Saved position of control menu windows
    wxPoint window_pos[RADARS]; // Saved position of radar windows, when
                                // floating and not docked
    wxPoint alarm_pos; // Saved position of alarm window
    wxString alert_audio_file; // Filepath of alarm audio file. Must be WAV.
    wxColour trail_start_colour; // Starting colour of a trail
    wxColour trail_end_colour; // Ending colour of a trail
    wxColour
        doppler_approaching_colour; // Colour for Doppler Approaching returns
    wxColour doppler_receding_colour; // Colour for Doppler Receding returns
    wxColour strong_colour; // Colour for STRONG returns
    wxColour intermediate_colour; // Colour for INTERMEDIATE returns
    wxColour weak_colour; // Colour for WEAK returns
    wxColour arpa_colour; // Colour for ARPA edges
    wxColour ais_text_colour; // Colour for AIS texts
    wxColour
        ppi_background_colour; // Colour for PPI background (normally very dark)
    double fixed_heading_value;
    bool fixed_heading;
    bool pos_is_fixed;
    GeoPosition fixed_pos;
    wxString radar_description_text;
    NetworkAddress target_mixer_address;
};

}

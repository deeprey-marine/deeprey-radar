#pragma once

#include <wx/jsonreader.h>
#include <wx/jsonval.h>
#include <wx/jsonwriter.h>

#include <unordered_map>
#include "ControlType.h"
#include "RadarState.h"
#include "RadarControlState.h"
#include <cstdint>

class ControlInfo;
class RadarControlItem;

class radar_pi;

class wxGLContext; // treat them as incomplete types
class wxSize;
class wxString;
class wxVariant;
class wxMouseEvent;

namespace RadarPlugin {
class GuardZone;
struct PersistentSettings;

/**
 * Interface for a plugin wanting to draw on the radar's OpenGL canvas.
 * The radar plugin calls OnRadarOverlayRender(...) during its GL draw cycle.
 *
 * We only pass pointers to wxGLContext and wxSize. We do NOT call their
 * methods, so we don't need the actual wxWidgets definitions here.
 */
struct IRadarOverlay {
    virtual ~IRadarOverlay() { }

    virtual void OnRadarOverlayRender(
        wxGLContext* pcontext, const wxSize* canvasSize, float radarRangePx)
        = 0;

    // NEW: Called to handle mouse clicks in the radar overlay
    virtual void OnMouseClick(wxMouseEvent& event) = 0;
};

class RadarAPI {
public:
    RadarAPI(radar_pi* radarPlugin);
    virtual ~RadarAPI();

    virtual void SetControlState(ControlType controlType, RadarControlState state);    
    virtual bool SetControl(ControlType controlType, int value, int controlIndex = 0);
    virtual void SetControlWithState(ControlType controlType, int value, RadarControlState state, int controlIndex = 0);
    virtual bool GetControl(ControlType controlType, int* value, RadarControlState* state, int controlIndex = 0);
    virtual int GetControl(ControlType controlType, int controlIndex = 0);
    virtual void AdjustRange(int adjustment);   
    virtual bool HasHeadingSource();

    virtual void Transmit(bool enable);
    virtual RadarState GetRadarState();

    // IRadarAPI interface:
    virtual void SetRadarRangeNM(double range_nm);
    virtual double GetRadarRangeNM() const;

    virtual void RegisterOverlayRenderer(IRadarOverlay* overlay);
    virtual void UnregisterOverlayRenderer(IRadarOverlay* overlay);
    /************************************************/

    virtual bool SelectRadarType(int type);
    virtual int GetRadarType();
    virtual uint64_t AddRadarTypeChangeCallback(std::function<void()> callback);
    virtual void RemoveRadarTypeChangeCallback(uint64_t callbackID);
    virtual uint64_t AddCanvasOverlayEnabledChangeCallback(std::function<void(int)> callback);
    virtual void RemoveCanvasOverlayEnabledChangeCallback(uint64_t callbackID);

    virtual ControlInfo* GetRadarControls();
    virtual GuardZone** GetGuardZones();
    virtual double GetRangeUnitsToMeters();
    virtual void ShowInfoDialog();
    virtual void ClearTrails();
    virtual PersistentSettings* GetSettings();
    virtual void ClearCursor();
    virtual void AcquireTarget();
    virtual void DeleteTarget();
    virtual void DeleteAllTargets();
    virtual void PlaceEblVrm(int index);
    virtual int GetArpaTargetsCount();
    virtual bool HasEblVrm(int bearingLineIndex);

    IRadarOverlay* GetOverlay() const { return m_overlay; }

    bool ProcessMessage(const wxString &message_id, const wxString &message_body);

    void SendPongMessage();

    virtual void SetNightMode(bool enabled);
    virtual bool IsNightModeEnabled() const { return m_nightModeEnabled; }

private:
    template <class T, class... Types>
    void CallCallbacks(std::unordered_map<uint64_t, std::function<T>>& callbacks,
        Types... args);    
    template <class T>
    uint64_t AddCallback(
        std::unordered_map<uint64_t, std::function<T>>& callbacks,
        std::function<T> callback);
    template <class T>
    void RemoveCallback(
        std::unordered_map<uint64_t, std::function<T>>& callbacks,
        uint64_t callbackID);

    RadarControlItem* GetControlItem(ControlType controlType, int controlIndex = 0);
    void SetControl(ControlType controlType, int value, RadarControlItem& controlItem);
    void SendMessageToDp(std::initializer_list<std::pair<const wxString, wxVariant>> values);
    
    radar_pi* m_pi;

    IRadarOverlay* m_overlay; // single overlay pointer
    std::unordered_map<uint64_t, std::function<void()>> m_radarTypeChangeCallbacks;
    std::unordered_map<uint64_t, std::function<void(int)>> m_canvasOverlayEnabledChangeCallbacks;
    uint64_t m_nextCallbackID;

    bool m_nightModeEnabled = false;
};

}

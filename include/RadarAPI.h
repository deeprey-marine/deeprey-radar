#pragma once
#include "RadarBridge.h" // so we know about IRadarAPI, etc.

#include <wx/jsonreader.h>
#include <wx/jsonval.h>
#include <wx/jsonwriter.h>

#include <unordered_map>

class radar_pi;

PLUGIN_BEGIN_NAMESPACE

class RadarAPI : public IRadarAPI {
public:
    RadarAPI(radar_pi* radarPlugin);
    virtual ~RadarAPI();

    virtual bool SetControl(const wxString& controlName, const wxVariant& value) override;
    virtual wxVariant GetControl(const wxString& controlName) override;

    virtual bool Transmit(bool enable) override;

    // IRadarAPI interface:
    virtual void SetRadarRangeNM(double range_nm) override;
    virtual double GetRadarRangeNM() const override;

    virtual void SetGain(int gain) override;
    virtual int GetGain() const override;

    virtual void RegisterOverlayRenderer(IRadarOverlay* overlay) override;
    virtual void UnregisterOverlayRenderer(IRadarOverlay* overlay) override;
    /************************************************/

    virtual bool SelectRadarType(int type) override;

    IRadarOverlay* GetOverlay() const { return m_overlay; }

    const std::unordered_map<wxString, ControlType>& GetControlTypeMap();
    ControlType StringToControlType(const wxString& controlTypeStr);

    bool ProcessMessage(const wxString &message_id, const wxString &message_body);

    void SendPongMessage();

private:

    void SendMessageToDp(std::initializer_list<std::pair<const wxString, wxVariant>> values);
    
    radar_pi* m_pi;

    IRadarOverlay* m_overlay; // single overlay pointer
};

PLUGIN_END_NAMESPACE
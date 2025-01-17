#pragma once
#include "RadarBridge.h" // so we know about IRadarAPI, etc.

class radar_pi;

PLUGIN_BEGIN_NAMESPACE

class RadarAPI : public IRadarAPI {
public:
    RadarAPI(radar_pi* radarPlugin);
    virtual ~RadarAPI();

    // IRadarAPI interface:
    virtual void SetRadarRangeNM(double range_nm) override;
    virtual double GetRadarRangeNM() const override;

    virtual void SetGain(int gain) override;
    virtual int GetGain() const override;

    virtual void RegisterOverlayRenderer(IRadarOverlay* overlay) override;
    virtual void UnregisterOverlayRenderer(IRadarOverlay* overlay) override;

    IRadarOverlay* GetOverlay() const { return m_overlay; }


private:
    
    radar_pi* m_radarPlugin;

    IRadarOverlay* m_overlay; // single overlay pointer
};

PLUGIN_END_NAMESPACE
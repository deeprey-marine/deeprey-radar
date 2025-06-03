#pragma once


namespace RadarPlugin {
//
// a RadarControlItem encapsulates a particular control, for instance
// sea clutter or gain.
//
// Some controls are always only just a value.
// Some other controls have state as well.

class RadarControlItem {
public:
    static const int VALUE_NOT_SET = -10000;

    RadarControlItem();

    // The copy constructor
    RadarControlItem(const RadarControlItem& other);

    // The assignment constructor
    RadarControlItem& operator=(const RadarControlItem& other);

    // The assignment constructor to allow "item = value"
    RadarControlItem& operator=(int v);

    void Update(int v, RadarControlState s);

    void UpdateState(RadarControlState s);
    void Update(int v);

    bool GetButton(int* value, RadarControlState* state);

    bool GetButton(int* value);

    int GetButton();

    int GetValue();

    RadarControlState GetState();

    bool IsModified();

    void SetMax(int max);
    void SetMin(int min);
    int GetMax();
    int GetMin();

    void TransformAndUpdate(int x);

    int DeTransform(int value);

protected:
    wxCriticalSection m_exclusive;
    int m_value;
    int m_button_v;
    RadarControlState m_state;
    RadarControlState m_button_s;
    bool m_mod;
    int m_max; // added for Raymarine
    int m_min;

public:
    double m_fraction;
};

}


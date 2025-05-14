#pragma once

class ControlInfo {
public:
    ControlType type;
    int autoValues;
    wxString* autoNames;
    bool hasOff;
    bool hasAutoAdjustable;
    int defaultValue;
    int minValue;
    int maxValue;
    int minAdjustValue;
    int maxAdjustValue;
    int stepValue;
    int nameCount;
    wxString unit;
    wxString* names;
};


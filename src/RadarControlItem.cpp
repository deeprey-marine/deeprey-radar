#include "RadarControlItem.h"

PLUGIN_BEGIN_NAMESPACE
RadarControlItem::RadarControlItem() {
  m_value = 0;
  m_state = RCS_OFF;
  m_button_v = VALUE_NOT_SET;  // Unlikely value so that first actual set
                               // sets proper value + mod
  m_button_s = RCS_OFF;
  m_mod = true;
  m_min = VALUE_NOT_SET;
  m_max = VALUE_NOT_SET;
  m_fraction = 0;
}

// The copy constructor
RadarControlItem::RadarControlItem(const RadarControlItem& other) { Update(other.m_value, other.m_state); }

// The assignment constructor
RadarControlItem& RadarControlItem::operator=(const RadarControlItem& other) {
  if (this != &other) {  // self-assignment check expected
    Update(other.m_value, other.m_state);
  }
  return *this;
}

// The assignment constructor to allow "item = value"
RadarControlItem& RadarControlItem::operator=(int v) {
  Update(v, RCS_MANUAL);
  return *this;
}

void RadarControlItem::Update(int v, RadarControlState s) {
  wxCriticalSectionLocker lock(m_exclusive);
  if (v != m_button_v || s != m_button_s) {
    m_mod = true;
    m_button_v = v;
    m_button_s = s;
  }
  m_value = v;
  m_state = s;
};

void RadarControlItem::UpdateState(RadarControlState s) {
  wxCriticalSectionLocker lock(m_exclusive);

  if (s != m_button_s) {
    m_mod = true;
    m_button_s = s;
  }
  m_state = s;
};

void RadarControlItem::Update(int v) { Update(v, RCS_MANUAL); };

bool RadarControlItem::GetButton(int* value, RadarControlState* state) {
  wxCriticalSectionLocker lock(m_exclusive);
  if (value) {
    *value = this->m_button_v;
  }
  if (state) {
    *state = this->m_button_s;
  }

  bool changed = m_mod;
  m_mod = false;
  return changed;
}

bool RadarControlItem::GetButton(int* value) {
  wxCriticalSectionLocker lock(m_exclusive);
  if (value) {
    *value = this->m_button_v;
  }

  bool changed = m_mod;
  m_mod = false;
  return changed;
}

int RadarControlItem::GetButton() {
  wxCriticalSectionLocker lock(m_exclusive);

  m_mod = false;
  return m_button_v;
}

int RadarControlItem::GetValue() {
  wxCriticalSectionLocker lock(m_exclusive);

  return m_value;
}

RadarControlState RadarControlItem::GetState() {
  wxCriticalSectionLocker lock(m_exclusive);

  return m_state;
}

bool RadarControlItem::IsModified() {
  wxCriticalSectionLocker lock(m_exclusive);

  return m_mod;
}

void RadarControlItem::SetMax(int max) { m_max = max; }
void RadarControlItem::SetMin(int min) { m_min = min; }
int RadarControlItem::GetMax() { return m_max; }
int RadarControlItem::GetMin() { return m_min; }

void RadarControlItem::TransformAndUpdate(int x) {  // Transforms the received value to the value shown on the button for
                                  // Raymarine
  if (m_max == VALUE_NOT_SET || m_min == VALUE_NOT_SET || m_max == m_min) {
    Update(x);
    return;
  }
  double new_value = (double)((x - m_min) * 100.) / (m_max - m_min) + .5;
  Update((int)new_value);
  m_fraction = new_value - (double)m_value;
  // wxLogMessage(wxT("new_value=%f, m_value=%i, m_fraction=%f"),
  // new_value, m_value, m_fraction);
}

int RadarControlItem::DeTransform(int value) {  // Reverse transform, transforms value to value to be transmitted to radar
  if (m_max == VALUE_NOT_SET || m_min == VALUE_NOT_SET || m_max == m_min) {
    return m_value;
  }
  return (int)(((double)(value) + m_fraction - .5) * (m_max - m_min) / 100. + m_min + .5);
}

RadarRangeControlItem::RadarRangeControlItem() {
  m_value = 0;
  m_state = RCS_OFF;
  m_button_v = VALUE_NOT_SET;  // Unlikely value so that first actual set
                               // sets proper value + mod
  m_button_s = RCS_OFF;
  m_mod = true;
}

void RadarRangeControlItem::Update(int v) {
  wxCriticalSectionLocker lock(m_exclusive);

  if (v != m_button_v) {
    m_mod = true;
    m_button_v = v;
  }
  m_value = v;
};

PLUGIN_END_NAMESPACE

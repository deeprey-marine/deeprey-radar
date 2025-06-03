#pragma once

#ifndef __WXMSW__
#include <netinet/in.h>
#endif

namespace RadarPlugin {

struct PackedAddress;

class NetworkAddress {
public:
    NetworkAddress();

    NetworkAddress(PackedAddress packed);
    NetworkAddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint16_t p);

    NetworkAddress(const wxString str);

    bool operator<(const NetworkAddress& other) const;

    bool operator==(const NetworkAddress& other) const;

    NetworkAddress& operator=(const NetworkAddress& other);

    wxString to_string() const;
    wxString FormatNetworkAddress() const;

    wxString FormatNetworkAddressPort() const;

    struct sockaddr_in GetSockAddrIn() const;

    bool IsNull() const;

    struct in_addr addr;
    uint16_t port;
};

}

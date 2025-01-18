#ifndef _DP_RADAR_COMMAND_H_
#define _DP_RADAR_COMMAND_H_

#include <wx/string.h>
#include <wx/jsonreader.h>
#include <wx/jsonval.h>
#include <wx/jsonwriter.h>

#include "pi_common.h"

PLUGIN_BEGIN_NAMESPACE


class radar_pi;
class RadarInfo;

/**
 * \class DpRadarCommand
 * \brief Classe pour parser et exécuter des commandes radar via DP (messages).
 */
class DpRadarCommand
{
public:
    /**
     * \brief Constructeur
     * \param plugin Pointeur vers l'instance radar_pi
     */
    explicit DpRadarCommand(radar_pi* plugin);

    /**
     * \brief Destructeur
     */
    ~DpRadarCommand();

    /**
     * \brief Parse et exécute un message (sous forme ID + contenu).
     *
     * \param message_id    Identifiant du message (ex: "DP_RADAR_CONTROL", etc.)
     * \param message_body  Le corps du message (ex: JSON)
     * \return true si le message est géré, false sinon
     */
    bool ProcessMessage(const wxString &message_id, const wxString &message_body);

    void SendNewRadarInfo();

private:
    radar_pi* m_pi; ///< Pointeur vers le plugin radar, pour accéder à m_settings, m_radar, etc.

    /**
     * \brief Exemple de fonction "helper" pour gérer un type de commande
     *
     * \param name   Nom de la commande (ex: "Range", "Gain", "Transmit")
     * \param value  Valeur associée
     */

    void HandleCommand(const wxString &name, const wxJSONValue &value, const int radarIndex);

    void initActions();

    void SendToDp(RadarInfo* ri, std::initializer_list<std::pair<const wxString, wxVariant>> values);

    std::unordered_map<wxString, std::function<void(RadarInfo*, const wxJSONValue&)>> m_actions;

};

PLUGIN_END_NAMESPACE

#endif // _DP_RADAR_COMMAND_H_

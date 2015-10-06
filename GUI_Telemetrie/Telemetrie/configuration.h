#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "datatypes.h"
#include <QString>

// location of the config file
#define CONFIG_FILE_STD_LOC L"!config"
#define CONFIG_FILE_DEFAULT L"!default"
/**
 * @brief Kurze beschreibung
 * @author
 * @warning
 *
 * lange
 */
class config_handler
{
public:
    /**
     * @brief config_handler
     */
    config_handler(); ///< kurze beschreibung
    ~config_handler();

    static void init_config (); // initializes the default config
    /**
     * @brief funktionsdoku
     * @param configuration dddd
     * @param file Erklärung
     * @return Rückgabewert
     *
     * TEXT
     */
    static int load_config (configdata *configuration, std::wstring file);
    static int save_config (configdata *configuration, std::wstring file);

    static configdata currentconfig;
private:
    static int interprete (configdata *config, QString buf1, QString buf2, int line);
    static configdata defaultconfig;
};

#endif // CONFIGURATION_H

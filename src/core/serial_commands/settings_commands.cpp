#include "settings_commands.h"
#include <globals.h>

uint32_t settingsCallback(cmd *c) {
    Command cmd(c);

    Argument setting_name_arg = cmd.getArgument("setting_name");
    Argument setting_value_arg = cmd.getArgument("setting_value");
    String setting_name = setting_name_arg.getValue();
    String setting_value = setting_value_arg.getValue();
    setting_name.trim();
    setting_value.trim();

    JsonDocument jsonDoc = viperConfig.toJson();
    JsonObject setting = jsonDoc.as<JsonObject>();

    if (setting_name.length() == 0 && setting_value.length() == 0) {
        // no args, just prints current config
        serializeJsonPretty(jsonDoc, Serial);
        serialDevice->println("");
        return true;
    }

    // Pin/device settings live in viperConfigPins, which is not part of
    // viperConfig.toJson(); handle them before the viperConfig whitelist check
    // below (otherwise they are wrongly rejected as "Invalid field name").
    {
        bool isPinField = true;
        if (setting_name == "bleName") viperConfigPins.setBleName(setting_value);
        else if (setting_name == "irTx") viperConfigPins.setIrTxPin(setting_value.toInt());
        else if (setting_name == "irTxRepeats")
            viperConfigPins.setIrTxRepeats(static_cast<uint8_t>(setting_value.toInt()));
        else if (setting_name == "irRx") viperConfigPins.setIrRxPin(setting_value.toInt());
        else if (setting_name == "rfTx") viperConfigPins.setRfTxPin(setting_value.toInt());
        else if (setting_name == "rfRx") viperConfigPins.setRfRxPin(setting_value.toInt());
        else if (setting_name == "rfModule")
            viperConfigPins.setRfModule(static_cast<RFModules>(setting_value.toInt()));
        else if (setting_name == "rfFreq") viperConfigPins.setRfFreq(setting_value.toFloat());
        else if (setting_name == "rfFxdFreq") viperConfigPins.setRfFxdFreq(setting_value.toInt());
        else if (setting_name == "rfScanRange") viperConfigPins.setRfScanRange(setting_value.toInt());
        else if (setting_name == "rfidModule")
            viperConfigPins.setRfidModule(static_cast<RFIDModules>(setting_value.toInt()));
        else isPinField = false;
        if (isPinField) {
            serialDevice->println(setting_name + " = " + setting_value);
            return true;
        }
    }

    if (setting[setting_name].isNull()) {
        serialDevice->println("Invalid field name: " + setting_name);
        return false;
    }

    if (setting_value.length() == 0) {
        serialDevice->print(setting_name + " = ");
        serialDevice->println(setting[setting_name].as<String>());
        return true;
    }

    // TODO: improve this logic and move to ViperConfig
    if (setting_name == "priColor") viperConfig.setUiColor(setting_value.toInt());
    if (setting_name == "rot") viperConfigPins.setRotation(setting_value.toInt());
    if (setting_name == "dimmerSet") viperConfig.setDimmer(setting_value.toInt());
    if (setting_name == "bright") viperConfig.setBright(setting_value.toInt());
    if (setting_name == "tmz") viperConfig.setTmz(setting_value.toFloat());
    if (setting_name == "soundEnabled") viperConfig.setSoundEnabled(setting_value.toInt());
    if (setting_name == "wifiAtStartup") viperConfig.setWifiAtStartup(setting_value.toInt());
    if (setting_name == "webUI") {
        viperConfig.setWebUICreds(
            setting_value.substring(0, setting_value.indexOf(",")),
            setting_value.substring(setting_value.indexOf(",") + 1)
        );
    }
    if (setting_name == "wifiAp") {
        viperConfig.setWifiApCreds(
            setting_value.substring(0, setting_value.indexOf(",")),
            setting_value.substring(setting_value.indexOf(",") + 1)
        );
    }
    if (setting_name == "wifi") {
        viperConfig.addWifiCredential(
            setting_value.substring(0, setting_value.indexOf(",")),
            setting_value.substring(setting_value.indexOf(",") + 1)
        );
    }
    if (setting_name == "wigleBasicToken") viperConfig.setWigleBasicToken(setting_value);
    if (setting_name == "wdgwarsApiKey") viperConfig.setWdgwarsApiKey(setting_value);
    if (setting_name == "devMode") viperConfig.setDevMode(setting_value.toInt());
    if (setting_name == "disabledMenus") viperConfig.addDisabledMenu(setting_value);

    return true;
}

uint32_t factoryResetCallback(cmd *c) {
    viperConfig.factoryReset();
    serialDevice->println("Factory reset done");
    return true;
}

void createSettingsCommands(SimpleCLI *cli) {
    cli->addCommand("factory_reset", factoryResetCallback);

    Command cmd = cli->addCommand("set/tings", settingsCallback);
    cmd.addPosArg("setting_name", "");
    cmd.addPosArg("setting_value", "");
}

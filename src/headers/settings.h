// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef SETTINGS_H
#define SETTINGS_H

typedef enum settings_status{
    SETTINGS_TOGGLEFULLSCREEN,
    SETTINGS_BACK,
    SETTINGS_QUIT,
    SETTINGS_NONE
}SettingsStatus;

SettingsStatus SettingsHandler(ToDraw *toDraw, SettingsFlags *settingsFlags);

#endif
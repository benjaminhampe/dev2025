#pragma once
#include <de/sound/Sound.h>

bool dbLoadSound( de::Sound & sound, const std::string& uri );




bool
dbSaveSound(
    const de::Sound & sound,
    const std::string& uri,
    const de::SoundSaveOptions& options = {});

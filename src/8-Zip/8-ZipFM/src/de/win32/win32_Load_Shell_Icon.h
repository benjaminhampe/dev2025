#pragma once
#include <de/image/Image.h>

// Get the icon the windows explorer shows for this file or directory.
// e.g.
// A directory probably gives the closed Directory icon.
// So an image file connected with IrfanView gives the IrfanView icon.
// So an video file connected with VLC gives the VLC icon.

std::shared_ptr<de::Image>
load_shell_icon(
    const std::string& uri,
    int ico_w = 0,
    int ico_h = 0,
    bool bOpenIcon = false);

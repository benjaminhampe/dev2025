#pragma once

// 8zipFM.exe --install
bool EightZip_Install();

// 8zipFM.exe --uninstall/ --deinstall
bool EightZip_Uninstall();

bool EightZip_isInstalled();

bool EightZip_isAdmin();


bool EightZip_Registry_updateExePath();

#pragma once
/*
🧰 Beispiel: Archiv‑Erstellungsdialog mit Fl_Double_Window
cpp

Fl_Double_Window* dlg = new Fl_Double_Window(420, 320, "Archiv erstellen");
dlg->set_modal();

Fl_Input* outName = new Fl_Input(120, 40, 250, 25, "Archivname:");
Fl_Choice* format = new Fl_Choice(120, 80, 250, 25, "Format:");
format->add("zip");
format->add("7z");
format->add("tar.zst");

Fl_Check_Button* encrypt = new Fl_Check_Button(120, 120, 250, 25, "Passwortschutz");

Fl_Input* password = new Fl_Input(120, 160, 250, 25, "Passwort:");
password->type(FL_SECRET_INPUT);

Fl_Button* ok = new Fl_Button(120, 220, 100, 30, "OK");
Fl_Button* cancel = new Fl_Button(230, 220, 100, 30, "Abbrechen");

dlg->end();
dlg->show();

🧩 How to emulate 7‑Zip‑style encryption for .tar.zst

You need a custom container format:
Container layout (simple, robust)
Code

struct MyEncryptedHeader {
    uint32_t magic = 0x455A5354;   // "EZST"
    uint8_t  version = 1;
    uint8_t  kdf_rounds;           // e.g. 24 → 2^24 iterations
    uint8_t  salt[16];
    uint8_t  iv[16];
    uint64_t encrypted_size;
    uint32_t crc32_of_ciphertext;
};

Then:
Code

AES-256-CBC(key, iv).encrypt(zstd_output)

Write:
Code

header + ciphertext

This gives you:

    deterministic structure

    easy detection

    safe encryption

    no metadata leaks

    no need for 7‑Zip compatibility

🔍 How to detect an encrypted archive

When opening a file:

    Read first 4 bytes

    Check for your magic number (EZST)

    If present → show password dialog

    If not → treat as plain .tar.zst

Example:
cpp

uint32_t magic;
file.read(&magic, sizeof(magic));
if (magic == 0x455A5354) {
    // encrypted
    showPasswordDialog();
}

This is exactly how 7‑Zip detects encrypted headers.
🔑 Password dialog logic

When user enters password:

    Derive key using your KDF (PBKDF2 or SHA‑256 iterative)

    Decrypt AES block

    Verify CRC

    If OK → decompress zstd → extract tar → done

    If fail → wrong password

*/

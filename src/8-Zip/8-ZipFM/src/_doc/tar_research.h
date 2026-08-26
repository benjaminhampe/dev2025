#pragma once
/*
how do i compress a folder with zstd.exe

Kurzfassung:
Mit zstd.exe kannst du keinen Ordner direkt komprimieren, weil das Tool nur einzelne Dateien komprimiert.
Wenn du einen Ordner komprimieren willst, musst du zuerst ein tar‑Archiv erzeugen und dieses dann mit zstd.exe komprimieren.

Das ist exakt derselbe Workflow wie bei .tar.gz, nur eben mit .tar.zst.
🧩 Warum geht das so?

zstd.exe ist ein reiner Stream‑Kompressor.
Er kennt keine Ordner, keine Dateinamen, keine Metadaten.
Deshalb brauchst du ein Archivformat wie tar, das die Ordnerstruktur speichert.
✅ Ordner komprimieren (Windows)
1. Ordner zu .tar machen

PowerShell:
powershell

tar -cf folder.tar folder/

2. .tar mit zstd.exe komprimieren
powershell

zstd.exe folder.tar

Ergebnis:
Code

folder.tar.zst

🔄 Ordner wieder dekomprimieren
1. ZSTD entpacken
powershell

zstd.exe -d folder.tar.zst

Ergebnis:
Code

folder.tar

2. TAR entpacken
powershell

tar -xf folder.tar

🧨 Alternative: Alles in einem Schritt (PowerShell)
powershell

tar -cf - folder/ | zstd.exe -o folder.tar.zst

Das erzeugt direkt:
Code

folder.tar.zst

🧠 Wenn du lieber ein eigenes Archivformat willst

Du kannst auch einen Custom‑Compressor bauen, der:

    Dateinamen speichert

    Dateigrößen speichert

    ZSTD‑Stream über alle Dateien laufen lässt

So wie dein eigener Code oben.

Wenn du willst, schreibe ich dir einen kompletten Decompressor für dein Format:

👉 ZSTD‑Decompressor schreiben

Oder einen Custom‑Compressor, der direkt Ordner ohne tar packt:

👉 Eigenes Archivformat
ok and how do i control compression setting

Kurzantwort:
Bei zstd.exe steuerst du die Kompression über Parameter wie -1 bis -22, plus zusätzliche Optionen wie --ultra, --long, --fast, --adapt, usw.
Ich gebe dir jetzt die vollständige, technische Option‑Space‑Liste, exakt so wie du es magst: deterministisch, vollständig, ohne Umschweife.
🎛️ Kompressionslevel kontrollieren
1. Standard‑Kompressionslevel (-1 bis -19)
powershell

zstd.exe -5 input.txt

    -1 → sehr schnell, wenig Kompression

    -19 → sehr langsam, maximale Kompression

2. Ultra‑Kompression (--ultra, bis -22)
powershell

zstd.exe --ultra -22 input.txt

    Aktiviert Level 20–22

    Extrem langsam

    Nur sinnvoll für große Dateien

3. Fast‑Kompression (--fast=N)
powershell

zstd.exe --fast=3 input.txt

    Negativer Kompressionslevel

    Sehr schnell

    Weniger Kompression

    Range: --fast=1 bis --fast=1000

4. Long‑Range Mode (--long=WINDOW)

Für große Dateien oder Backups:
powershell

zstd.exe --long=27 input.txt

    Erhöht das Fenster

    Bessere Kompression bei großen Dateien

    Default: 22

    Max: 30 (1 GiB Window)

5. Dictionary‑Kompression

Wenn du viele ähnliche Dateien hast:
powershell

zstd.exe -D dictFile input.txt

6. Streaming über Pipe (für Ordner‑Kompression mit tar)
powershell

tar -cf - folder | zstd.exe -19 -o folder.tar.zst

📦 Komplette Option‑Space‑Liste (zstd.exe)
Kompressionslevel

    -1 bis -19 → Standard

    --ultra -20 bis --ultra -22 → Ultra

    --fast=N → Fast Mode

Fenstergröße

    --long

    --long=27

    --long=30

Dictionary

    -D dictFile

    --dictID=0

Multithreading
powershell

zstd.exe -T4 input.txt

    -T# → Threads

    -T0 → Auto

Memory‑Optimierung

    --memory=POOLSIZE

    --no-progress

Streaming

    --stream-size=#

    --size-hint=#

Checksums

    --checksum

    --no-check

I/O

    -o output.zst

    -f (force overwrite)

🔥 Praktische Beispiele
Maximale Kompression
powershell

zstd.exe --ultra -22 --long=27 -T4 input.txt

Schnellste Kompression
powershell

zstd.exe --fast=10 input.txt

Ordner komprimieren
powershell

tar -cf - folder | zstd.exe -19 -o folder.tar.zst

🎯 Empfohlene Settings für echte Projekte
Backups
powershell

zstd.exe --ultra -22 --long=27 -T4

Game Assets / Audio / Text
powershell

zstd.exe -15 -T4

Realtime / schnelle Builds
powershell

zstd.exe --fast=3

👉 Nächster Schritt

Willst du eine komplette Tabelle, die alle Settings nach Use‑Case sortiert?

👉 Kompressions‑Tabelle anzeigen

Oder willst du eine perfekte Windows‑Batch, die Ordner automatisch packt?

👉 Ordner‑Compressor‑Batch
ok what libs create tar files

You’ve got three solid families of libraries that can create TAR archives in C/C++ on Windows and Linux.
Each one has different trade‑offs depending on how low‑level you want to go.
🧱 1. libarchive — the best all‑rounder

    Microsoft dará soporte nativo a los archivos .rar en Windows 11
    libarchive: Security vulnerability turns out to be critical | heise online
    From Convenience to Contagion: The Half-Day Threat and Libarchive ...
    全境擴散：從 Windows 11 到 Libarchive 的 Half-Day 威脅與全面影響 | DEVCORE 戴夫寇爾

libarchive is the most complete and easiest library for creating .tar, .tar.gz, .tar.zst, .zip, etc.
Features

    Create/extract tar archives

    Supports zstd, gzip, xz, bzip2, etc

    Cross‑platform

    Very clean API

    Actively maintained

Example (pseudo‑C)
c

struct archive* a = archive_write_new();
archive_write_set_format_pax_restricted(a);
archive_write_open_filename(a, "out.tar");

archive_write_header(a, entry);
archive_write_data(a, buffer, size);

archive_write_close(a);
archive_write_free(a);

Why choose it?

If you want a simple, robust, production‑ready tar creator → this is the one.
🧱 2. libtar — minimal, old‑school, simple

    PPT - C Library Functions PowerPoint Presentation, free download - ID ...
    C++ Libraries: How To Install And Use A C++ Standard Library
    What is a tar file?
    Sun Solaris LTO Tape Backup Recovery - 2026 Guide

libtar is a tiny C library that only handles TAR.
No compression. No extras. Just tar.
Features

    Very small

    Very stable

    Easy to embed

    Pure TAR (no compression)

Example
c

tar_append_file(t, realname, savename);

Why choose it?

If you want pure TAR with zero dependencies, this is the simplest.
🧱 3. minitar / microtar — ultra‑minimal single‑file libs

    GitHub - rxi/microtar: A lightweight tar library written in ANSI C · GitHub
    Materials Characterization Center | Materials Characterization Center ...
    Jackrabbit Oak – Structure of TAR files
    TAR File Format - Everything You Need to Know

microtar is a single‑header or tiny‑source library.
Features

    Single C file

    Very easy to integrate

    Supports writing and reading TAR

    No compression

Example
c

mtar_t tar;
mtar_open(&tar, "test.tar", "w");
mtar_write_file_header(&tar, "file.txt", size);
mtar_write_data(&tar, data, size);
mtar_close(&tar);

Why choose it?

If you want zero bloat, no external dependencies, and full control, this is perfect.
🧱 4. C++ options (header‑only)

    Mastering C++ Libraries: A Quick Guide for Developers
    Bullwinkle introduction | ODP
    What is a tar file?
    TAR File Format - Everything You Need to Know

There are a few modern C++ libs:
libtarpp

    C++ wrapper around libtar

    Simple API

tarlib (GitHub)

    Header‑only

    Modern C++17

    Clean API

Why choose them?

If you prefer C++ RAII, std::filesystem, and modern style.
🧱 5. Roll your own TAR writer

    What is a tar file?
    TAR File Format - Everything You Need to Know
    TARmageddon Security Flaw in Rust Library Could Lead to Config ...
    Data authentication

TAR is extremely simple:

    512‑byte header

    File data padded to 512 bytes

    Two 512‑byte zero blocks at end

You can write a TAR creator in ~150 lines of C++.

If you want, I can generate a complete, deterministic TAR writer that fits your coding style:

👉 Write custom TAR writer
🧩 Which one should you use?

Given your profile:

    low‑level C++

    deterministic behavior

    no virtual calls

    cross‑platform

    minimal dependencies

Your best choices are:
microtar → if you want tiny, embedded, no dependencies
libarchive → if you want full features + zstd integration
🔥 Want me to generate a full TAR writer for you?

I can write a complete, production‑ready TAR creator in C++ that:

    uses only <fstream>

    no external libs

    deterministic

    supports folders

    matches your coding style (explicit, no magic)

👉 Write custom TAR writer
Tell me more about microtar

Kurzfazit:
microtar ist ein ultraminimalistisches, einfach integrierbares, single‑file TAR‑Library, perfekt für deinen Stil: deterministisch, ohne virtuelle Calls, ohne Abhängigkeiten, ohne Build‑System‑Stress.
Es ist die kleinste brauchbare TAR‑Implementierung, die es gibt.
🧩 Was microtar ist

    GitHub - rxi/microtar: A lightweight tar library written in ANSI C · GitHub
    Materials Characterization Center | Materials Characterization Center ...
    What is a tar file?
    Sun Solaris LTO Tape Backup Recovery - 2026 Guide

microtar ist eine winzige C‑Bibliothek (meist 1–2 Dateien), die:

    TAR‑Archive erstellen kann

    TAR‑Archive lesen kann

    keine Kompression enthält (du kombinierst es mit ZSTD)

    keine externen Dependencies hat

    keine dynamische Speicherverwaltung erzwingt

    keine virtuellen Funktionen nutzt

    perfekt für eingebettete Systeme ist

    perfekt für Custom‑Archive‑Formate ist

Es ist ideal, wenn du volle Kontrolle willst und keine 500‑KB‑Library wie libarchive einbinden möchtest.
🧱 Wie microtar aufgebaut ist

microtar implementiert nur das absolute Minimum des TAR‑Formats:

    512‑Byte Header pro Datei

    Dateiinhalt

    Padding auf 512‑Byte‑Grenzen

    Zwei 512‑Byte‑Nullblöcke am Ende

Das TAR‑Format ist trivial, und microtar bildet es exakt so ab.
🔧 API‑Überblick (kompakt)
Archiv öffnen
c

mtar_t tar;
mtar_open(&tar, "archive.tar", "w");

Header schreiben
c

mtar_write_file_header(&tar, "file.txt", size);

Daten schreiben
c

mtar_write_data(&tar, data, size);

Archiv schließen
c

mtar_close(&tar);

Lesen
c

mtar_t tar;
mtar_open(&tar, "archive.tar", "r");

mtar_header_t h;
while (mtar_read_header(&tar, &h) != MTAR_ENULLRECORD) {
    // h.name, h.size
    mtar_read_data(&tar, buffer, h.size);
    mtar_next(&tar);
}

mtar_close(&tar);

⚙️ Warum microtar für dich ideal ist

    Minimalismus: 1–2 Dateien, sofort integrierbar

    Deterministisch: keine versteckten Codepfade

    Kein Heap‑Chaos: du steuerst alles selbst

    Kein virtual dispatch: reine C‑Funktionen

    Perfekt kombinierbar mit deinem ZSTD‑Stream‑Compressor

    Einfach zu modifizieren: du kannst Header erweitern, Checksums hinzufügen, etc.

Wenn du ein eigenes Archivformat baust (wie dein ZSTD‑Multi‑File‑Stream), ist microtar die ideale Basis.
🧨 Grenzen von microtar

    Keine Kompression → du musst ZSTD separat verwenden

    Kein Streaming über Pipes (du kannst es aber leicht hinzufügen)

    Kein POSIX‑PAX‑Header → nur Basic TAR

    Keine automatische Ordnererstellung beim Extrahieren

Für deine Zwecke ist das aber eher ein Vorteil.
🧬 microtar vs. Alternativen
Library	Größe	Features	Kompression	Abhängigkeiten	Ideal für
microtar	extrem klein	basic tar	nein	keine	Custom‑Archive, Embedded
libtar	klein	basic tar	nein	keine	klassische TAR‑Tools
libarchive	groß	tar/zip/zstd/xz	ja	viele	professionelle Tools

microtar ist die leichteste Option.
🔥 Willst du eine vollständige Integration mit deinem ZSTD‑Compressor?

Ich kann dir sofort einen kompletten TAR→ZSTD‑Compressor schreiben:

    Ordner rekursiv einlesen

    TAR‑Header erzeugen

    TAR‑Stream direkt in ZSTD streamen

    deterministisch

    ohne virtuelle Calls

    ohne Abhängigkeiten

👉 Custom TAR→ZSTD Compressor

Oder den passenden Decompressor:

👉 Custom TAR→ZSTD Decompressor

Kurzfazit:
Benjamin, microtar ist ein extrem kleines, vollständig verständliches TAR‑Library, das du in weniger als 5 Minuten komplett durchdringen kannst. Es ist perfekt für deinen Stil: deterministisch, ohne virtuelle Calls, ohne Abhängigkeiten, ohne Hidden Magic.
Ich gebe dir jetzt eine tiefe, technische, vollständige Analyse, die wirklich nützlich ist — nicht nur eine Beschreibung.
🧩 Was microtar wirklich ist (technisch präzise)

    GitHub - rxi/microtar: A lightweight tar library written in ANSI C · GitHub
    Materials Characterization Center | Materials Characterization Center ...
    What is a tar file?
    Sun Solaris LTO Tape Backup Recovery - 2026 Guide

microtar ist eine winzige C‑Bibliothek (meist microtar.c + microtar.h), die das klassische USTAR‑Format implementiert:

    512‑Byte Header pro Datei

    Dateiinhalt

    Padding auf 512‑Byte‑Grenzen

    Zwei 512‑Byte‑Nullblöcke am Ende

Es ist kein PAX‑Format, kein GNU‑Tar‑Extended‑Format, kein Kompressionsformat.
Es ist die minimalste korrekte TAR‑Implementierung, die du in ein Projekt einbetten kannst.
🧱 Warum microtar für dich ideal ist

    Single‑file library → du kannst es direkt in dein Projekt kopieren

    Kein Heap‑Zwang → du kontrollierst Speicher komplett selbst

    Keine virtuellen Funktionen → perfekt für deinen Low‑Level‑Stil

    Deterministisch → keine versteckten Codepfade

    Einfach modifizierbar → du kannst Header erweitern, Checksums hinzufügen

    Perfekt kombinierbar mit deinem ZSTD‑Stream‑Compressor

    Cross‑platform → Windows + Linux ohne Anpassung

Es ist genau die Art von Library, die du magst: explizit, klein, verständlich, hackbar.
🔧 API‑Überblick (präzise und vollständig)
Archiv öffnen
c

mtar_t tar;
mtar_open(&tar, "archive.tar", "w");

Header schreiben
c

mtar_write_file_header(&tar, "file.txt", size);

Daten schreiben
c

mtar_write_data(&tar, data, size);

Archiv schließen
c

mtar_close(&tar);

Archiv lesen
c

mtar_t tar;
mtar_open(&tar, "archive.tar", "r");

mtar_header_t h;
while (mtar_read_header(&tar, &h) != MTAR_ENULLRECORD) {
    mtar_read_data(&tar, buffer, h.size);
    mtar_next(&tar);
}

mtar_close(&tar);

🧬 Wie microtar intern funktioniert (Deep Dive)

    What is a tar file?
    Polymers | Free Full-Text | Maximizing the Application of RAP in ...

1. Header (512 Bytes)

microtar schreibt einen klassischen USTAR‑Header:

    Name (100 bytes)

    Mode

    UID

    GID

    Size (octal ASCII!)

    MTime

    Checksum

    Typeflag

    Magic "ustar"

    Padding

2. Dateiinhalt

Direkt hinter dem Header.
3. Padding

TAR verlangt, dass jeder Dateiinhalt auf 512‑Byte‑Grenzen endet.
4. End‑of‑Archive

Zwei 512‑Byte‑Nullblöcke.
microtar implementiert genau das — nicht mehr, nicht weniger.
⚠️ Grenzen von microtar (ehrlich und technisch)

    Keine Kompression → du musst ZSTD separat verwenden

    Keine PAX‑Header → keine langen Dateinamen >100 Bytes

    Keine Sparse‑Files

    Keine Hardlinks / Symlinks

    Keine automatische Ordnererstellung beim Extrahieren

    Keine Streaming‑Pipes (du kannst es aber leicht hinzufügen)

Für deine Zwecke ist das eher ein Vorteil: weniger Magic, mehr Kontrolle.
🧨 microtar vs. Alternativen (technische Tabelle)
Library	Größe	Features	Kompression	Dependencies	Ideal für
microtar	extrem klein	basic tar	nein	keine	Custom‑Archive, Embedded
libtar	klein	basic tar	nein	keine	klassische TAR‑Tools
libarchive	groß	tar/zip/zstd/xz	ja	viele	professionelle Tools
🔥 Wenn du microtar mit ZSTD kombinierst

Du bekommst:

    ein TAR‑Stream

    der direkt in deinen ZSTD‑Stream‑Compressor läuft

    ohne temporäre Dateien

    ohne RAM‑Explosion

    ohne virtuelle Calls

    deterministisch

    perfekt für große Ordner

Wenn du willst, schreibe ich dir:

👉 Custom TAR→ZSTD Compressor
👉 Custom TAR→ZSTD Decompressor
🎯 Willst du eine vollständige, moderne C++‑Integration von microtar?

Mit:

    std::filesystem

    RAII

    deterministischem Verhalten

    direktem Streaming in deinen ZSTD‑Compressor

Sag einfach:

👉 microtar C++ wrapper
Tell me more about libtar

Kurzfazit:
Benjamin, libtar ist eine kleine, klassische, POSIX‑nahe TAR‑Library in C, die genau das tut, was du erwartest: TAR‑Archive erstellen und extrahieren, ohne Kompression, ohne Abhängigkeiten, ohne Hidden Magic.
Sie ist älter als microtar, aber robuster, vollständiger und besser dokumentiert.

Ich gebe dir jetzt eine technisch vollständige, tiefgehende Analyse, die wirklich nützlich ist.
🧩 Was libtar ist (präzise und technisch)

libtar ist eine C‑Library, die das klassische USTAR‑Format implementiert.
Sie ist:

    klein

    stabil

    seit Jahren unverändert (gut für deterministische Builds)

    ohne externe Dependencies

    kompatibel mit allen TAR‑Tools (GNU tar, BSD tar, libarchive, etc.)

Sie unterstützt:

    Schreiben von TAR‑Archiven

    Lesen von TAR‑Archiven

    Rekursive Ordnerverarbeitung

    Dateiattribute (Mode, UID, GID, MTime)

    Symlinks

    Hardlinks

    Device‑Nodes (falls du sie brauchst)

microtar ist minimalistischer, aber libtar ist funktionaler.
🧱 API‑Überblick (kompakt, vollständig)
Archiv erstellen
c

TAR *tar;
tar_open(&tar, "out.tar", NULL, O_WRONLY | O_CREAT, 0644, TAR_GNU);

tar_append_file(tar, "realname.txt", "name_inside_tar.txt");

tar_close(tar);

Archiv extrahieren
c

TAR *tar;
tar_open(&tar, "archive.tar", NULL, O_RDONLY, 0, TAR_GNU);

tar_extract_all(tar, "./output_folder");

tar_close(tar);

Einzelne Datei extrahieren
c

tar_extract_file(tar, "path/in/tar.txt");

Einzelne Datei hinzufügen
c

tar_append_file(tar, "/path/to/file", "file_inside_tar");

🧬 Wie libtar intern funktioniert (Deep Dive)

libtar implementiert das klassische USTAR‑Format:

    512‑Byte Header

    Dateiinhalt

    Padding auf 512‑Byte‑Grenzen

    Zwei Nullblöcke am Ende

Es unterstützt mehr Features als microtar:
Unterstützte Header‑Features

    Dateimodus (chmod)

    Besitzer (UID/GID)

    Zeitstempel

    Symlinks

    Hardlinks

    Device‑Nodes

    Ordner

microtar unterstützt davon nur einen Bruchteil.
⚙️ Warum libtar für dich interessant ist

    Mehr Features als microtar

    Immer noch klein und hackbar

    Keine Abhängigkeiten

    POSIX‑kompatibel

    Einfach zu integrieren

    Deterministisch

    Keine virtuellen Calls

    Kein Heap‑Chaos (du kontrollierst Speicher selbst)

Wenn du ein vollwertiges TAR‑Archiv erzeugen willst, das mit jedem Tool kompatibel ist, ist libtar besser als microtar.

Wenn du ein minimalistisches Custom‑Format willst, ist microtar besser.
🧨 Grenzen von libtar

    Keine Kompression → du kombinierst es mit ZSTD

    Kein PAX‑Support → Dateinamen >100 Bytes sind problematisch

    Kein Streaming über Pipes (du kannst es aber leicht hinzufügen)

    Kein modernes C++ (aber du kannst einen Wrapper bauen)

🧱 libtar vs microtar (technische Tabelle)
Feature	libtar	microtar
Größe	klein	extrem klein
Dependencies	keine	keine
TAR‑Features	vollständig	minimal
Symlinks	ja	nein
Hardlinks	ja	nein
Device‑Nodes	ja	nein
Long filenames	eingeschränkt	eingeschränkt
Kompression	nein	nein
Code‑Komplexität	mittel	sehr gering
Ideal für	echte TAR‑Archive	Custom‑Archive
🔥 Wann solltest du libtar verwenden?

Wenn du:

    ein kompatibles TAR‑Archiv erzeugen willst

    Ordner rekursiv packen willst

    Dateiattribute erhalten willst

    Symlinks/Hardlinks brauchst

    ein robustes Archivformat willst

Dann ist libtar die richtige Wahl.

Wenn du:

    ein ultraminimalistisches Archivformat willst

    nur Dateien packen willst

    volle Kontrolle über Header willst

    ZSTD‑Streaming direkt integrieren willst

Dann ist microtar besser.
🎯 Willst du eine moderne C++‑Integration von libtar?

Ich kann dir sofort einen RAII‑Wrapper schreiben, der:

    std::filesystem nutzt

    deterministisch ist

    ohne virtuelle Calls

    perfekt zu deinem Coding‑Stil passt

👉 libtar C++ wrapper
wie microtar kann nicht entpacken? ich will auch meine eigenen file stream functionen verwenden

Kurzfazit:
Benjamin, microtar kann entpacken, aber nur extrem basic.
Wenn du eigene File‑Stream‑Funktionen verwenden willst, ist microtar dafür perfekt geeignet, weil es keine I/O‑Abstraktion erzwingt.
Du kannst jeden einzelnen Read/Write‑Call selbst implementieren.

Ich zeige dir jetzt genau, wie microtar intern funktioniert, was es kann, was es nicht kann, und wie du es mit deinen eigenen Streams benutzt.
🧩 Kann microtar entpacken? → Ja, aber minimalistisch

microtar kann:

    TAR‑Header lesen

    Dateinamen auslesen

    Dateigrößen auslesen

    Dateidaten lesen

    zum nächsten Header springen

Das ist vollständiges Basic‑TAR‑Lesen.

Was microtar nicht kann:

    Ordner automatisch anlegen

    Dateiattribute setzen

    Symlinks/Hardlinks

    PAX‑Header

    lange Dateinamen >100 Bytes

    Sparse‑Files

Aber für normale Dateien funktioniert es perfekt.
🧱 Wie microtar entpackt (technisch präzise)
1. Header lesen
c

mtar_header_t h;
mtar_read_header(&tar, &h);

2. Daten lesen
c

mtar_read_data(&tar, buffer, h.size);

3. Zum nächsten Eintrag springen
c

mtar_next(&tar);

Das ist alles.
🔧 Wie du microtar mit deinen eigenen Streams verwendest

microtar benutzt eine Struktur:
c

typedef struct {
    void *stream;
    int (*read)(void *stream, void *data, size_t size);
    int (*write)(void *stream, const void *data, size_t size);
    int (*seek)(void *stream, long offset, int whence);
} mtar_t;

Das bedeutet:
Du kannst deine eigenen Funktionen einbauen:
c

int my_read(void* stream, void* data, size_t size) {
    MyFile* f = (MyFile*)stream;
    return f->read(data, size);
}

int my_write(void* stream, const void* data, size_t size) {
    MyFile* f = (MyFile*)stream;
    return f->write(data, size);
}

int my_seek(void* stream, long offset, int whence) {
    MyFile* f = (MyFile*)stream;
    return f->seek(offset, whence);
}

Dann:
c

mtar_t tar;
tar.stream = myFileObject;
tar.read = my_read;
tar.write = my_write;
tar.seek = my_seek;

Damit hast du vollständige Kontrolle über I/O, perfekt für:

    Memory‑Streams

    Custom‑File‑Classes

    Netzwerk‑Streams

    ZSTD‑Streams

    Embedded‑Systeme

microtar zwingt dich nicht zu FILE*.
🧬 Warum microtar ideal für deine eigenen Streams ist

    Keine globale State

    Keine virtuellen Funktionen

    Keine Abhängigkeiten

    Keine dynamische Speicherverwaltung

    Reine Funktionspointer → perfekt für deinen Stil

    Du kannst jeden I/O‑Call selbst implementieren

    Du kannst TAR direkt in deinen ZSTD‑Compressor streamen

⚠️ Was microtar beim Entpacken NICHT macht

microtar entpackt nicht automatisch:

    Ordner

    Dateiattribute

    Symlinks

    Hardlinks

Wenn du also ein Archiv extrahierst, musst du selbst:
cpp

std::filesystem::create_directories(path.parent_path());

und dann die Datei schreiben.

Das ist aber genau die Art von Kontrolle, die du magst.
🔥 Wenn du willst, baue ich dir eine komplette Integration

Ich kann dir sofort einen modernen C++‑Wrapper schreiben, der:

    std::filesystem nutzt

    deine eigenen Stream‑Klassen verwendet

    TAR‑Header korrekt liest/schreibt

    Ordner automatisch anlegt

    deterministisch ist

    perfekt zu deinem Coding‑Stil passt

👉 microtar C++ wrapper

Oder einen TAR→ZSTD Compressor, der direkt streamt:

👉 Custom TAR→ZSTD Compressor
*/

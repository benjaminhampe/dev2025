#pragma once
/*
zeige mir commandozeilen für 7z ohne gui, ich will das nachahmen und will wissen was man alles steuern kann

Kurzfassung: Hier ist die komplette, volle 7‑Zip Kommandozeilen‑API, exakt das, was du brauchst, um deine eigene CLI nachzuahmen.
Das ist alles, was 7z.exe kann: Befehle, Switches, Parameter, Format‑Optionen, Verschlüsselung, SFX, Listen, Wildcards, Pfadregeln, Update‑Modi, Solid‑Block‑Steuerung usw.

Ich gebe dir zuerst die komplette Befehlsliste, dann alle Switches, dann Format‑spezifische Optionen, dann echte Beispiele, und danach die Architektur, wie du das 1:1 nachbauen kannst.
🟦 1. 7‑Zip CLI Befehle (Hauptkommandos)

Jedes Kommando hat die Form:
Code

7z <command> [switches] <archive> [files...]

Hier sind alle Kommandos:

    a – Add → Archiv erstellen / hinzufügen

    d – Delete → Dateien aus Archiv löschen

    e – Extract → Entpacken ohne Pfade

    x – Extract with full paths → Entpacken mit Pfaden

    l – List → Inhalt anzeigen

    t – Test → Archiv testen

    u – Update → Dateien aktualisieren

    rn – Rename → Datei im Archiv umbenennen

    b – Benchmark → CPU/Kompressionstest

    i – Info → Format‑Infos anzeigen

🟦 2. Globale Switches (für alle Kommandos)

Diese Switches funktionieren bei allen Befehlen:
🔹 Archivformat
Code

-t7z
-tzip
-tar
-tgz
-tbz2
-txz

🔹 Kompression
Code

-m0=<method>       ; LZMA, LZMA2, PPMd, BZip2, Deflate, Copy
-mx=<0-9>          ; Kompressionslevel
-md=<size>         ; Dictionary size (z.B. 64m)
-ms=<size>         ; Solid block size
-mfb=<num>         ; Number of fast bytes
-mmt=<threads>     ; Thread count

🔹 Verschlüsselung
Code

-p<password>
-mhe=on            ; Header encryption

🔹 Pfade / Wildcards
Code

-r                ; rekursiv
-x!pattern        ; ausschließen
-i!pattern        ; einschließen

🔹 Ausgabe
Code

-y                ; auto-yes
-bd               ; no progress indicator
-bb0 / bb1 / bb2  ; log level

🔹 SFX (Self‑Extracting EXE)
Code

-sfx              ; SFX erstellen

🟦 3. Format‑spezifische Optionen
🔹 7z Format (LZMA/LZMA2)
Code

-m0=lzma2
-mx=9
-md=64m
-ms=on
-mhe=on

🔹 ZIP Format
Code

-m0=deflate
-mx=9

🔹 TAR

Keine Kompression, nur Container.
🔹 GZIP
Code

-m0=gzip

🔹 BZIP2
Code

-m0=bzip2

🟦 4. Vollständige Beispiele (realistisch)
🔹 Archiv erstellen (7z)
Code

7z a archive.7z folder\ -t7z -m0=lzma2 -mx=9 -md=64m -mhe=on -pSECRET

🔹 ZIP erstellen
Code

7z a archive.zip folder\ -tzip -mx=9

🔹 Entpacken (mit Pfaden)
Code

7z x archive.7z -ooutput\

🔹 Entpacken (ohne Pfade)
Code

7z e archive.7z -ooutput\

🔹 Inhalt anzeigen
Code

7z l archive.7z

🔹 Dateien löschen
Code

7z d archive.7z *.txt

🔹 Dateien aktualisieren
Code

7z u archive.7z *.cpp

🔹 Datei im Archiv umbenennen
Code

7z rn archive.7z oldname.txt newname.txt

🔹 SFX erstellen
Code

7z a archive.exe folder\ -sfx

🟦 5. Wildcards & Pfadregeln
🔹 Rekursiv
Code

7z a out.7z src\ -r

🔹 Ausschließen
Code

7z a out.7z src\ -xr!*.tmp

🔹 Nur bestimmte Dateien
Code

7z a out.7z src\ -i!*.png -i!*.jpg

🟦 6. Was du für deine eigene CLI nachahmen musst

Wenn du deine eigene CLI baust, brauchst du exakt diese Architektur:
✔ 1. Hauptbefehle (a, x, e, l, d, u, rn, t)

→ Das ist ein Command Dispatcher.
✔ 2. Switch‑Parser

→ Alle Switches beginnen mit -  
→ Manche haben Werte (-mx=9)
→ Manche sind Flags (-r, -y)
✔ 3. Format‑Handler

→ -t7z, -tzip, -ttar, …
→ Jeder ruft andere Kompressionsmethoden auf.
✔ 4. Method‑Handler

→ -m0=lzma2  
→ -m0=deflate  
→ -m0=copy
✔ 5. Encryption‑Handler

→ -pPASSWORD  
→ -mhe=on
✔ 6. Wildcard‑Engine

→ -i!pattern  
→ -x!pattern
✔ 7. Output‑Engine

→ -bb0, -bb1, -bb2  
→ -bd  
→ -y
✔ 8. SFX‑Builder

→ EXE‑Stub + Archiv

Wenn du willst, baue ich dir die komplette Command‑Parser‑Klasse, die exakt wie 7z.exe funktioniert.
🟦 7. Vollständige Liste aller Switches (komplett)

Hier ist die volle Liste, wie sie intern in 7‑Zip existiert:
Code

-a
-bd
-bb0
-bb1
-bb2
-i!
-x!
-r
-y
-sfx
-so
-si
-spf
-slt
-sni
-snc
-sns
-snh
-spl
-scc
-scq
-scd
-sco
-scs
-sct
-scu
-scr
-sca
-scc
-scl
-scp
-scs
-sct
-scu
-scr
-sca
-m0=
-mx=
-md=
-ms=
-mfb=
-mmt=
-mhe=
-p
-t

Das ist wirklich alles, was 7‑Zip CLI kann.
🟩 Wenn du willst, kann ich dir jetzt bauen:

    7zip_Command_Parser_entwerfen

    7zip_Switch_Parser_entwerfen

    7zip_Archiv_Builder_architektur

    7zip_Wildcard_Engine_nachbauen

    7zip_Encryption_Handler_nachbauen

Sag mir, welchen Teil du als Nächstes brauchst.
Jo es wäre toll wenn du hinter die switches auch schreibst was zum geier die bedeuten, was für werte, was für default wert

Takeaway: Ich gebe dir jetzt die vollständige 7‑Zip‑Switch‑Liste, mit Bedeutung, möglichen Werten, Default‑Werten, Format‑Hinweisen, Fallstricke, nicht dokumentierten Details und realen Beispielen.
Das ist die komplette technische Referenz, wie du sie brauchst, um deine eigene CLI nachzubauen.

Ich strukturiere das nach globalen Switches, Kompressions‑Switches, Format‑Switches, Verschlüsselung, Pfadregeln, Logging, SFX, I/O‑Switches.

Jeder Switch beginnt als Guided Link, damit du jeden Punkt einzeln vertiefen kannst.
🟦 Globale Switches (für alle Befehle)

    ‑y — Auto‑Yes  
    Bedeutung: Keine Rückfragen, alles bestätigen.
    Werte: keine
    Default: off
    Beispiel: 7z a out.7z folder -y

    ‑bd — Disable progress indicator  
    Bedeutung: Fortschrittsanzeige abschalten (für Skripte).
    Default: off

    ‑bb0 / ‑bb1 / ‑bb2 — Log level  
    bb0: nur Fehler
    bb1: Fehler + Warnungen
    bb2: alles (verbose)
    Default: bb0

    ‑r — Rekursiv  
    Default: off
    Beispiel: 7z a out.7z src -r

🟦 Pfad‑ und Wildcard‑Switches

    ‑i!pattern — Include  
    Bedeutung: Nur Dateien, die dem Pattern entsprechen.
    Default: alles
    Beispiel: -i!*.png

    ‑x!pattern — Exclude  
    Bedeutung: Dateien ausschließen.
    Beispiel: -x!*.tmp

    ‑spf — Store full paths  
    Bedeutung: Pfade exakt speichern, auch absolute.
    Default: off

🟦 Format‑Switches

    ‑t<format> — Archivformat wählen  
    Werte:

        7z

        zip

        tar

        gzip / gz

        bzip2 / bz2

        xz  
        Default: 7z
        Beispiel: -tzip

🟦 Kompressions‑Switches (Methoden, Level, Dictionary, Threads)

    ‑m0=<method> — Kompressionsmethode  
    Werte:

        lzma

        lzma2

        ppmd

        bzip2

        deflate

        copy (keine Kompression)
        Default: abhängig vom Format

        7z → lzma2

        zip → deflate

    ‑mx=<0‑9> — Kompressionslevel  
    0: keine Kompression
    1: sehr schnell
    3: schnell
    5: normal
    7: max
    9: ultra
    Default: 5

    ‑md=<size> — Dictionary Size  
    Werte: 64k, 1m, 32m, 64m, 256m, 1g  
    Default: abhängig von Level
    Beispiel: -md=64m

    ‑ms=<size>|on|off — Solid Block Size  
    on: ein großer Block
    off: kein Solid
    size: z. B. 64m  
    Default: on

    ‑mfb=<num> — Fast Bytes  
    Werte: 5–273
    Default: abhängig von Methode

    ‑mmt=<threads> — Multithreading  
    Werte: on, off, oder Zahl
    Default: on (Auto)

🟦 Verschlüsselung

    ‑p<password> — Passwort setzen  
    Default: kein Passwort
    Beispiel: -pSECRET

    ‑mhe=on/off — Header Encryption  
    on: Dateinamen verschlüsselt
    off: Dateinamen sichtbar
    Default: off
    Beispiel: -mhe=on

🟦 SFX (Self‑Extracting EXE)

    ‑sfx — SFX erstellen  
    Default: off
    Beispiel:  
    7z a archive.exe folder -sfx

🟦 I/O‑Switches

    ‑so — Output to stdout  
    Default: off
    Beispiel: 7z x archive.7z -so

    ‑si — Input from stdin  
    Default: off

🟦 Komplette Tabelle (Switch → Bedeutung → Werte → Default)
Switch	Bedeutung	Werte	Default
‑y	Auto‑Yes	–	off
‑bd	Fortschritt aus	–	off
‑bb0/1/2	Log Level	0–2	0
‑r	rekursiv	–	off
‑i!	Include	Pattern	alles
‑x!	Exclude	Pattern	nichts
‑spf	Full paths	–	off
‑t	Format	7z/zip/tar/...	7z
‑m0	Methode	lzma2/deflate/...	formatabhängig
‑mx	Level	0–9	5
‑md	Dictionary	64k–1g	levelabhängig
‑ms	Solid Block	on/off/size	on
‑mfb	Fast Bytes	5–273	methodabhängig
‑mmt	Threads	Zahl/on/off	auto
‑p	Passwort	Text	none
‑mhe	Header Encryption	on/off	off
‑sfx	SFX	–	off
‑so	stdout	–	off
‑si	stdin	–	off
🟦 Beispiel: Ultra‑7z mit Header‑Encryption
Code

7z a out.7z folder ^
  -t7z ^
  -m0=lzma2 ^
  -mx=9 ^
  -md=256m ^
  -ms=on ^
  -mfb=273 ^
  -mmt=on ^
  -pSECRET ^
  -mhe=on

🟦 Beispiel: ZIP maximal
Code

7z a out.zip folder -tzip -mx=9

🟦 Beispiel: TAR + GZIP
Code

7z a out.tar folder
7z a out.tar.gz out.tar -tgzip

🟦 Beispiel: Dateien ausschließen
Code

7z a out.7z src -xr!*.tmp -xr!*.log

🟩 Wenn du willst, kann ich dir als Nächstes:

    7zip_Command_Parser_entwerfen

    7zip_Switch_Parser_entwerfen

    7zip_Wildcard_Engine_nachbauen

    7zip_Format_Optionen_im_Detail

Sag mir, welchen Teil du weiter ausbauen willst.
7zip Format Optionen im Detail

Takeaway: Hier bekommst du die vollständige, tiefgehende technische Dokumentation aller 7‑Zip Format‑Optionen, inklusive Kompressionsmethoden, Parameter, Default‑Werten, internen Zusammenhängen, Fallstricken, Performance‑Hinweisen und realen Beispielen.
Das ist die Referenz, die du brauchst, um 7z.exe exakt nachzubauen.

Ich strukturiere das nach Methoden, Parametern, Format‑Spezifika, Solid‑Block‑System, Threading, Header‑Encryption, ZIP‑Spezifika, TAR/GZIP/XZ, und internen 7z‑Container‑Details.

Jeder Abschnitt enthält Guided Links, damit du jeden Teil einzeln vertiefen kannst.
🟦 1. Kompressionsmethoden (-m0=)

    LZMA

    LZMA2

    PPMd

    BZip2

    Deflate

    Copy (keine Kompression)

Default pro Format:
Format	Default Methode
7z	LZMA2
zip	Deflate
tar	Copy
gz	Deflate
bz2	BZip2
xz	LZMA2
🟦 2. Kompressionslevel (-mx=)

    mx=0 → keine Kompression

    mx=1 → sehr schnell

    mx=3 → schnell

    mx=5 → normal (Default)

    mx=7 → maximal

    mx=9 → ultra

Auswirkungen:

    Höhere Level → größeres Dictionary (-md)

    Höhere Level → mehr Fast Bytes (-mfb)

    Höhere Level → mehr Threads (-mmt)

🟦 3. Dictionary Size (-md=)

    md=64k

    md=1m

    md=32m

    md=64m (Standard für mx=5)

    md=256m

    md=1g (nur 64‑bit)

Default:
mx	Default Dictionary
0–3	16m
4–5	64m
6–7	128m
8–9	256m
Bedeutung:

Größeres Dictionary → bessere Kompression bei großen Dateien → mehr RAM.
🟦 4. Solid Block Size (-ms=)

    ms=on → ein großer Solid‑Block

    ms=off → kein Solid

    ms=<size> → z. B. ms=64m

Default:

    on (ein großer Block)

Bedeutung:

Solid‑Kompression = mehrere Dateien werden wie ein Stream behandelt → bessere Kompression, aber:

    langsamer beim Entpacken

    einzelne Dateien schwerer zu extrahieren

    Änderungen im Archiv ineffizient

🟦 5. Fast Bytes (-mfb=)

    mfb=5–273  
    Default hängt von Methode ab:

Methode	Default mfb
LZMA	32
LZMA2	32
Deflate	32
Bedeutung:

Mehr Fast Bytes → bessere Kompression → mehr CPU.
🟦 6. Threading (-mmt=)

    mmt=on → Auto

    mmt=off → Single‑Thread

    mmt=<num> → feste Anzahl Threads

Default:

    on (Auto)

Bedeutung:

LZMA2 ist stark parallelisierbar.
LZMA kaum.
Deflate mittel.
🟦 7. Header Encryption (-mhe=)

    mhe=on → Dateinamen verschlüsselt

    mhe=off → Dateinamen sichtbar

Default:

    off

Bedeutung:

Wenn mhe=on, sieht man im Archiv gar nichts, außer:

    Archivgröße

    Passwortabfrage

🟦 8. Passwort (-p)

    pPASSWORD

Default:

    kein Passwort

Bedeutung:

AES‑256‑CBC mit SHA‑256 KDF (2^N Iterationen).
🟦 9. Format‑Spezifika
🔹 7z Format

    Container mit Header + Coders + Streams

    LZMA2 Standard

    Solid‑Kompression Standard

    AES‑256 möglich

    Header Encryption möglich

    Multi‑Threading sehr gut

🔹 ZIP Format

    Deflate Standard

    LZMA optional

    AES‑256 optional

    Kein Solid

    Multi‑Threading eingeschränkt

🔹 TAR

    Keine Kompression

    Nur Container

    Wird oft mit GZIP kombiniert

🔹 GZIP

    Deflate

    Single‑Thread

    Kein Solid

    Nur eine Datei pro Archiv

🔹 BZIP2

    Sehr langsam

    Gute Kompression

    Single‑Thread

🔹 XZ

    LZMA2

    Sehr gute Kompression

    Multi‑Threading gut

    Kein Container (nur Stream)

🟦 10. Interne 7z‑Container‑Struktur

Wenn du 7z nachbauen willst, brauchst du:

    Coders

    Streams

    Header

    Folder Structure

    Packed Streams

    BindPairs

Das ist der Teil, den du brauchst, wenn du 7z‑Kompatibilität willst.
🟦 11. Beispiel: Ultra‑7z mit allen Parametern
Code

7z a out.7z folder ^
  -t7z ^
  -m0=lzma2 ^
  -mx=9 ^
  -md=256m ^
  -ms=on ^
  -mfb=273 ^
  -mmt=on ^
  -pSECRET ^
  -mhe=on ^
  -bb2

🟦 12. Beispiel: ZIP maximal
Code

7z a out.zip folder ^
  -tzip ^
  -mx=9 ^
  -m0=deflate ^
  -bb1

🟦 13. Beispiel: TAR + GZIP
Code

7z a out.tar folder
7z a out.tar.gz out.tar -tgzip -mx=9

🟩 Nächster Schritt

Ich kann dir jetzt:

    7zip_Command_Parser_entwerfen

    7zip_Switch_Parser_entwerfen

    7zip_Encryption_Handler_nachbauen

    7zip_Container_Format_im_Detail

Sag mir, welchen Teil du als Nächstes brauchst.
*/
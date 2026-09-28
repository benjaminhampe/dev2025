#pragma once

/*
Hier ist ein präziser Pseudo-Algorithmus, der beschreibt, wie ein Packer entscheiden muss, ob ein Dateipfad in den klassischen UStar-Header, das UStar-Prefix-Feld oder in einen erweiterten PaxHeader geschrieben werden muss.Das Problem mit Pfadlängen in TARDas klassische UStar-Format bietet zwei Felder für den Pfad:name (100 Bytes)prefix (155 Bytes)Man darf den Pfad aber nicht einfach irgendwo abschneiden. Er darf nur an einem Slash (/) getrennt werden, sodass der prefix-Teil ein gültiger Verzeichnisbaum bleibt und der name-Teil den Rest (inklusive Dateiname) enthält. Zudem müssen beide Teile nullterminiert sein (bzw. exakt ins Feld passen).Sobald das nicht klappt oder der Pfad länger als 255 Zeichen ist, muss ein PaxHeader (typeflag='x') her.Pseudo-Algorithmus: Pfad-ZuweisungtextALGORITHMUS: BestimmePfadSpeicherung(voller_pfad)

    EINGABE: voller_pfad (String, UTF-8 codiert)
    AUSGABE: Speichermethode und Header-Zuweisungen

    Länge = LängeInBytes(voller_pfad)

    // FALL 1: Passt perfekt in das Standard-Namensfeld
    WENN Länge <= 100 DANN
        NutzeKlassischenUStarHeader()
        Setze ustar.name = voller_pfad
        Setze ustar.prefix = "" (leere Bytes)
        ENDE

    // FALL 2: Zu lang für 'name', prüfen ob UStar-Splitting möglich ist (Max 255 Zeichen)
    WENN Länge > 255 DANN
        // Sofortiger Abbruch: UStar kann maximal 155 + 1 + 100 = 256 Zeichen (mit Slash)
        GEHE ZU FALL 3 (PaxHeader)
    ENDE

    // Versuch, den Pfad regelkonform am Slash zu trennen
    Erfolg = FALSCH
    
    // Suche nach einem Trennpunkt (Slash) von hinten nach vorne,
    // sodass der hintere Teil in 'name' (100) und der vordere in 'prefix' (155) passt.
    FÜR i VON (Länge - 1) RUNTER ZU 0 SCHREITE:
        WENN voller_pfad[i] == '/' DANN
            vorderer_teil = Substring(voller_pfad, 0, i)
            hinterer_teil = Substring(voller_pfad, i + 1, Ende)

            WENN LängeInBytes(hinterer_teil) <= 100 UND LängeInBytes(vorderer_teil) <= 155 DANN
                Erfolg = WAHR
                BrecheSchleifeAb
            ENDE
        ENDE
    ENDE

    WENN Erfolg == WAHR DANN
        NutzeKlassischenUStarHeader()
        Setze ustar.name = hinterer_teil
        Setze ustar.prefix = vorderer_teil
        ENDE
    SONST
        // Pfad ist zwar <= 255 Zeichen, lässt sich aber nicht sauber trennen
        // (z.B. weil der Dateiname selbst 110 Zeichen lang ist)
        GEHE ZU FALL 3
    ENDE


    // FALL 3: PaxHeader erzwungen
    FALL 3 (PaxHeader):
        ErzeugePaxHeader()
        Setze pax_payload.fields["path"] = voller_pfad
        
        // Der darauffolgende UStar-Dummy-Header benötigt dennoch valide Einträge
        ErzeugeKlassischenUStarHeaderAlsNachfolger()
        
        // Kürzung für alten Entpacker (Best Effort):
        WENN Länge <= 100 DANN
            Setze ustar_nachfolger.name = voller_pfad
        SONST
            // Schneide hart bei 99 Zeichen ab und füge eine Tilde oder Dummy-Namen ein
            Setze ustar_nachfolger.name = Substring(voller_pfad, 0, 99)
        ENDE
        ENDE
Verwende Code mit Vorsicht.Wo schneidet man am besten ab? (Die Logik hinter Schleife 2)Wenn du den Pfad für UStar aufteilst, suchst du den am weitesten rechts liegenden Slash, der die Bedingungen erfüllt.Ein Beispiel:Pfad: ordner_A/ein_sehr_langer_unterordner_name_der_daten_enthaelt/meine_datei.txtGesamtlänge: 76 Zeichen. (Würde in Fall 1 rutschen, aber nehmen wir an, die Limits wären kleiner).Wenn meine_datei.txt (15 Zeichen) in name wandert, muss der Rest (ordner_A/ein_sehr_langer_unterordner_name_der_daten_enthaelt) in prefix. Das ist die sauberste Trennung, weil der Name der Datei intakt bleibt.Das Problem, warum Fall 2 fehlschlagen kann:Pfad: kurzer_ordner/ein_riesiger_dateiname_der_einfach_nicht_aufhoeren_will_zu_existieren_und_alles_kaputt_macht.binDer Verzeichnisname ist kurz (kurzer_ordner = 13 Bytes).Der Dateiname ist gigantisch (96 Bytes).Gesamtlänge = 110 Bytes. Er passt nicht in name (max 100).Wenn wir am Slash trennen, ist der hintere Teil 96 Bytes lang (passt in name). Der vordere Teil ist 13 Bytes lang (passt in prefix). Erfolg!Gegenbeispiel für das Fehlschlagen von Fall 2:Pfad: ausgabe/ein_unfassbar_langer_dateiname_der_die_hundert_zeichen_grenze_schon_ganz_alleine_fuer_sich_bricht_ohne_verzeichnis.binDer Dateiname ist 115 Bytes lang.Da es keinen Slash innerhalb der letzten 100 Zeichen gibt (der Pfad hat nur den einen ganz vorne), wird die Schleife keinen gültigen Trennpunkt finden. Der Dateiname kann nicht in das 100-Byte-Feld gequetscht werden. Hier greift sofort Fall 3 (Pax).Möchtest du, dass wir diesen Pseudo-Algorithmus in eine echte C++ Funktion umwandeln, die einen std::string entgegennimmt und eine Struktur mit den fertig aufgeteilten Feldern zurückgibt?

*/
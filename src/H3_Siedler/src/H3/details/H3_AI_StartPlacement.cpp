#include "H3_AI_StartPlacement.h"
#include <H3/H3_Game.h>
#include <H3/details/H3_Topology.h>
#include <vector>
#include <algorithm>

/*
KI-Strategie	Beschreibung                            Mathematische Anpassung

Expansionist    Setzt auf starke Standard-Ressourcen    Nutzt die Basis-Gewichte
(Standard)      für schnellen Straßenbau.               (A und B leicht erhöht).

Port-Händler    Sucht gezielt nach Feldern, die nah an  Gibt Kreuzungspunkten mit
(3:1)           einem allgemeinen 3:1 Hafen liegen.     Port3v1 einen Bonus von z.B. +3 Punkten.

Monopolist      Sucht ein extrem starkes Feld von       Erhöht das Gewicht einer einzelnen Ressource massiv und sucht den passenden Hafen.
(2:1)           einer Ressource und versucht den
                passenden Port2v1 zu besetzen.


3. Schritt: Bewertung der Farm-Bauplätze (Kreuzungen)

Die KI läuft nun in einer Schleife über alle freien Kreuzungspunkte der Karte, die die Abstandsregel (mindestens 2 Kanten Abstand zu anderen Farmen) erfüllen, und berechnet einen Score:
\(\text{Score}=\sum (\text{Wahrscheinlichkeit\ des\ Feldes}\times \text{Ressourcen-Gewicht})+\text{Hafen-Bonus}\)
Beispiel für die Berechnung eines Knotens:
Ein Knoten grenzt an:
1. Feld mit Ressource A (Zahl 8 = 5 Punkte Wahrscheinlichkeit)
2. Feld mit Ressource C (Zahl 3 = 2 Punkte Wahrscheinlichkeit)
3. Ein Hafen-Feld (3:1) – falls die KI die Strategie "Port-Händler" hat.
\(\text{Score}=(5\times 1.2)+(2\times 1.0)+3.0\text{\ (Hafen-Bonus)}=6+2+3=11.0\)
Die KI sortiert alle Kreuzungen nach diesem Score und wählt für die erste Farm den höchsten Wert.

4. Schritt: Platzierung der Straßen

Sobald die Farm platziert ist, muss die KI sofort die angeschlossene Straße setzen. Dafür bewertet sie die 3 von der Farm ausgehenden Kanten:
1. Pfad zu weiteren Ressourcen: Welcher angrenzende unbesetzte Knoten hat den höchsten Ressourcen-Score? (Wichtig für die Expansion).
2. Pfad zum Hafen: Wenn die KI eine Hafen-Strategie verfolgt und die Farm nicht direkt auf dem Hafen steht, priorisiert sie die Kante, die näher zum Hafen-Knoten führt.

5. Schritt: Die zweite Farm (Synergie-Prüfung)

Für die zweite Farm läuft dasselbe System, allerdings mit einer Synergie-Bedingung, um Vielfalt in den Erträgen zu garantieren:
• Die KI sollte bevorzugt Ressourcen wählen, die sie durch die erste Farm noch nicht abgedeckt hat.
• Hafen-Abgleich: Wenn die erste Farm auf einem Port2v1 für Ressource A steht, muss die zweite Farm zwingend an ein Feld mit Ressource A und hoher Wahrscheinlichkeit gebaut werden, um den Hafen effektiv zu nutzen.

Pseudocode-Struktur für deine Implementierung

text
Funktion BerechneBestePosition(kiStrategie):
    BesterKnoten = null
    BesterScore = -Unendlich

    Für jeden Knoten auf der Karte:
        Wenn ValidStartposition(Knoten) == falsch: Überspringe

        AktuellerScore = 0
        Für jedes angrenzendeHex von Knoten:
            Ressource = angrenzendeHex.Typ
            Wahrscheinlichkeit = angrenzendeHex.WürfelChancen

            AktuellerScore += Wahrscheinlichkeit * kiStrategie.Gewicht(Ressource)

        Wenn Knoten.HatHafen() und kiStrategie.MagHäfen:
            AktuellerScore += kiStrategie.HafenBonus(Knoten.HafenTyp)

        Wenn AktuellerScore > BesterScore:
            BesterScore = AktuellerScore
            BesterKnoten = Knoten

    Rückgabe BesterKnoten
Verwende Code mit Vorsicht.
Mit diesem System verhalten sich die Gegner sofort organisch: Einige drängen auf Häfen, andere auf pure Ressourcen-Masse für A und B, wodurch sich die KI-Spieler auf dem Brett nicht gegenseitig blockieren, sondern unterschiedliche Taktiken wählen.
Soll die KI bei der Platzierung der zweiten Farm auch schon die Erreichbarkeit der ersten Farm mit einberechnen (um Straßen miteinander zu verbinden), oder soll sie die zweite Farm komplett unabhängig an der besten verbleibenden Stelle der Karte platzieren?


Die universelle Formel

text
Score = (Punkte_Feld1 * Gewicht_Feld1)
      + (Punkte_Feld2 * Gewicht_Feld2)
      + (Punkte_Feld3 * Gewicht_Feld3)
      + HafenBonus
Verwende Code mit Vorsicht.

================================================================================
C++ KI-STRATEGIEN: GEWICHTE UND BERECHNUNGSBEISPIELE FÜR DIE STARTPHASE

WÜRFELWAHRSCHEINLICHKEITEN (PUNKTE-SYSTEM):
• Zahlen 2 & 12 = 1 Punkt
• Zahlen 3 & 11 = 2 Punkte
• Zahlen 4 & 10 = 3 Punkte
• Zahlen 5 & 9  = 4 Punkte
• Zahlen 6 & 8  = 5 Punkte

1. KOEFFIZIENTEN-TABELLE (TRANSPONIERT)

Parameter / Faktor              1.          2. Händler  3.(Typ AB)  3.(Typ CD)  3.(Typ E)
                                Expansion   Port-3v1	Monopolist  Monopolist  Monopolist
Gewicht Ressource A (Straße+)	1.6         1.2         2.1         0.8         0.8
Gewicht Ressource B (Straße+)	1.6         1.2         0.8         0.8         0.8
Gewicht Ressource C (Farm)      0.8         1.2         0.8         2.1         0.5
Gewicht Ressource D (Farm)      0.5         0.9         0.8         0.8         0.4
Gewicht Ressource E (Ignor.)	0.0         0.0         0.0         0.0         2.0
Sum                             4.5         4.5         4.5         4.5         4.5
Bonus: Allgemeiner Port3v1      +20% für alle Ressourcen (überkommt Nachteile von weniger Landtiles)
Bonus: Passender Port2v1        +50% für spezielle Ressource (überkommt Nachteile von weniger Landtiles)

1. DIE FORMEL

Score = BasisRessourcenScore + HafenBonus;

Wobei sich der "BasisRessourcenScore" aus den 3 angrenzenden Feldern berechnet:

BasisRessourcenScore = (Punkte_Feld1 * Gewicht_Feld1)
                     + (Punkte_Feld2 * Gewicht_Feld2)
                     + (Punkte_Feld3 * Gewicht_Feld3);

Der BasisRessourcenScore:
    • "Punkte_Feld": Das sind die Würfel-Wahrscheinlichkeiten (von 1 bis 5 Punkten)
        des jeweiligen Hex-Feldes. Wasser oder Wüste liefern hier automatisch 0 Punkte.
    • "Gewicht_Feld": Das ist der Multiplikator aus deiner Strategie-Tabelle
        (z.B. 2.1 für die Monopol-Ressource oder 1.6 für den Expansionisten).
    • Zweck: Dieser Teil misst rein die rohe Rohstoff-Produktion des Standorts,
        angepasst an das, was die jeweilige Strategie gerade sucht.

Der HafenBonus (Prozentual gekoppelt!):
    Damit der Hafenbonus die Monopolisten im Innenland nicht unfair bevorzugt,
    wird er nicht stumpf als feste Zahl addiert, sondern berechnet sich dynamisch
    aus der tatsächlichen Stärke der Felder vor Ort:
    • Fall 1: Der Knoten hat KEINEN Hafen (Innenland)
        HafenBonus = 0.0;
        (Kein Push für niemanden, das Innenland bleibt fair)
    • Fall 2: Allgemeiner Hafen (Port 3v1)
        Der Bonus beträgt 20% des gesamten Rohstoff-Ertrags dieses Knotens.
        HafenBonus = BasisRessourcenScore * 0.20;
        (Gleicht das fehlende dritte Feld an der Küste für flexible Händler aus)
    • Fall 3: Passender Spezialhafen (Port 2v1, z.B. für Monopol-Ressource A)
        Der Bonus beträgt massive 50%, aber NUR auf den Ertrag des Monopol-Feldes!
        HafenBonus = (Punkte_MonopolFeld * Gewicht_Monopol) * 0.50;
        (Macht die Küste für den Monopolisten extrem attraktiv, bringt ihm im
        Innenland ohne das passende Feld aber absolut gar nichts)
    • Fall 4: Falscher Spezialhafen (z.B. ein B-Hafen an einem C-Monopol-Knoten)
        HafenBonus = 0.0;

WARUM DAS DIE STRATEGIE-DIVERSITÄT SICHERT:
    • Im Innenland (Wasser = 0, HafenBonus = 0) gewinnt der Expansionist, weil seine
        Gewichte (1.6 für A und B) bei guten Zahlen perfekt ineinandergreifen.
    • An der Küste (ein Feld ist Wasser = 0) bricht der Score des Expansionisten ein.
        Hier schlägt die Stunde des Monopolisten: Sein HafenBonus (50% Extra auf sein
        starkes Hauptfeld) gleicht den Verlust des Wasserfeldes aus und katapultiert
        ihn an diesem spezifischen Küstenknoten an die Spitze.
================================================================================

*/

#if 0

// Definiert die Ressourcen-Koeffizienten für die KI-Berechnung
struct StrategieGewichte
{
    double gewichtA = 1.0;
    double gewichtB = 1.0;
    double gewichtC = 1.0;
    double gewichtD = 1.0;
    double gewichtE = 0.0;
};

// Repräsentiert ein angrenzendes Hex-Feld
struct HexFeld
{
    int ressourcenTyp; // 0=A, 1=B, 2=C, 3=D, 4=E, 5=Wasser/Wüste
    int wuerfelPunkte; // Kombinationspunkte (1 bis 5) basierend auf der Zahl
};

// Repräsentiert eine Kreuzung (Corner) auf deiner 3D-Karte
struct Knoten
{
    int id;
    bool hatHafen;
    int hafenTyp;                           // 0=Port3v1, 1=Port2v1_A, 2=Port2v1_B, etc.
    std::vector<HexFeld> angrenzendeFelder;
    std::vector<int> nachbarKnotenIDs;      // Direkt über 1 Kante verbundene Knoten-IDs
};

// Repräsentiert die Auswahl einer Straßen-Kante
struct Kante
{
    int vonKnotenID;
    int zuKnotenID;
};

// Speichert das beste gefundene Knoten-Paar für die zwei Start-Farmen
struct BestesStartPaar
{
    int knotenID_X = -1;
    int knotenID_Y = -1;
    double besterScore = -1.0;
};

// Hilfsfunktion: Berechnet den Ressourcen-Basiswert eines einzelnen Knotens
double
BerechneEinzelKnotenBasisScore( const Knoten& k,
                                const StrategieGewichte& g)
{
    double score = 0.0;

    for (const auto& feld : k.angrenzendeFelder) {
        if (feld.ressourcenTyp == 0) {
            score += feld.wuerfelPunkte * g.gewichtA;
        } else if (feld.ressourcenTyp == 1) {
            score += feld.wuerfelPunkte * g.gewichtB;
        } else if (feld.ressourcenTyp == 2) {
            score += feld.wuerfelPunkte * g.gewichtC;
        } else if (feld.ressourcenTyp == 3) {
            score += feld.wuerfelPunkte * g.gewichtD;
        } else if (feld.ressourcenTyp == 4) {
            score += feld.wuerfelPunkte * g.gewichtE;
        }
    }

    return score;
}

// Hauptfunktion zur Ermittlung der besten Kante für die Start-Straße
Kante FindeBesteStartStrasse(const Knoten& startFarm,
                             const std::vector<Knoten>& weltKnoten,
                             const StrategieGewichte& kiGewichte,
                             bool suchtHafen,
                             int zielHafenTyp)
{
    Kante besteKante;
    besteKante.vonKnotenID = startFarm.id;
    besteKante.zuKnotenID = -1;
    double besterKantenScore = -1.0;

    // Die KI prüft alle direkt angrenzenden Wege (Nachbarknoten)
    for (int nachbarID : startFarm.nachbarKnotenIDs)
    {
        const Knoten& zielKnoten = weltKnoten[nachbarID];

        // 1. Basis-Ressourcenwert des Zielknotens berechnen
        double aktuellerScore = BerechneEinzelKnotenBasisScore(zielKnoten, kiGewichte);

        // 2. Strategischen Richtungs-Bonus addieren
        double richtungsBonus = 0.0;

        if (zielKnoten.hatHafen)
        {
            if (suchtHafen && zielKnoten.hafenTyp == zielHafenTyp)
            {
                // Passender Spezialhafen (z.B. Port2v1 für das Monopol)
                richtungsBonus = 10.0;
            }
            else if (zielKnoten.hafenTyp == 0)
            {
                // Allgemeiner 3:1 Hafen
                richtungsBonus = 6.0;
            }
        }

        double gesamtKantenScore = aktuellerScore + richtungsBonus;

        // Besten Weg loggen
        if (gesamtKantenScore > besterKantenScore)
        {
            besterKantenScore = gesamtKantenScore;
            besteKante.zuKnotenID = nachbarID;
        }
    }

    return besteKante;
}

// Prüft, ob zwei Knoten die Catan-Abstandsregel zueinander einhalten
bool IstGueltigesPaar(const Knoten& x, const Knoten& y)
{
    // 1. Die Farmen dürfen nicht auf demselben Knoten stehen
    if (x.id == y.id)
    {
        return false;
    }

    // 2. Abstandsregel: x darf kein direkter Nachbar von y sein (1 Kante Abstand ist verboten)
    for (int nachbarID : x.nachbarKnotenIDs)
    {
        if (nachbarID == y.id)
        {
            return false;
        }
    }

    return true;
}

// Hilfsfunktion: Berechnet den reinen Ressourcen-Basiswert eines einzelnen Knotens
double
BerechneEinzelKnotenBasis(const Knoten& k, const StrategieGewichte& g)
{
    double score = 0.0;
    for (const auto& feld : k.angrenzendeFelder)
    {
        if (feld.ressourcenTyp == 0)      score += feld.wuerfelPunkte * g.gewichtA;
        else if (feld.ressourcenTyp == 1) score += feld.wuerfelPunkte * g.gewichtB;
        else if (feld.ressourcenTyp == 2) score += feld.wuerfelPunkte * g.gewichtC;
        else if (feld.ressourcenTyp == 3) score += feld.wuerfelPunkte * g.gewichtD;
        else if (feld.ressourcenTyp == 4) score += feld.wuerfelPunkte * g.gewichtE;
    }
    return score;
}

// Hilfsfunktion: Prüft, welche Ressourcen-Typen an den Knoten überhaupt existieren
bool HatPaarRessource(const Knoten& x, const Knoten& y, int gesuchterTyp) {
    for (const auto& f : x.angrenzendeFelder) {
        if (f.ressourcenTyp == gesuchterTyp && f.wuerfelPunkte > 0) return true;
    }
    for (const auto& f : y.angrenzendeFelder) {
        if (f.ressourcenTyp == gesuchterTyp && f.wuerfelPunkte > 0) return true;
    }
    return false;
}

// Hilfsfunktion: Berechnet die kombinierten Synergien zweier Knoten
double BerechnePaarScore(const Knoten& x, const Knoten& y, const StrategieGewichte& g, bool istMonopolist, int monopolTyp) {
    // 1. Basis-Rohstoffertrag addieren
    double basisX = BerechneEinzelKnotenBasis(x, g);
    double basisY = BerechneEinzelKnotenBasis(y, g);
    double gesamtScore = basisX + basisY;

    // 2. Globaler Hafenbonus (Synergie-Prüfung)
    double hafenBonus = 0.0;

    // Prüfen auf passenden 2:1 Spezialhafen (z.B. Monopolist hat Typ A und einer der Knoten ist der A-Hafen)
    if (istMonopolist) {
        int passenderHafenTyp = monopolTyp + 1; // Wenn Typ=0 (A), ist HafenTyp=1 (Port2v1_A)
        bool einKnotenHatHafen = (x.hatHafen && x.hafenTyp == passenderHafenTyp) || (y.hatHafen && y.hafenTyp == passenderHafenTyp);

        if (einKnotenHatHafen && HatPaarRessource(x, y, monopolTyp)) {
            // 50% Bonus auf die gesamte Produktion des Monopol-Materials dieses Paares
            double monopolPunkte = 0.0;
            for (const auto& f : x.angrenzendeFelder) if (f.ressourcenTyp == monopolTyp) monopolPunkte += f.wuerfelPunkte;
            for (const auto& f : y.angrenzendeFelder) if (f.ressourcenTyp == monopolTyp) monopolPunkte += f.wuerfelPunkte;

            double monopolGewicht = (monopolTyp == 0) ? g.gewichtA : ((monopolTyp == 1) ? g.gewichtB : ((monopolTyp == 2) ? g.gewichtC : g.gewichtD));
            hafenBonus = (monopolPunkte * monopolGewicht) * 0.50;
        }
    }
    // Prüfen auf allgemeinen 3:1 Hafen (Port-Händler Taktik)
    else {
        bool einKnotenHatAllgemeinHafen = (x.hatHafen && x.hafenTyp == 0) || (y.hatHafen && y.hafenTyp == 0);
        if (einKnotenHatAllgemeinHafen) {
            // 20% Bonus auf den gesamten Rohstoff-Ertrag des Paares
            hafenBonus = gesamtScore * 0.20;
        }
    }

    gesamtScore += hafenBonus;

    // 3. Diversitäts-Bonus (Belohnung für unterschiedliche Ressourcen)
    int unterschiedlicheSorten = 0;
    for (int typ = 0; typ <= 3; ++typ) { // Typ 0 bis 3 entspricht Ressourcen A, B, C, D
        if (HatPaarRessource(x, y, typ)) {
            unterschiedlicheSorten++;
        }
    }
    gesamtScore += (unterschiedlicheSorten * 2.0);

    return gesamtScore;
}

// Hauptfunktion: Gibt die IDs der zwei besten Knoten für die ersten 2 Farmen zurück
BestesStartPaar
FindeBesteStartPlaetze(const std::vector<Knoten>& alleKnoten, const StrategieGewichte& g, bool istMonopolist, int monopolTyp)
{
    BestesStartPaar ergebnis;
    int n = alleKnoten.size();

    for (int i = 0; i < n; ++i) {
        const Knoten& knotenX = alleKnoten[i];

        for (int j = i + 1; j < n; ++j) {
            const Knoten& knotenY = alleKnoten[j];

            // Catan-Abstandsregel prüfen
            if (!IstGueltigesPaar(knotenX, knotenY)) {
                continue;
            }

            // Berechne den kombinierten Wert für dieses Farm-Paar
            double aktuellerScore = BerechnePaarScore(knotenX, knotenY, g, istMonopolist, monopolTyp);

            if (aktuellerScore > ergebnis.besterScore) {
                ergebnis.besterScore = aktuellerScore;
                ergebnis.knotenID_X = knotenX.id;
                ergebnis.knotenID_Y = knotenY.id;
            }
        }
    }

    return ergebnis;
}

#endif



/*
1. KOEFFIZIENTEN-TABELLE (TRANSPONIERT)

Parameter / Faktor              1.          2. Händler  3.(Typ AB)  3.(Typ CD)  3.(Typ E)
                                Expansion   Port-3v1	Monopolist  Monopolist  Monopolist
Gewicht Ressource A (Straße+)	1.6         1.2         2.1         0.8         0.8
Gewicht Ressource B (Straße+)	1.6         1.2         0.8         0.8         0.8
Gewicht Ressource C (Farm)      0.8         1.2         0.8         2.1         0.5
Gewicht Ressource D (Farm)      0.5         0.9         0.8         0.8         0.4
Gewicht Ressource E (Ignor.)	0.0         0.0         0.0         0.0         2.0
Sum                             4.5         4.5         4.5         4.5         4.5
Bonus: Allgemeiner Port3v1      +20% für alle Ressourcen (überkommt Nachteile von weniger Landtiles)
Bonus: Passender Port2v1        +50% für spezielle Ressource (überkommt Nachteile von weniger Landtiles)
*/

// Definiert die Ressourcen-Koeffizienten für die KI-Berechnung
struct StrategieGewichte
{
    typedef double T;
    T A, B, C, D, E;

    StrategieGewichte(T a, T b, T c, T d, T e = 0.0)
        : A(a), B(b), C(c), D(d), E(e)
    {}

    static StrategieGewichte expansionist()
    {
        return StrategieGewichte(1.6,1.6,0.8,0.5);
    }
    static StrategieGewichte trader3v1()
    {
        return StrategieGewichte(1.2,1.2,1.2,0.9);
    }
    static StrategieGewichte monopolist2v1(H3_TileType tt)
    {
        switch (tt)
        {
        case H3_TileType::TileA: return StrategieGewichte(2.1,0.8,0.8,0.8);
        case H3_TileType::TileB: return StrategieGewichte(0.8,2.1,0.8,0.8);
        case H3_TileType::TileC: return StrategieGewichte(0.8,0.8,2.1,0.8);
        case H3_TileType::TileD: return StrategieGewichte(0.8,0.8,0.8,2.1);
        case H3_TileType::TileE: return StrategieGewichte(0.8,0.8,0.5,0.4,2.0);
        default: return StrategieGewichte(0,0,0,0,0);
        }
    }
};

/*
// Repräsentiert ein angrenzendes Hex-Feld
struct HexFeld
{
    int ressourcenTyp; // 0=A, 1=B, 2=C, 3=D, 4=E, 5=Wasser/Wüste
    int wuerfelPunkte; // Kombinationspunkte (1 bis 5) basierend auf der Zahl
};

// Repräsentiert eine Kreuzung (Corner) auf deiner 3D-Karte
struct Knoten
{
    int id;
    bool hatHafen;
    int hafenTyp;                           // 0=Port3v1, 1=Port2v1_A, 2=Port2v1_B, etc.
    std::vector<HexFeld> angrenzendeFelder;
    std::vector<int> nachbarKnotenIDs;      // Direkt über 1 Kante verbundene Knoten-IDs
};

// Repräsentiert die Auswahl einer Straßen-Kante
struct Kante
{
    int vonKnotenID;
    int zuKnotenID;
};
*/

// Hilfsfunktion: Berechnet den Ressourcen-Basiswert eines einzelnen Knotens
double BerechneEinzelKnotenBasisScore(
    const H3_Game& game,
    const H3_Corner& corner,
    const StrategieGewichte& gewichte)
{
    double score = 0.0;

    for (u32 tileId : corner.tiles)
    {
        const H3_Tile& tile = H3_getTile(game, __func__, tileId);
        const int diceChance = tile.getDiceChance();
        switch (tile.tt)
        {
        case H3_TileType::TileA: score += gewichte.A * diceChance; break;
        case H3_TileType::TileB: score += gewichte.B * diceChance; break;
        case H3_TileType::TileC: score += gewichte.C * diceChance; break;
        case H3_TileType::TileD: score += gewichte.D * diceChance; break;
        case H3_TileType::TileE: score += gewichte.E * diceChance; break;
        }
    }

    return score;
}

// Prüft, ob zwei Knoten die Catan-Abstandsregel zueinander einhalten
bool
IstGueltigesPaar(
    const H3_Game& game,
    const H3_Corner& x,
    const H3_Corner& y)
{
    // 1. Die Farmen dürfen nicht auf demselben Knoten stehen
    if (x.id == y.id)
    {
        return false;
    }

    if (game.m_cfg.buildingSpacing == 0)
    {
        return true;
    }

    // 2. Abstandsregel: x darf kein direkter Nachbar von y sein (1 Kante Abstand ist verboten)
    for (int nachbarID : x.next)
    {
        if (nachbarID == y.id)
        {
            return false;
        }
    }
    return true;
}

// Hilfsfunktion: Berechnet den reinen Ressourcen-Basiswert eines einzelnen Knotens
double
BerechneEinzelKnotenBasis(
    const H3_Game& game,
    const H3_Corner& corner,
    const StrategieGewichte& g)
{
    double score = 0.0;
    for (u32 tileId : corner.tiles)
    {
        const H3_Tile& tile = H3_getTile(game,__func__,tileId);
        int diceChance = tile.getDiceChance();
        switch (tile.tt)
        {
        case H3_TileType::TileA: score += g.A * diceChance; break;
        case H3_TileType::TileB: score += g.B * diceChance; break;
        case H3_TileType::TileC: score += g.C * diceChance; break;
        case H3_TileType::TileD: score += g.D * diceChance; break;
        case H3_TileType::TileE: score += g.E * diceChance; break;
        default: break;
        }
    }
    return score;
}

// Hilfsfunktion: Prüft, welche Ressourcen-Typen an den Knoten überhaupt existieren
bool
HatPaarRessource(
    const H3_Game& game,
    const H3_Corner& x,
    const H3_Corner& y,
    const H3_TileType& gesuchterTyp)
{
    for (u32 tileId : x.tiles)
    {
        const H3_Tile& tile = H3_getTile(game, __func__, tileId);
        if (tile.tt == gesuchterTyp && tile.chipValue > 0) return true;
    }
    for (u32 tileId : y.tiles)
    {
        const H3_Tile& tile = H3_getTile(game, __func__, tileId);
        if (tile.tt == gesuchterTyp && tile.chipValue > 0) return true;
    }
    return false;
}

// Hilfsfunktion: Berechnet die kombinierten Synergien zweier Knoten
double
BerechnePaarScore(
    const H3_Game& game,
    const H3_Corner& x,
    const H3_Corner& y,
    const StrategieGewichte& g,
    bool istMonopolist,
    const H3_TileType& monopolTyp)
{
    // 1. Basis-Rohstoffertrag addieren
    double basisX = BerechneEinzelKnotenBasis(game, x, g);
    double basisY = BerechneEinzelKnotenBasis(game, y, g);
    double gesamtScore = basisX + basisY;

    // 2. Globaler Hafenbonus (Synergie-Prüfung)
    double hafenBonus = 0.0;

    // Prüfen auf passenden 2:1 Spezialhafen (z.B. Monopolist hat Typ A und einer der Knoten ist der A-Hafen)
    if (istMonopolist)
    {
        const H3_Bank x_hasPort2v1 = H3_numPort2v1AtCorner(game,x.id);
        const H3_Bank y_hasPort2v1 = H3_numPort2v1AtCorner(game,y.id);

        bool hatSpezialHafen =
            (x_hasPort2v1.A && monopolTyp == H3_TileType::TileA) ||
            (x_hasPort2v1.B && monopolTyp == H3_TileType::TileB) ||
            (x_hasPort2v1.C && monopolTyp == H3_TileType::TileC) ||
            (x_hasPort2v1.D && monopolTyp == H3_TileType::TileD) ||
            (x_hasPort2v1.E && monopolTyp == H3_TileType::TileE) ||

            (y_hasPort2v1.A && monopolTyp == H3_TileType::TileA) ||
            (y_hasPort2v1.B && monopolTyp == H3_TileType::TileB) ||
            (y_hasPort2v1.C && monopolTyp == H3_TileType::TileC) ||
            (y_hasPort2v1.D && monopolTyp == H3_TileType::TileD) ||
            (y_hasPort2v1.E && monopolTyp == H3_TileType::TileE);

        if (hatSpezialHafen && HatPaarRessource(game, x, y, monopolTyp))
        {
            // 50% Bonus auf die gesamte Produktion des Monopol-Materials dieses Paares
            double monopolPunkte = 0.0;
            for (u32 tileId : x.tiles)
            {
                const auto& tile = H3_getTile(game,__func__,tileId);
                if (tile.tt == monopolTyp)
                {
                    monopolPunkte += tile.getDiceChance();
                }
            }
            for (u32 tileId : y.tiles)
            {
                const auto& tile = H3_getTile(game,__func__,tileId);
                if (tile.tt == monopolTyp)
                {
                    monopolPunkte += tile.getDiceChance();
                }
            }

            double monopolGewicht = 0.0;
            switch (monopolTyp)
            {
            case H3_TileType::TileA: monopolGewicht = g.A; break;
            case H3_TileType::TileB: monopolGewicht = g.B; break;
            case H3_TileType::TileC: monopolGewicht = g.C; break;
            case H3_TileType::TileD: monopolGewicht = g.D; break;
            case H3_TileType::TileE: monopolGewicht = g.E; break;
            default: break;
            }
            hafenBonus = (monopolPunkte * monopolGewicht) * 0.50;
        }
    }
    // Prüfen auf allgemeinen 3:1 Hafen (Port-Händler Taktik)
    else
    {
        const bool x_hasPort3v1 = H3_numPort3v1AtCorner(game,x.id);
        const bool y_hasPort3v1 = H3_numPort3v1AtCorner(game,y.id);
        if (x_hasPort3v1 || y_hasPort3v1)
        {
            // 20% Bonus auf den gesamten Rohstoff-Ertrag des Paares
            hafenBonus = gesamtScore * 0.20;
        }
    }

    gesamtScore += hafenBonus;

    // 3. Diversitäts-Bonus (Belohnung für unterschiedliche Ressourcen)
    int unterschiedlicheSorten = 0;
    for (auto typ : { H3_TileType::TileA, H3_TileType::TileB,
                      H3_TileType::TileC, H3_TileType::TileD,
                      H3_TileType::TileE })
    {
        if (HatPaarRessource(game, x, y, typ))
        {
            unterschiedlicheSorten++;
        }
    }
    gesamtScore += (2.0 * unterschiedlicheSorten);

    return gesamtScore;
}

// Hauptfunktion: Gibt die IDs der zwei besten Knoten für die ersten 2 Farmen zurück
BestesStartPaar
FindeBesteStartPlaetze(
    const H3_Game& game,
    const std::vector<u32>& alleKnoten,
    const StrategieGewichte& g,
    bool istMonopolist,
    int monopolTyp)
{
    BestesStartPaar ergebnis;

    for (u32 cornerIdX : alleKnoten)
    {
        const H3_Corner& x = H3_getCorner(game,__func__,cornerIdX);

        for (u32 cornerIdY : alleKnoten)
        {
            const H3_Corner& y = H3_getCorner(game,__func__,cornerIdY);

            // Catan-Abstandsregel prüfen
            if (!IstGueltigesPaar(game, x, y))
            {
                continue;
            }

            // Berechne den kombinierten Wert für dieses Farm-Paar
            double score = BerechnePaarScore(game, x, y, g, istMonopolist, monopolTyp);
            if (score > ergebnis.score)
            {
                ergebnis.score = score;
                ergebnis.cornerX = x.id;
                ergebnis.cornerY = y.id;
            }
        }
    }

    return ergebnis;
}

// Hauptfunktion: Gibt die IDs der zwei besten Knoten für die ersten 2 Farmen zurück
std::vector<BestesStartPaar>
FindeBesteStartPlaetze(const H3_Game& game, const uint32_t playerId)
{
    std::vector<BestesStartPaar> bestePaare;

    std::vector<u32> cornerIds = H3_computeVisibleCorners(game, playerId);

    // 1.
    {
        BestesStartPaar paar = FindeBesteStartPlaetze(
            game, cornerIds, StrategieGewichte::expansionist(),
            false, H3_TileType::Desert);

        bestePaare.emplace_back( paar );
    }

    // 2.
    {
        BestesStartPaar paar = FindeBesteStartPlaetze(
            game, cornerIds, StrategieGewichte::trader3v1(),
            false, H3_TileType::Desert);

        bestePaare.emplace_back( paar );
    }

    // 3.
    for (auto tt : { H3_TileType::TileA, H3_TileType::TileB,
                     H3_TileType::TileC, H3_TileType::TileD,
                     H3_TileType::TileE })
    {
        BestesStartPaar paar = FindeBesteStartPlaetze(
            game, cornerIds, StrategieGewichte::monopolist2v1(tt),
            true, tt);

        bestePaare.emplace_back( paar );
    }

    std::sort( bestePaare.begin(), bestePaare.end(),
              [] ( BestesStartPaar const & a, BestesStartPaar const & b )
              {
                  return a.score > b.score;
              } );

    return bestePaare;
}

/*
// Hauptfunktion zur Ermittlung der besten Kante für die Start-Straße
Kante FindeBesteStartStrasse(const H3_Corner& startFarm,
                             const std::vector<Knoten>& weltKnoten,
                             const StrategieGewichte& kiGewichte,
                             bool suchtHafen,
                             int zielHafenTyp)
{
    Kante besteKante;
    besteKante.vonKnotenID = startFarm.id;
    besteKante.zuKnotenID = -1;
    double besterKantenScore = -1.0;

    // Die KI prüft alle direkt angrenzenden Wege (Nachbarknoten)
    for (int nachbarID : startFarm.nachbarKnotenIDs)
    {
        const Knoten& zielKnoten = weltKnoten[nachbarID];

        // 1. Basis-Ressourcenwert des Zielknotens berechnen
        double aktuellerScore = BerechneEinzelKnotenBasisScore(zielKnoten, kiGewichte);

        // 2. Strategischen Richtungs-Bonus addieren
        double richtungsBonus = 0.0;

        if (zielKnoten.hatHafen)
        {
            if (suchtHafen && zielKnoten.hafenTyp == zielHafenTyp)
            {
                // Passender Spezialhafen (z.B. Port2v1 für das Monopol)
                richtungsBonus = 10.0;
            }
            else if (zielKnoten.hafenTyp == 0)
            {
                // Allgemeiner 3:1 Hafen
                richtungsBonus = 6.0;
            }
        }

        double gesamtKantenScore = aktuellerScore + richtungsBonus;

        // Besten Weg loggen
        if (gesamtKantenScore > besterKantenScore)
        {
            besterKantenScore = gesamtKantenScore;
            besteKante.zuKnotenID = nachbarID;
        }
    }

    return besteKante;
}
*/

#if 0
void CPU_doRound2( H3_Game & game )
{
    auto& player = game.getCurrentPlayer();
    if (player.typ != H3_PlayerType::CPU)
    {
        DE_ERROR("No CPU player ", player.id)
    }

    if (game.m_bDebugLogic)
    {
        DE_DEBUG("CPU[",player.id,"] ",player.name)
    }

    AI_Data* ai_data = reinterpret_cast<AI_Data*>(player.ai_data);

    ai_data->bestePaare = FindeBesteStartPlaetze(game, player.id);
    DE_DEBUG("Player[",player.name,"] bestPairs = ", ai_data->bestePaare.size())
    for (size_t i = 0; i < ai_data->bestePaare.size(); ++i)
    {
        DE_DEBUG("Player[",player.name,"].bestPair[",i,"] ", ai_data->bestePaare[i].str())
    }

    if (ai_data->bestePaare.empty())
    {
        DE_ERROR("No best pair. ", player.name)
        return;
    }

    BestesStartPaar & bestPair = ai_data->bestePaare.front();

    // DEM_DEBUG("BuyFarm with Match ", match.toString() )
    game.doBuyFarm();
    H3_Corner & corner = H3_getCorner( game, __func__, bestPair.cornerX );
    game.finalizeBuyFarm( corner );



    // // DEM_DEBUG("BuyRoad with Match ", match.toString() )
    // game.doBuyRoad();
    // H3_Edge & edge = H3_getEdge( game, __func__, match.edge );
    // game.finalizeBuyRoad( edge );
/*


    std::vector< FarmMatch >
    farmMatches = CPU_findBestFarm( game, player.id );

    if ( player.state.farms.empty() )
    {
        for ( size_t i = 0; i < farmMatches.size(); ++i )
        {
            FarmMatch & match = farmMatches[ i ];
            if ( match.corner )
            {
                // DEM_DEBUG("BuyFarm with Match ", match.toString() )
                game.doBuyFarm();
                H3_Corner & corner = H3_getCorner( game, __func__, match.corner );
                game.finalizeBuyFarm( corner );

                if ( game.m_state == H3_State::Idle )
                {
                    break;
                }
            }
        }
    }

    std::vector< RoadMatch >
    roadMatches = CPU_findBestRoad( game, player.id );

    if ( player.state.roads.empty() )
    {
        for ( size_t i = 0; i < roadMatches.size(); ++i )
        {
            RoadMatch & match = roadMatches[ i ];
            if ( match.edge )
            {
                // DEM_DEBUG("BuyRoad with Match ", match.toString() )
                game.doBuyRoad();
                H3_Edge & edge = H3_getEdge( game, __func__, match.edge );
                game.finalizeBuyRoad( edge );
                if ( game.m_state == H3_State::Idle )
                {
                    break;
                }
            }
        }
    }
*/

    game.doEndTurn();
}

/*
void CPU_doRound2( H3_Game & game )
{
    auto& player = game.getCurrentPlayer();
    if (player.typ != H3_PlayerType::CPU)
    {
        DE_ERROR("No CPU player ", player.id)
    }

    if (game.m_bDebugLogic)
    {
        DE_DEBUG("CPU[",player.id,"] ",player.name)
    }

    std::vector< FarmMatch >
    farmMatches = CPU_findBestFarm( game, player.id );

    if ( player.state.farms.empty() )
    {
        for ( size_t i = 0; i < farmMatches.size(); ++i )
        {
            FarmMatch & match = farmMatches[ i ];
            if ( match.corner )
            {
                // DEM_DEBUG("BuyFarm with Match ", match.toString() )
                game.doBuyFarm();
                H3_Corner & corner = H3_getCorner( game, __func__, match.corner );
                game.finalizeBuyFarm( corner );

                if ( game.m_state == H3_State::Idle )
                {
                    break;
                }
            }
        }
    }

    std::vector< RoadMatch >
    roadMatches = CPU_findBestRoad( game, player.id );

    if ( player.state.roads.empty() )
    {
        for ( size_t i = 0; i < roadMatches.size(); ++i )
        {
            RoadMatch & match = roadMatches[ i ];
            if ( match.edge )
            {
                // DEM_DEBUG("BuyRoad with Match ", match.toString() )
                game.doBuyRoad();
                H3_Edge & edge = H3_getEdge( game, __func__, match.edge );
                game.finalizeBuyRoad( edge );
                if ( game.m_state == H3_State::Idle )
                {
                    break;
                }
            }
        }
    }

    game.doEndTurn();
}
*/
void CPU_doRound3( H3_Game & game )
{
    auto& player = game.getCurrentPlayer();
    if (player.typ != H3_PlayerType::CPU)
    {
        DE_ERROR("No CPU player ", player.id)
    }

    if (game.m_bDebugLogic)
    {
        DE_DEBUG("CPU[",player.id,"] ",player.name)
    }

    AI_Data* ai_data = reinterpret_cast<AI_Data*>(player.ai_data);

    ai_data->bestePaare = FindeBesteStartPlaetze(game, player.id);

    if (ai_data->bestePaare.empty())
    {
        DE_ERROR("No best pair. ", player.name)
        return;
    }

    BestesStartPaar & bestPair = ai_data->bestePaare.front();

    // DEM_DEBUG("BuyFarm with Match ", match.toString() )
    game.doBuyFarm();
    H3_Corner & corner = H3_getCorner( game, __func__, bestPair.cornerY );
    game.finalizeBuyFarm( corner );

    game.doEndTurn();

}

#endif

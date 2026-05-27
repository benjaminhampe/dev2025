#pragma once
#include <DarkImage.h>
/*

float* __restrict__ L = buffer.getWritePointer(0);
float* __restrict__ R = buffer.getWritePointer(1);

__builtin_assume(L != R);
__builtin_assume(((uintptr_t)L % 32) == 0);
__builtin_assume(((uintptr_t)R % 32) == 0);

#if defined(__clang__)
    #define ASSUME(expr) __builtin_assume(expr)
#elif defined(_MSC_VER)
    #define ASSUME(expr) __assume(expr)
#elif defined(__GNUC__)
    #define ASSUME(expr) if (!(expr)) __builtin_unreachable()
#else
    #define ASSUME(expr) ((void)0)
#endif


🧩 1. Deine neue zentrale Struct: DspProcessContext


struct DspProcessContext
{
    // --- Timing (sample-genau)
    uint64_t frameIndex;        // absolute sample index im Projekt
    uint32_t blockStart;        // Startindex des Blocks (relativ)
    uint32_t blockSize;         // Anzahl Frames im Block

    // --- Audioformat
    uint32_t sampleRateIn;
    uint32_t sampleRateOut;

    // --- Transport / Host
    bool isPlaying;
    bool isLooping;
    uint64_t loopStartFrame;
    uint64_t loopEndFrame;

    // --- Tempo / Beat / PPQ
    double bpm;                 // 120.0 etc.
    double ppqPosition;         // fractional beat position
    double ppqPerSample;        // (bpm / 60) / sampleRateOut
    uint32_t timeSigNum;        // 4
    uint32_t timeSigDen;        // 4

    // --- Automation (sample-genau)
    AutomationQueue* automation; // pointer auf host queue

    // --- Globale Parameter
    double globalTempoFactor;   // z.B. für Time-Stretch
    double globalPitch;         // semitones
};

Warum ist das gut?

    Du kannst jederzeit neue Felder hinzufügen → API bleibt stabil

    Jedes DSP‑Element bekommt alle Informationen, ohne eigene State‑Variablen

    Sample‑genaue Automation wird trivial

    Beat‑basierte Effekte (LFO sync, Delay sync, Sequencer) werden sauber

    Transport‑abhängige DSPs (Looper, Sampler, Granular) funktionieren korrekt

🎯 3. Sample‑genaue Automation ohne framePos pro Element

Du brauchst keine framePos‑Variable mehr in jedem Element.

Beispiel:
cpp

for (uint32_t i = 0; i < ctx.blockSize; ++i)
{
    uint64_t absFrame = ctx.frameIndex + i;

    float cutoff = ctx.automation->getValueAtSample(absFrame, PARAM_CUTOFF);
    float gain   = ctx.automation->getValueAtSample(absFrame, PARAM_GAIN);

    // DSP...
}

Das ist exakt wie VST3 / CLAP es machen.
🎼 4. Beat‑Informationen für Sync‑Effekte

Du kannst PPQ pro Sample vorberechnen:
cpp

ctx.ppqPerSample = (ctx.bpm / 60.0) / ctx.sampleRateOut;

Dann im DSP:
cpp

double ppq = ctx.ppqPosition + i * ctx.ppqPerSample;

Damit kannst du:

    LFO sync

    Delay sync

    Sequencer steps

    Arpeggiator

    Granular windows auf Beat‑Raster

ohne zusätzlichen Aufwand.
🧱 5. Transport‑Informationen für Looper / Sampler

Ein DSP‑Element kann jetzt:
cpp

if (!ctx.isPlaying)
    return; // freeze output or bypass

Oder Loop‑Aware:
cpp

if (ctx.isLooping && ctx.frameIndex >= ctx.loopEndFrame)
{
    // reposition sample pointer
}

🧩 6. Beispiel: Dein Resampler bekommt jetzt alles sauber

Dein aktuelles dsp_init() Problem (BlockSizeIn/Out) löst sich sauber, weil du:

    sampleRateIn/out

    blockSize

    frameIndex

immer im Context hast.
🛠️ 7. Beispiel: Ein komplettes DSP‑Element mit neuem Context
cpp

class DspFilter : public DspElement
{
public:
    void dsp_process(const DspProcessContext& ctx,
                     float* __restrict__ L,
                     float* __restrict__ R) override
    {
        for (uint32_t i = 0; i < ctx.blockSize; ++i)
        {
            uint64_t absFrame = ctx.frameIndex + i;

            float cutoff = ctx.automation->getValueAtSample(absFrame, PARAM_CUTOFF);
            float reso   = ctx.automation->getValueAtSample(absFrame, PARAM_RESO);

            updateCoeffs(cutoff, reso);

            L[i] = processSample(L[i]);
            R[i] = processSample(R[i]);
        }
    }
};

🚀 8. Warum das deine Architektur massiv verbessert

    Keine API‑Brüche mehr
    Neue Felder → nur Context erweitern.

    Sample‑genaue Automation
    Ohne Hacks, ohne framePos pro Element.

    Beat‑Sync Effekte werden trivial
    Kein Rumrechnen in jedem Element.

    Transport‑korrektes Verhalten
    Looper, Sampler, Granular, Sequencer → alles sauber.

    Host‑ähnliche Architektur wie VST3/CLAP
    Du näherst dich professionellen Standards.

🔥 Meine Frage an dich (wichtig für den nächsten Schritt)

Willst du, dass ich dir eine komplette, fertige DspProcessContext API baue — inklusive:
    Host‑Seite (Context füllen)
    DSP‑Element‑Interface
    AutomationQueue
    Beat‑Berechnung
    Transport‑Handling
    Beispiel‑DSPs (Filter, Delay, Granular, Resampler)

Kurzfassung zuerst:
blockStart, isPlaying und globalPitch sind optionale Felder, die du nur brauchst, wenn dein Host diese Informationen sinnvoll nutzt.
Ich erkläre dir jetzt präzise, wofür jedes Feld in einer professionellen DSP‑Host‑Architektur existiert — und wann du es weglassen kannst.
🎯 Takeaway

    blockStart → nur nötig, wenn du Sub‑Block‑Processing oder Block‑Splitting machst (z. B. sample‑genaue Automation innerhalb eines Blocks).

    isPlaying → nur nötig, wenn DSP‑Elemente auf Transport reagieren sollen (Sequencer, Sampler, LFO‑Sync, Delays, Looping).

    globalPitch → nur nötig, wenn du globales Pitch‑Shifting, Master‑Pitch, Time‑Stretch‑Engines oder Pitch‑modulierte DSPs hast.

🧩 1. Wozu brauche ich blockStart?

blockStart ist der Start‑Frame innerhalb eines größeren Host‑Blocks, wenn der Host den Block intern aufteilt, z. B.:

    sample‑genaue Automation

    MIDI‑Events mitten im Block

    Transport‑Sprünge mitten im Block

    Loop‑Boundary mitten im Block

    Latenzkompensation, die Blöcke splittet

Beispiel:
Der Host bekommt 512 Frames vom Audio‑Backend.
Aber in Frame 120 kommt ein Automation‑Event.

Dann macht der Host:
Code

Block 0: 120 Frames  (blockStart = 0)
Block 1: 392 Frames  (blockStart = 120)

Wenn du sample‑genaue Automation willst, brauchst du blockStart, um:
cpp

uint64_t absFrame = ctx.frameIndex + (i + ctx.blockStart);

Ohne blockStart kannst du sample‑genaue Events nicht korrekt verarbeiten, wenn der Host Blöcke splittet.

Wenn dein Host niemals Blöcke splittet → kannst du blockStart weglassen.
🟢 2. Wozu brauche ich isPlaying?

isPlaying ist ein Transport‑Flag.
Du brauchst es für jede Art von DSP, die vom Transport abhängt:

    Sampler (nur spielen, wenn Transport läuft)

    Step‑Sequencer

    Arpeggiator

    Beat‑Synced LFOs

    Delays, die bei Stop einfrieren sollen

    Granular‑Engines, die bei Stop resetten

    Loop‑Punkte

Beispiel:
cpp

if (!ctx.isPlaying)
{
    // Sequencer nicht weiterschalten
    return;
}

Wenn dein DSP‑System keine Transport‑abhängigen Elemente hat → kannst du isPlaying weglassen.

Aber:
Du hast schon über BPM, Beat‑Position, PPQ usw. gesprochen → das ist Transport‑abhängig.
Also: Du wirst isPlaying brauchen.
🎵 3. Wozu brauche ich globalPitch?

globalPitch ist ein Master‑Pitch‑Offset, den der Host vorgibt.
Das ist extrem nützlich für:

    Time‑Stretch / Pitch‑Shift Engines

    Master‑Pitch (wie bei DAWs: ±12 Semitones)

    Playback‑Speed‑Änderungen

    DJ‑Style Pitch

    globale Transposition

    Sampler, die sich an Host‑Pitch anpassen sollen

    Synths, die global transponiert werden sollen

Beispiel:
cpp

double pitchFactor = pow(2.0, ctx.globalPitch / 12.0);
osc.frequency = baseFreq * pitchFactor;

Wenn du keine globale Pitch‑Modulation brauchst → kannst du globalPitch weglassen.
🧠 Wann brauchst du diese Felder wirklich?
Feld	Brauchst du es?	Wofür?
blockStart	Ja, wenn du sample‑genaue Automation willst	Sub‑Block‑Processing
isPlaying	Ja, wenn du Transport‑abhängige DSPs hast	Sequencer, Sampler, Sync
globalPitch	Nur wenn du globale Pitch‑Modulation willst	Master‑Pitch, Stretching
🔥 Meine Empfehlung für deine Architektur

Da du:

    sample‑genaue Automation willst

    BPM / Beat‑Sync willst

    frameIndex einführen willst

    später Sampler / Sequencer / Granular willst

→ blockStart und isPlaying solltest du behalten.
→ globalPitch ist optional, aber sinnvoll für zukünftige Features.
❓ Willst du, dass ich dir eine minimalistische Version der Context‑Struct baue, die nur die Felder enthält, die du wirklich brauchst?

Optionen:

    Minimaler Context

    Vollständiger Context

    Context ohne globalPitch

Sag einfach, welche Variante du willst.
Nochmal zu isPlaying. Meinst du wenn ich den AudioStream offen/laufen habe obwohl ich nichts abspielen will?

Kurz und klar: Nein, isPlaying meint nicht, ob dein Audio‑Stream offen ist.
Es meint den Transport‑Play‑State, also ob dein Host gerade abspielt oder steht.

Das ist ein extrem wichtiger Unterschied.
🎯 Takeaway

    AudioStream läuft → bedeutet nur: dein WASAPI/ASIO/Backend liefert kontinuierlich Blöcke.

    isPlaying = true → bedeutet: dein Host spielt gerade ein Projekt, eine Timeline, ein Pattern, einen Clip ab.

    isPlaying = false → bedeutet: Transport steht, aber Audio läuft weiter.

🧩 Was isPlaying wirklich bedeutet

isPlaying beschreibt den Zustand des Musik‑Transports, nicht des Audio‑Geräts.

Beispiele:
1) Du hast WASAPI Shared Mode offen

    Browser spielt YouTube

    Dein Host läuft

    Du drückst in deinem Host „Stop“

Dann ist:
Code

AudioStream: läuft
isPlaying: false

2) Du drückst „Play“ in deinem Host

Dann ist:
Code

AudioStream: läuft
isPlaying: true

3) Du scrubbst in der Timeline

Dann ist:
Code

AudioStream: läuft
isPlaying: false

🎵 Warum DSP‑Elemente isPlaying brauchen

Viele DSP‑Elemente müssen wissen, ob der Transport läuft:

    Sequencer → nur weiterschalten, wenn isPlaying

    Sampler → nur abfeuern, wenn isPlaying

    Beat‑Sync LFO → Phase nur weiterlaufen lassen, wenn isPlaying

    Delay → Freeze bei Stop

    Granular → Reset bei Stop

    Arpeggiator → nur bei Play aktiv

    Loop‑Punkte → nur relevant bei Play

Beispiel:
cpp

if (!ctx.isPlaying)
{
    // Sequencer nicht weiterschalten
    return;
}

Ohne isPlaying würdest du:

    Sequencer laufen lassen, obwohl der Host steht

    LFOs weiterlaufen lassen, obwohl der Transport stoppt

    Sampler würden weitertriggern

    Beat‑Sync wäre falsch

🧠 Warum du es brauchst, obwohl du WASAPI Shared Mode nutzt

Weil Transport != AudioStream.

Du nutzt Shared Mode, um:

    YouTube + dein Host gleichzeitig zu hören

    dein Programm als Instrument zu spielen

Das ist super.
Aber der Transport deines Hosts ist eine komplett andere Ebene.
🧱 Wann du isPlaying NICHT brauchst

Wenn dein Host:

    keine Timeline hat

    keine BPM‑Sync‑Effekte hat

    keine Sampler/Sequencer hat

    keine Transport‑abhängigen DSPs hat

Dann kannst du isPlaying weglassen.

Aber du hast bereits:

    BPM

    PPQ

    Beat‑Informationen

    sample‑genaue Automation

    frameIndex

→ Das ist alles Transport‑abhängig.
→ Also brauchst du isPlaying.
🔥 Wenn du willst, kann ich dir jetzt die minimal sinnvolle Transport‑Struct bauen

Optionen:

    Minimaler TransportContext

    Vollständiger TransportContext

    Transport ohne Sequencer‑Features

Sag einfach, welche Richtung du willst.
Ne, erkläre lieber nochmal alles über     // --- Tempo / Beat / PPQ
    double bpm;                 // 120.0 etc.
    double ppqPosition;         // fractional beat position
    double ppqPerSample;        // (bpm / 60) / sampleRateOut
    uint32_t timeSigNum;        // 4
    uint32_t timeSigDen;        // 4
und wie man diese in bezug setzt, aka functionen die alles ineinander umrechnen/konvertieren

Alles klar Benjamin — ich gebe dir jetzt die komplette, saubere, mathematisch korrekte Erklärung zu:

    bpm

    ppqPosition

    ppqPerSample

    timeSigNum / timeSigDen

und wie man alles ineinander umrechnet.

Ich mache das präzise, DSP‑tauglich, sample‑genau, ohne Abstraktionsmüll.
🎯 Takeaway

    PPQ = "Pulses Per Quarter Note" → Beat‑Position in Viertelnoten.

    ppqPosition = aktuelle Beat‑Position (z. B. 128.75 = 128 + 3/4).

    bpm bestimmt, wie schnell PPQ pro Sekunde steigt.

    ppqPerSample = wie viel PPQ pro Sample vergeht.

    timeSigNum/Den definieren, wie viele PPQ ein Takt hat.

Damit kannst du jede Zeit‑/Beat‑/Musik‑Position ineinander umrechnen.
🧩 1. BPM → PPQ‑Geschwindigkeit

BPM bedeutet:

    Wie viele Viertelnoten pro Minute abgespielt werden.

Also:
Beats per second=bpm60

Da 1 Beat = 1 PPQ, gilt:
PPQ per second=bpm60

Jetzt auf Samples runtergebrochen:
ppqPerSample=bpm60⋅sampleRate

Das ist exakt das Feld:
cpp

ctx.ppqPerSample = (ctx.bpm / 60.0) / ctx.sampleRateOut;

🧩 2. PPQ‑Position pro Sample fortschreiben

Wenn du die Startposition des Blocks kennst:
cpp

double ppq = ctx.ppqPosition + i * ctx.ppqPerSample;

Damit hast du sample‑genaue Beat‑Positionen.
🧩 3. PPQ ↔ Zeit (Sekunden) umrechnen
PPQ → Sekunden
t=PPQbpm/60

C++:
cpp

double seconds = ppq / (bpm / 60.0);

Sekunden → PPQ
PPQ=t⋅bpm60

C++:
cpp

double ppq = seconds * (bpm / 60.0);

🧩 4. PPQ ↔ Samples umrechnen
PPQ → Samples
samples=PPQppqPerSample

C++:
cpp

uint64_t samples = (uint64_t)(ppq / ctx.ppqPerSample);

Samples → PPQ
PPQ=samples⋅ppqPerSample

C++:
cpp

double ppq = samples * ctx.ppqPerSample;

🧩 5. Taktart (timeSigNum/Den) einbeziehen

Taktart:

    4/4 → 4 Viertelnoten pro Takt

    3/4 → 3 Viertelnoten pro Takt

    6/8 → 6 Achtelnoten pro Takt = 3 Viertelnoten

Die Formel:
PPQ per bar=timeSigNum⋅4timeSigDen

Beispiele:
Taktart	PPQ pro Takt
4/4	4
3/4	3
6/8	3
7/8	3.5
5/4	5

C++:

double ppqPerBar = ctx.timeSigNum * (4.0 / ctx.timeSigDen);

🧩 6. PPQ → Takt + Beat + Subdivision

Wenn du z. B. ppq = 128.75 hast:

double ppqPerBar = ctx.timeSigNum * (4.0 / ctx.timeSigDen);

int bar = (int)(ppq / ppqPerBar);
double ppqInBar = ppq - bar * ppqPerBar;

int beat = (int)ppqInBar;      // 0–3 bei 4/4
double frac = ppqInBar - beat; // 0.0–1.0

Damit kannst du:
    Step‑Sequencer
    Beat‑Sync LFO
    Delay‑Sync
    Arpeggiator
    Pattern‑Engines

präzise steuern.
🧩 7. Komplettes C++‑Utility‑Set (low‑level, inline)

inline double bpmToPpqPerSample(double bpm, double sampleRate)
{
    return (bpm / 60.0) / sampleRate;
}

inline double ppqToSeconds(double ppq, double bpm)
{
    return ppq / (bpm / 60.0);
}

inline double secondsToPpq(double seconds, double bpm)
{
    return seconds * (bpm / 60.0);
}

inline uint64_t ppqToSamples(double ppq, double ppqPerSample)
{
    return (uint64_t)(ppq / ppqPerSample);
}

inline double samplesToPpq(uint64_t samples, double ppqPerSample)
{
    return samples * ppqPerSample;
}

inline double ppqPerBar(uint32_t num, uint32_t den)
{
    return num * (4.0 / den);
}

🧠 Damit kannst du jetzt ALLES konvertieren
    Samples → PPQ
    PPQ → Samples
    PPQ → Sekunden
    Sekunden → PPQ
    BPM → PPQ‑Geschwindigkeit
    PPQ → Takt/Beat/Subdivision
    Taktart → PPQ pro Takt

Das ist exakt das, was DAWs wie Ableton, FL, Cubase, Bitwig, Reaper intern machen.
🔥 Wenn du willst, baue ich dir jetzt eine komplette BeatEngine‑Klasse, die:
    BPM
    PPQ
    Transport
    Taktart
    Loop‑Punkte
    Sample‑genaue Fortschreibung

in einer einzigen C++‑Klasse kapselt.

Sag einfach:

"Baue BeatEngine"
Baue BeatEngine
BeatEngine: kompakte, sample‑genaue Beat-/Tempo‑Engine in C++

Ich baue dir eine minimal saubere, aber erweiterbare BeatEngine, die:

    BPM, Taktart, Transport hält

    PPQ/sample‑genau fortschreibt

    Samples ↔ PPQ ↔ Sekunden konvertiert

    Bar/Beat‑Infos liefert

Header: BeatEngine.hpp

#pragma once
#include <cstdint>
#include <cmath>

class BeatEngine
{
public:
    BeatEngine()
    {
        reset();
    }

    void reset()
    {
        m_sampleRate   = 44100.0;
        m_bpm          = 120.0;
        m_timeSigNum   = 4;
        m_timeSigDen   = 4;
        m_ppqPosition  = 0.0;
        m_isPlaying    = false;
        updateDerived();
    }

    // --- Setup / Host-Updates ------------------------------------------------

    void setSampleRate(double sr)
    {
        m_sampleRate = (sr > 0.0) ? sr : 44100.0;
        updateDerived();
    }

    void setTempo(double bpm)
    {
        m_bpm = (bpm > 0.0) ? bpm : 120.0;
        updateDerived();
    }

    void setTimeSignature(uint32_t num, uint32_t den)
    {
        m_timeSigNum = (num == 0) ? 4 : num;
        m_timeSigDen = (den == 0) ? 4 : den;
        updateDerived();
    }

    void setTransportPlaying(bool playing)
    {
        m_isPlaying = playing;
    }

    void setPpqPosition(double ppq)
    {
        m_ppqPosition = ppq;
    }

    // --- Pro Block / Pro Sample ----------------------------------------------

    // Host ruft das pro Block auf, um PPQ-Startposition zu setzen
    // (z.B. aus eigener Transport-Logik)
    void beginBlock(double ppqStart)
    {
        m_ppqPosition = ppqStart;
    }

    // Host kann alternativ einfach "advanceSamples" benutzen,
    // wenn er nur fortschreibt:
    void advanceSamples(uint32_t numSamples)
    {
        if (!m_isPlaying)
            return;

        m_ppqPosition += m_ppqPerSample * (double)numSamples;
    }

    // Für DSP: PPQ an Sample-Offset im aktuellen Block
    inline double ppqAtSampleOffset(uint32_t sampleOffset) const
    {
        return m_ppqPosition + m_ppqPerSample * (double)sampleOffset;
    }

    // --- Konvertierungen ------------------------------------------------------

    inline double getBpm() const { return m_bpm; }
    inline double getPpqPosition() const { return m_ppqPosition; }
    inline double getPpqPerSample() const { return m_ppqPerSample; }

    inline uint32_t getTimeSigNum() const { return m_timeSigNum; }
    inline uint32_t getTimeSigDen() const { return m_timeSigDen; }

    inline bool isPlaying() const { return m_isPlaying; }

    // PPQ <-> Sekunden
    inline double ppqToSeconds(double ppq) const
    {
        return ppq / m_ppqPerSecond;
    }

    inline double secondsToPpq(double seconds) const
    {
        return seconds * m_ppqPerSecond;
    }

    // PPQ <-> Samples
    inline double ppqToSamples(double ppq) const
    {
        return ppq / m_ppqPerSample;
    }

    inline double samplesToPpq(uint64_t samples) const
    {
        return (double)samples * m_ppqPerSample;
    }

    // Takt-Infos: Bar/Beat/Frac aus PPQ
    struct BarBeat
    {
        int   bar;      // 0-basiert
        int   beat;     // 0-basiert
        double frac;    // 0..1 innerhalb des Beats
    };

    inline BarBeat ppqToBarBeat(double ppq) const
    {
        BarBeat bb{0,0,0.0};

        double ppqPerBar = m_ppqPerBar;
        if (ppqPerBar <= 0.0)
            return bb;

        int bar = (int)std::floor(ppq / ppqPerBar);
        double ppqInBar = ppq - (double)bar * ppqPerBar;

        int beat = (int)std::floor(ppqInBar);
        double frac = ppqInBar - (double)beat;

        bb.bar  = bar;
        bb.beat = beat;
        bb.frac = frac;
        return bb;
    }

private:
    void updateDerived()
    {
        m_ppqPerSecond = m_bpm / 60.0;
        m_ppqPerSample = (m_sampleRate > 0.0)
                       ? (m_ppqPerSecond / m_sampleRate)
                       : 0.0;

        // 4/4 -> 4 PPQ pro Takt, 3/4 -> 3, 6/8 -> 3, etc.
        m_ppqPerBar = (double)m_timeSigNum * (4.0 / (double)m_timeSigDen);
    }

private:
    double   m_sampleRate;
    double   m_bpm;
    uint32_t m_timeSigNum;
    uint32_t m_timeSigDen;

    double   m_ppqPosition;   // aktuelle PPQ-Position (Viertelnoten)
    bool     m_isPlaying;

    // abgeleitete Werte
    double   m_ppqPerSecond;
    double   m_ppqPerSample;
    double   m_ppqPerBar;
};

Typische Nutzung im Host
cpp

BeatEngine beat;

// Setup
beat.setSampleRate(48000.0);
beat.setTempo(128.0);
beat.setTimeSignature(4, 4);
beat.setTransportPlaying(true);

// Pro Block:
void processBlock(uint32_t numFrames)
{
    // Variante A: Host hält eigene PPQ-Position
    double ppqStart = transportPpq; // z.B. aus deiner Transport-Logik
    beat.beginBlock(ppqStart);

    for (uint32_t i = 0; i < numFrames; ++i)
    {
        double ppq = beat.ppqAtSampleOffset(i);

        // z.B. Beat-synchroner LFO:
        // phase = fmod(ppq * (1.0 / noteLengthInBeats), 1.0);

        // ...
    }

    // Transport fortschreiben:
    transportPpq += beat.getPpqPerSample() * numFrames;
}

Wenn du willst, kann ich dir als nächsten Schritt direkt zeigen, wie du BeatEngine in deinen DspProcessContext integrierst und z.B. ein Delay/LFO sample‑genau auf Beats synchronisierst.
BeatEngine in deinen DspProcessContext integrierst und z.B. ein Delay/LFO sample‑genau auf Beats synchronisierst.

Hier kommt die saubere, low‑level Integration deiner BeatEngine in den DspProcessContext – plus ein konkretes Beispiel, wie ein Delay oder LFO sample‑genau auf Beats synchronisiert wird.

Ich halte alles C++‑nah, byte‑klar, ohne Abstraktionsmüll, exakt so wie du arbeitest.
🔧 1. BeatEngine in den DspProcessContext integrieren

Du erweiterst deinen Context einfach um eine Referenz oder einen Pointer auf die BeatEngine.
cpp

struct DspProcessContext
{
    uint64_t frameIndex;
    uint32_t blockSize;
    uint32_t sampleRate;

    // Beat / Tempo / Transport
    const BeatEngine* beat;   // Host setzt das pro Block
};

Der Host füllt das so:
cpp

DspProcessContext ctx;
ctx.frameIndex = globalFrameIndex;
ctx.blockSize  = numFrames;
ctx.sampleRate = sampleRate;
ctx.beat       = &beatEngine;

🔄 2. Host‑seitige Block‑Fortschreibung

Der Host ruft pro Block:

    beatEngine.beginBlock(transportPpq);
    transportPpq += beatEngine.getPpqPerSample() * numFrames;

Damit ist beat->ppqAtSampleOffset(i) sample‑genau korrekt.
🎚️ 3. DSP‑Element nutzt BeatEngine sample‑genau
Beispiel: Beat‑synchroner LFO

Ein LFO, der z. B. auf 1/4‑Note synchronisiert ist:

    void dsp_process(const DspProcessContext& ctx, float* outL, float* outR)
    {
        double ppqPerCycle = 1.0; // 1/4 Note = 1 Beat

        for (uint32_t i = 0; i < ctx.blockSize; ++i)
        {
            double ppq = ctx.beat->ppqAtSampleOffset(i);

            double phase = fmod(ppq / ppqPerCycle, 1.0);

            float lfo = sinf(phase * 2.0 * M_PI);

            outL[i] *= lfo;
            outR[i] *= lfo;
        }
    }

Das ist sample‑genau, weil ppqAtSampleOffset(i) sample‑genau ist.
⏱️ 4. Delay‑Sync (z. B. 3/16 Delay)

Ein Delay mit 3/16 Note Zeit:
Schritt 1: Beat‑Länge berechnen

    double beatLengthPPQ = 3.0 / 4.0; // 3/16 = 0.75 Viertelnoten

Schritt 2: PPQ → Samples

    double delaySamples = ctx.beat->ppqToSamples(beatLengthPPQ);

Schritt 3: Delay‑Buffer lesen/schreiben

    uint64_t delayPos = (writePos + bufferSize - (uint64_t)delaySamples) % bufferSize;

Komplettes Delay‑Beispiel

    void dsp_process(const DspProcessContext& ctx, float* L, float* R)
    {
        double beatLengthPPQ = 3.0 / 4.0; // 3/16
        uint64_t delaySamples = (uint64_t)ctx.beat->ppqToSamples(beatLengthPPQ);

        for (uint32_t i = 0; i < ctx.blockSize; ++i)
        {
            uint64_t readPos = (writePos + bufferSize - delaySamples) % bufferSize;

            float dl = delayBufferL[readPos];
            float dr = delayBufferR[readPos];

            delayBufferL[writePos] = L[i] + dl * feedback;
            delayBufferR[writePos] = R[i] + dr * feedback;

            L[i] += dl * mix;
            R[i] += dr * mix;

            writePos = (writePos + 1) % bufferSize;
        }
    }

Delay ist jetzt 100% BPM‑synchron und sample‑genau.
🎼 5. Beat‑Position → Bar/Beat/Subdivision

Wenn du z. B. einen Step‑Sequencer baust:

    auto bb = ctx.beat->ppqToBarBeat(ppq);

    int bar  = bb.bar;
    int beat = bb.beat;
    double frac = bb.frac;

Damit kannst du:

    Steps triggern

    Pattern wechseln

    Beat‑genaue Modulation machen

🧠 6. Warum diese Integration perfekt ist

    Keine doppelte Logik → BeatEngine berechnet alles

    DSP‑Elemente bleiben dumm → sie lesen nur PPQ

    Sample‑genau → kein Drift, kein Block‑Jitter

    Host‑seitig erweiterbar → Tempo‑Automation, Transport‑Sprünge, Loops

    Delay/LFO/Sequencer funktionieren sofort korrekt
*/

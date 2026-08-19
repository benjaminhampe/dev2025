#pragma once

#include <cstdint>
#include <string>
#include <vector>

// ---------------- Plattformneutrale Audiotypen ----------------

// Sample-Typen, die deine Engine kennt.
// Keine Plattformdetails, nur logische Repräsentation.
enum class AudioSampleType : uint8_t {
    Unknown = 0,
    Int16,
    Int24,
    Int32,
    Float32,
    Float64
};

// Beschreibung eines Audioformats.
// Wird benutzt, um Streams zu konfigurieren und DSP zu informieren.
struct AudioFormat {
    uint32_t sampleRate;   // z.B. 44100, 48000, 96000
    uint8_t  channels;     // 1..32
    AudioSampleType sampleType;
    uint32_t channelMask;  // 0 = unbekannt / nicht gesetzt
};

// Lebenszyklus eines Audio-Streams.
// Wichtig für Engine-Logik, UI, Fehlerbehandlung.
enum class AudioStreamState : uint8_t {
    Stopped = 0,   // Stream ist nicht aktiv, keine Threads, keine IO
    Starting,      // Stream wird initialisiert (Device öffnen, Buffer holen)
    Running,       // Stream läuft, Callback wird regelmäßig aufgerufen
    Stopping,      // Stream wird gerade gestoppt (Stop, Drain, Cleanup)
    Error          // Fehlerzustand (z.B. Device weg, Init fehlgeschlagen)
};

// Abstrakte DSP-Chain.
// Deine Engine implementiert dieses Interface und füllt den Output.
struct IAudioDsp {
    virtual ~IAudioDsp() = default;

    // out: Zeiger auf interleavte Samples (float, aligned)
    // frames: Anzahl Frames
    // format: Format (Samplerate, Kanäle, etc.)
    //
    // Erwartung:
    // - Du füllst out[0 .. frames*channels-1] mit Samples im Bereich [-1, 1].
    // - Kein Blocking, keine langen Locks – das läuft im Audio-Thread.
    virtual void process(float* out, uint32_t frames, const AudioFormat& format) = 0;
};

// Plattformneutrale Beschreibung eines Devices, soweit für Stream nötig.
// Die plattformspezifische Implementierung kennt die Details (z.B. WASAPI-Handle).
struct AudioDeviceDesc {
    std::string id;        // stabile ID (z.B. von deiner Device-Enumeration)
    std::string name;      // Anzeigename
    bool isOutput = false; // dieses Device kann Audio ausgeben
    bool isInput = false;  // dieses Device kann Audio aufnehmen
};

// Öffnet einen Audio-Output-Stream auf einem konkreten Device.
// Plattformdetails (WASAPI, ALSA, CoreAudio) sind in der .cpp versteckt.
class AudioStream {
public:
    // Konstruktor: speichert nur Parameter, macht noch kein IO.
    AudioStream(const AudioDeviceDesc& dev,
                const AudioFormat& fmt,
                IAudioDsp* dsp);

    // Startet den Stream.
    // Gibt true zurück, wenn der Startvorgang erfolgreich angestoßen wurde.
    // Im Fehlerfall geht der Stream in den State Error.
    bool start();

    // Stoppt den Stream sauber.
    // Wartet, bis der Audio-Thread beendet ist.
    void stop();

    // Liefert den aktuellen State.
    AudioStreamState state() const;

private:
    // Plattformunabhängige Daten
    AudioDeviceDesc deviceInfo;
    AudioFormat format;
    IAudioDsp* dspChain = nullptr;

    // Plattformabhängige Implementierung ist in der .cpp versteckt.
    struct Impl;
    Impl* impl = nullptr;
};

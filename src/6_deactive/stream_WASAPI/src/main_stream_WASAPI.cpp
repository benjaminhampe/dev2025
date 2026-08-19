#include <iostream>
#include <thread>
#include <chrono>

#include "AudioStream.h"

// ------------------------------------------------------------
// Beispiel-DSP: einfacher Sinusgenerator
// ------------------------------------------------------------
// Diese DSP-Chain erzeugt einen Sinuston. In einer echten Engine
// würdest du hier Filter, Mixer, Effekte, Routing usw. einbauen.
// ------------------------------------------------------------
class SineDsp : public IAudioDsp {
public:
    explicit SineDsp(float freq)
        : frequency(freq) {}

    void process(float* out, uint32_t frames, const AudioFormat& format) override {
        // Jede DSP-Chain bekommt interleavte float-Samples.
        // Wir erzeugen einen Sinus für alle Kanäle.
        for (uint32_t i = 0; i < frames; ++i) {
            float s = std::sin(phase);
            phase += 2.0f * 3.1415926535f * frequency / format.sampleRate;

            if (phase > 2.0f * 3.1415926535f)
                phase -= 2.0f * 3.1415926535f;

            // Alle Kanäle mit dem gleichen Sample füllen.
            for (uint8_t c = 0; c < format.channels; ++c)
                out[i * format.channels + c] = s;
        }
    }

private:
    float frequency;
    float phase = 0.0f;
};

// ------------------------------------------------------------
// Hilfsfunktion: Stream-State als Text
// ------------------------------------------------------------
const char* stateToString(AudioStreamState s) {
    switch (s) {
        case AudioStreamState::Stopped:  return "Stopped";
        case AudioStreamState::Starting: return "Starting";
        case AudioStreamState::Running:  return "Running";
        case AudioStreamState::Stopping: return "Stopping";
        case AudioStreamState::Error:    return "Error";
    }
    return "Unknown";
}

// ------------------------------------------------------------
// main()
// ------------------------------------------------------------
// Dieses Beispiel:
// 1. Wählt ein Device (hier: Dummy-Daten, du nutzt deine Enumeration)
// 2. Erstellt eine DSP-Chain (Sinusgenerator)
// 3. Startet den Stream
// 4. Lässt ihn 5 Sekunden laufen
// 5. Stoppt ihn sauber
// ------------------------------------------------------------
int main() {
    std::cout << "AudioStream Testprogramm\n";

    // --------------------------------------------------------
    // 1. Device auswählen
    // --------------------------------------------------------
    // In deiner Engine würdest du hier deine Device-Liste nutzen.
    // Für dieses Beispiel nehmen wir ein Dummy-Device, dessen ID
    // mit deiner WASAPI-Enumeration übereinstimmen muss.
    AudioDeviceDesc device;
    device.id = "{0.0.0.00000000}.{SOME-REAL-WASAPI-ID-HERE}";
    device.name = "Default Output Device";
    device.isOutput = true;

    // --------------------------------------------------------
    // 2. AudioFormat definieren
    // --------------------------------------------------------
    AudioFormat format;
    format.sampleRate = 48000;
    format.channels = 2;
    format.sampleType = AudioSampleType::Float32;
    format.channelMask = 0; // optional

    // --------------------------------------------------------
    // 3. DSP-Chain erstellen
    // --------------------------------------------------------
    SineDsp dsp(440.0f); // 440 Hz Sinus

    // --------------------------------------------------------
    // 4. Stream erstellen
    // --------------------------------------------------------
    AudioStream stream(device, format, &dsp);

    // --------------------------------------------------------
    // 5. Stream starten
    // --------------------------------------------------------
    std::cout << "Starte Stream...\n";
    if (!stream.start()) {
        std::cout << "Fehler: Stream konnte nicht gestartet werden.\n";
        return 1;
    }

    // Warten, bis der Stream wirklich läuft
    while (stream.state() == AudioStreamState::Starting) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    if (stream.state() != AudioStreamState::Running) {
        std::cout << "Stream ist nicht im Running-State: "
                  << stateToString(stream.state()) << "\n";
        return 1;
    }

    std::cout << "Stream läuft.\n";

    // --------------------------------------------------------
    // 6. 5 Sekunden laufen lassen
    // --------------------------------------------------------
    for (int i = 0; i < 50; ++i) {
        if (stream.state() == AudioStreamState::Error) {
            std::cout << "Stream-Fehler während Laufzeit!\n";
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // --------------------------------------------------------
    // 7. Stream stoppen
    // --------------------------------------------------------
    std::cout << "Stoppe Stream...\n";
    stream.stop();

    std::cout << "Stream gestoppt. State = "
              << stateToString(stream.state()) << "\n";

    return 0;
}

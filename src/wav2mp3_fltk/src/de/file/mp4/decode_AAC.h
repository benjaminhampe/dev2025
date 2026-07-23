/*

SampleInfo‑Tabelle um FAAD2 korrekt zu füttern.
🎯 Was du hast

Du hast jetzt:
cpp

struct SampleInfo {
    uint64_t offset;
    uint32_t size;
    uint64_t dts;
    uint32_t duration;
    uint64_t cts;
};

std::vector<SampleInfo> samples;

Und du hast:

    den MP4‑File im Speicher oder als File‑Handle

    die AudioSpecificConfig (ASC) aus esds → für NeAACDecInit2()

    einen initialisierten FAAD2‑Decoder

🎯 Was FAAD2 braucht

Für jeden AAC‑Frame:

    Pointer auf die rohen AAC‑Bytes

    Frame‑Größe

    Keine ADTS‑Header

    Keine MP4‑Boxen

    Nur der nackte AAC‑Frame

Genau das liefert deine Sample‑Tabelle.
🔥 Minimaler, korrekter C++‑Code: Sample‑Tabelle → FAAD2‑Decoder
cpp

void decodeAAC(
    NeAACDecHandle h,
    const uint8_t* fileData,
    const std::vector<SampleInfo>& samples,
    std::function<void(const float*, size_t)> pcmCallback
)
{
    for (size_t i = 0; i < samples.size(); i++) {

        const SampleInfo& s = samples[i];

        const uint8_t* frame = fileData + s.offset;
        uint32_t frameSize   = s.size;

        NeAACDecFrameInfo info;
        void* pcm = NeAACDecDecode(h, &info, frame, frameSize);

        if (info.error != 0) {
            // Fehlerbehandlung
            continue;
        }

        // PCM ist float* oder int16*, abhängig von FAAD2-Config
        float* pcmF = reinterpret_cast<float*>(pcm);
        size_t samplesOut = info.samples; // Anzahl PCM-Samples

        pcmCallback(pcmF, samplesOut);
    }
}

🧩 Wie du den Decoder initialisierst

    NeAACDecHandle h = NeAACDecOpen();

    unsigned long sampleRate = 0;
    unsigned char channels = 0;

    NeAACDecInit2(h, ascData, ascSize, &sampleRate, &channels);

    ascData = nur ASC‑Payload, nicht ESDS‑Header.
    
🧩 Wie du alles zusammensteckst

// 1. MP4-Datei laden
std::vector<uint8_t> fileData = loadFile("audio.m4a");

// 2. Sample-Tabelle bauen
std::vector<SampleInfo> samples = build_sample_table(
    stsz_sizes,
    stsc_first_chunk,
    stsc_samples_per_chunk,
    stco_chunk_offsets,
    stts_sample_count,
    stts_sample_delta,
    ctts_sample_count,
    ctts_sample_offset
);

// 3. FAAD2 initialisieren
NeAACDecHandle h = NeAACDecOpen();
unsigned long sr = 0;
unsigned char ch = 0;
NeAACDecInit2(h, asc, ascSize, &sr, &ch);

// 4. Dekodieren
decodeAAC(h, fileData.data(), samples,
    [&](const float* pcm, size_t count) {
        // PCM verarbeiten
    }
);

NeAACDecClose(h);

💯 Warum das funktioniert

Deine Sample‑Tabelle liefert:
Feld	Bedeutung
offset	Wo im MP4‑File der AAC‑Frame beginnt
size	Wie viele Bytes der Frame hat
dts/cts	Für Audio irrelevant, aber korrekt berechnet
duration	AAC‑Frame‑Dauer (z. B. 1024 Samples)

FAAD2 braucht nur offset + size → perfekt.

*/
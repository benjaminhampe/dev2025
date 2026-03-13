// main.cpp
// Minimalbeispiel: Audio (wav/mp3) -> deutscher Text mit whisper.cpp
// Backend-Priorität (build-zeitig):
//   1. Vulkan (GGML_USE_VULKAN)
//   2. OpenCL + CLBlast (GGML_USE_CLBLAST)
//   3. OpenBLAS (GGML_USE_OPENBLAS)
// Kompilieren Sie whisper.cpp mit den entsprechenden Flags/Libraries.

#include "whisper.h"

#include "AudioFile.h"
#include "AudioFileReader_MP3.h"
#include "AudioFileReader_WAV.h"
#include "DownMixMono.h"
#include "ResampleEasy.h"
#include "ResampleSpeexDsp.h"

#include <iostream>

// --- Backend-Info (nur zur Anzeige) -----------------------------------------

inline std::string get_backend_name()
{
#if defined(GGML_USE_VULKAN)
    return "Vulkan";
#elif defined(GGML_USE_CLBLAST)
    return "OpenCL + CLBlast";
#elif defined(GGML_USE_OPENBLAS)
    return "OpenBLAS";
#else
    return "CPU (kein spezielles Backend aktiviert)";
#endif
}


// --- Transkriptionsfunktion --------------------------------------------------

inline bool
transcribe_to_german(const std::string & model_uri,
                     const std::string & audio_url)
{
    std::cout << "Lade Model: " << model_uri << "\n";
    std::cout << "Lade Audio: " << audio_url << "\n";
    std::cout << "Backend:    " << get_backend_name() << "\n";

    AudioFile file;
    if (!load_mp3_f32(file, audio_url))
    {
        std::cerr << "Fehler beim Laden der Audiodatei: " << audio_url << "\n";
        return false;
    }

    std::cout << "AudioLoad complete: " << file.str() << std::endl;

    DownMixToMono( file );

    std::cout << "DownMix complete: " << file.str() << std::endl;

    resample_speex_f32( file, 16000 );

    std::cout << "Resample complete: " << file.str() << std::endl;

    struct whisper_context_params cparams = whisper_context_default_params();
    // if your build supports it, this will use GPU (Vulkan) automatically
    // e.g. compiled with -DGGML_VULKAN=ON
    cparams.use_gpu = true;

    struct whisper_context* ctx = whisper_init_from_file_with_params(model_uri.c_str(), cparams);
    if (!ctx)
    {
        std::cerr << "Fehler: Konnte Whisper-Kontext nicht initialisieren.\n";
        return false;
    }


    // Standard-Parameter
    whisper_full_params wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
    wparams.translate = false; // wir wollen direkt Deutsch, keine Übersetzung ins Englische
    wparams.no_context = false; // not to use previous text as context for the next segment.
    wparams.max_len = -1; // unendlich tokens
    // wparams.speed_up = false;
    // wparams.language  = "de"; // target language
    // wparams.detect_language = true; // (automatische Erkennung)
    wparams.n_threads = 8;
    // wparams.n_max_text_ctx = 0;
    // wparams.offset_ms = 0;
    // wparams.single_segment = true; // Output as one big segment.
    // wparams.print_realtime   = true;
    wparams.print_progress   = true;
    wparams.print_timestamps = true;
    wparams.print_special    = true;

    // Transkription
    std::cout << "Starte Transkription...\n";

    auto ok = whisper_full(ctx, wparams,
                reinterpret_cast<const float*>(file.samples.data()),
                int(file.frame_count * file.channels));
    if (ok != 0)
    {
        std::cerr << "Fehler: whisper_full() schlug fehl.\n";
        whisper_free(ctx);
        return false;
    }

    // Ausgabe der Segmente
    const int n_segments = whisper_full_n_segments(ctx);
    std::cout << "\n--- Transkript (Deutsch) ---\n";

    std::cout << "Got n_segments = " << n_segments << "\n";
    for (int i = 0; i < n_segments; ++i)
    {
        const char * text = whisper_full_get_segment_text(ctx, i);
        std::cout << "[" << i << "]" << text << "\n";
    }
    std::cout << "\n----------------------------\n";

    whisper_free(ctx);
    return true;
}

// --- main --------------------------------------------------------------------

int main(int argc, char ** argv) 
{
/*
    if (argc < 3) {
        std::cerr << "Nutzung:\n"
                  << "  " << argv[0] << " <modell.bin> <audio.wav|audio.mp3>\n\n"
                  << "Beispiel:\n"
                  << "  " << argv[0] << " ggml-base.de.bin input.wav\n";
        return 1;
    }
*/
    const std::string media_dir = "../../media/whisper.cpp";
    const std::string model_uri = media_dir + "/models/ggml-large-v3-turbo-q8_0.bin";
    //const std::string audio_uri = media_dir + "/input/jfk.mp3";
    const std::string audio_uri = media_dir + "/input/AmourPlastique.mp3";

    // const std::string model_uri = argv[1];
    // const std::string audio_uri = argv[2];

    if (!transcribe_to_german(model_uri, audio_uri))
    {
        return 1;
    }

    return 0;
}

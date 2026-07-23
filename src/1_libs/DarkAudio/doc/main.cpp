#include <QApplication>
#include <MainWindow.h>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    //QCoreApplication::setAttribute(Qt::AA_NativeWindows);

    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.resize(800, 600); // Fenstergröße einstellen
    mainWindow.show();

    int result = app.exec();

    return result;
}


/*

// Funktion zum Konvertieren in PCM float32
void writePCMFloat32(const std::vector<float>& buffer, const std::string& filename)
{
    std::ofstream outFile(filename, std::ios::binary);    // Öffnen der Ausgabedatei im Binärmodus
    if (!outFile) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    for (float sample : buffer) 
    {
        outFile.write(reinterpret_cast<const char*>(&sample), sizeof(float)); // Schreiben der float-Werte in die Datei
    }

    outFile.close();                                      // Schließen der Datei
}

int main() {
    float frequency = 440.0f;  // Frequenz der A4-Note (440 Hz)
    float duration = 1.0f;     // Dauer des Tons (1 Sekunde)
    int sampleRate = 44100;    // Abtastrate (44.1 kHz)

    std::vector<float> audioSignal = karplusStrong(frequency, duration, sampleRate); // Erzeugen des Audiosignals

    writePCMFloat32(audioSignal, "output.pcm");          // Schreiben des Audiosignals in eine PCM-Datei

    std::cout << "Audio signal generated and saved to output.pcm" << std::endl; // Ausgabe einer Bestätigung

    return 0;                                            // Beenden des Programms
}

// Gedämpfte Sinusfunktion
std::vector<float> dampedSine(float frequency, float duration, int sampleRate, float dampingFactor) {
    std::vector<float> buffer(static_cast<int>(sampleRate * duration));
    for (int i = 0; i < buffer.size(); ++i) {
        float t = static_cast<float>(i) / sampleRate;
        buffer[i] = exp(-dampingFactor * t) * sin(2.0f * M_PI * frequency * t);
    }
    return buffer;
}

int main() {
    float frequency = 440.0f;  // A4 Note
    float duration = 1.0f;     // 1 Sekunde
    int sampleRate = 44100;
    float dampingFactor = 2.0f; // Dämpfungsfaktor

    std::vector<float> audioSignal = dampedSine(frequency, duration, sampleRate, dampingFactor);

    writePCMFloat32(audioSignal, "damped_sine.pcm");

    std::cout << "Audio signal generated and saved to damped_sine.pcm" << std::endl;

    return 0;
}



// Funktion zur Konvertierung von PCM float32 zu PCM short
std::vector<int16_t> convertFloat32ToPCMShort(const std::vector<float>& floatBuffer) {
    std::vector<int16_t> shortBuffer(floatBuffer.size());
    std::transform(floatBuffer.begin(), floatBuffer.end(), shortBuffer.begin(),
                   [](float sample) {
                       return static_cast<int16_t>(std::max(-1.0f, std::min(1.0f, sample)) * 32767);
                   });
    return shortBuffer;
}

// Funktion zum Schreiben von PCM short in eine Datei
void writePCMShort(const std::vector<int16_t>& buffer, const std::string& filename) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }
    outFile.write(reinterpret_cast<const char*>(buffer.data()), buffer.size() * sizeof(int16_t));
    outFile.close();
}


#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstdint>

// Funktion zur Konvertierung von PCM float32 zu PCM short
std::vector<int16_t> convertFloat32ToPCMShort(const std::vector<float>& floatBuffer) {
    std::vector<int16_t> shortBuffer(floatBuffer.size());
    std::transform(floatBuffer.begin(), floatBuffer.end(), shortBuffer.begin(),
                   [](float sample) {
                       if (sample >= 0.0f) {
                           return static_cast<int16_t>(std::min(sample, 1.0f) * 32767.0f);
                       } else {
                           return static_cast<int16_t>(std::max(sample, -1.0f) * 32768.0f);
                       }
                   });
    return shortBuffer;
}

// Funktion zum Schreiben von PCM short in eine Datei
void writePCMShort(const std::vector<int16_t>& buffer, const std::string& filename) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }
    outFile.write(reinterpret_cast<const char*>(buffer.data()), buffer.size() * sizeof(int16_t));
    outFile.close();
}

int main() {
    // Beispiel: Einfache float32-Daten
    std::vector<float> floatBuffer = {0.0f, 0.5f, -0.5f, 1.0f, -1.0f};

    // Konvertierung zu PCM short
    std::vector<int16_t> shortBuffer = convertFloat32ToPCMShort(floatBuffer);

    // Schreiben der PCM short-Daten in eine Datei
    writePCMShort(shortBuffer, "output.pcm");

    std::cout << "Conversion complete and saved to output.pcm" << std::endl;

    return 0;
}

int main() {
    // Beispiel: Einfache float32-Daten
    std::vector<float> floatBuffer = {0.0f, 0.5f, -0.5f, 1.0f, -1.0f};

    // Konvertierung zu PCM short
    std::vector<int16_t> shortBuffer = convertFloat32ToPCMShort(floatBuffer);

    // Schreiben der PCM short-Daten in eine Datei
    writePCMShort(shortBuffer, "output.pcm");

    std::cout << "Conversion complete and saved to output.pcm" << std::endl;

    return 0;
}
*/

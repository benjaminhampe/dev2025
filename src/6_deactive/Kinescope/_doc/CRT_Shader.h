// Wir verwenden GLSL Version 330, die moderne Shader-Funktionalität wie Textursampling und Vektormath unterstützt.
#version 330 core

// Die Textur mit dem Bildschirminhalt (z. B. Spielszene oder UI), die wir verzerren und manipulieren.
uniform sampler2D screenTex;

// Die Bildschirmauflösung (z. B. 1920x1080), nötig für korrekte Skalierung von Effekten wie Scanlines.
uniform vec2 resolution;

// Zeitvariable für animierte Effekte wie Flackern oder Nachleuchten.
uniform float time;

// Die aktuelle Pixelposition im Bildschirmraum, übergeben vom Vertex-Shader.
// Bereich: [0, resolution.x] × [0, resolution.y]
in vec2 fragCoord;

// Die finale Farbe, die dieser Pixel ausgibt.
out vec4 fragColor;

void main() {

    // Normiere die Pixelkoordinaten in den Bereich [0.0, 1.0].
    // Das ist nötig, um sie als UV-Koordinaten für Textursampling zu verwenden.
    vec2 uv = fragCoord / resolution;

    // Verschiebe die UVs in den Bereich [-1.0, 1.0].
    // Vorher: uv ∈ [0.0, 1.0], Mittelpunkt bei (0.5, 0.5)
    // Nachher: uv ∈ [-1.0, 1.0], Mittelpunkt bei (0.0, 0.0)
    // Warum? Viele Effekte wie Krümmung oder Vignette basieren auf der Entfernung vom Zentrum.
    uv = uv * 2.0 - 1.0;

    // Verzerrung entlang der X-Achse abhängig von der Y-Position.
    // Simuliert die horizontale Krümmung eines CRT-Bildschirms.
    // Je weiter oben/unten ein Pixel liegt, desto stärker wird es horizontal gestreckt.
    uv.x *= 1.0 + 0.1 * pow(abs(uv.y), 2.0);

    // Verzerrung entlang der Y-Achse abhängig von der X-Position.
    // Simuliert die vertikale Krümmung des Bildschirms.
    // Je weiter links/rechts ein Pixel liegt, desto stärker wird es vertikal gestreckt.
    uv.y *= 1.0 + 0.1 * pow(abs(uv.x), 2.0);

    // Bringe die UVs zurück in den Bereich [0.0, 1.0], damit wir sie für das Textursampling verwenden können.
    // Vorher: uv ∈ [-1.0, 1.0]
    // Nachher: uv ∈ [0.0, 1.0]
    uv = uv * 0.5 + 0.5;

    // Stärke der chromatischen Aberration.
    // Ein kleiner Wert sorgt für subtile Farbverschiebung der RGB-Kanäle.
    float aberration = 0.002;

    // Initialisiere die Farbvariable, die später die manipulierten RGB-Werte enthält.
    vec3 color;

    // Roter Kanal wird leicht nach rechts verschoben.
    // Simuliert die Brechung von Licht in der Glasstruktur des CRTs.
    color.r = texture(screenTex, uv + vec2(aberration, 0.0)).r;

    // Grüner Kanal bleibt zentral.
    // Dient als Referenz und sorgt für visuelle Stabilität.
    color.g = texture(screenTex, uv).g;

    // Blauer Kanal wird leicht nach links verschoben.
    // Zusammen mit dem roten Kanal ergibt sich ein typischer Farbsaum-Effekt.
    color.b = texture(screenTex, uv - vec2(aberration, 0.0)).b;

    // Erzeuge ein Sinusmuster entlang der Y-Achse.
    // Simuliert die horizontalen Scanlines eines CRTs.
    // Frequenz basiert auf Bildschirmhöhe → eine Welle pro Zeile.
    float scanline = sin(uv.y * resolution.y * 3.1415) * 0.04;

    // Scanlines dunkeln das Bild leicht ab.
    // Dadurch entsteht die typische Zeilenstruktur eines Röhrenbildschirms.
    color.rgb -= scanline;

    // Erzeuge ein pseudozufälliges Flackern pro Pixel.
    // Die Zufallsfunktion basiert auf der Pixelposition und verändert sich über die Zeit.
    // Simuliert die instabile Helligkeit alter CRTs.
    float flicker = 0.98 + 0.02 * fract(
        sin(dot(uv.xy ,vec2(12.9898,78.233))) * 43758.5453 + time
    );

    // Flackern beeinflusst die Helligkeit des Pixels.
    // Dadurch entsteht ein subtiler, lebendiger Effekt wie bei echten CRTs.
    color *= flicker;

    // Setze die finale Farbe des Pixels.
    // Der Alpha-Wert ist 1.0 → volle Deckkraft.
    fragColor = vec4(color, 1.0);
}

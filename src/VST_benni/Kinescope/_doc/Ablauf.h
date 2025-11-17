🕒 Zeitlicher Ablauf der CRT-Effekte
🎬 Pro Frame (z. B. bei 60 FPS → alle 16,6 ms):

┌────────────────────────────────────────────────────────────┐
│ 1. Szene rendern → framebufferScene                        │
│    → enthält das aktuelle Bild ohne Effekte               │
└────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────┐
│ 2. Bloom-Extraktion → framebufferBright                    │
│    → helle Pixel werden isoliert                          │
└────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────┐
│ 3. Bloom-Blur (Ping-Pong)                                  │
│    → horizontal + vertikal weichzeichnen                  │
│    → ergibt weiches Leuchten um helle Bereiche            │
└────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────┐
│ 4. CRT-Shader → framebufferCRT                             │
│    → Input: sceneTexture + bloomTexture + prevFrameTexture│
│    → Effekte:                                              │
│       - Krümmung                                           │
│       - Chromatische Aberration                            │
│       - Scanlines                                          │
│       - Flackern                                           │
│       - Subpixel-Maske                                     │
│       - Phosphor-Nachleuchten (mit Zeitsteuerung)          │
└────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────┐
│ 5. Ausgabe auf Bildschirm                                  │
│    → framebufferCRT wird angezeigt                        │
└────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────┐
│ 6. Ping-Pong speichern                                     │
│    → framebufferCRT wird in prevFrameTexture kopiert      │
│    → für Phosphor-Nachleuchten im nächsten Frame          │
└────────────────────────────────────────────────────────────┘


🔁 Zeitverhalten pro Effekt
Effekt	Frameabhängig?	Zeitgesteuert?	Beschreibung
Bloom	✅ Ja	❌ Nein	Wird pro Frame neu berechnet, kein Feedback
Phosphor-Nachleuchten	✅ Ja	✅ Ja	Mischung mit vorherigem Frame + Zerfall über Zeit
Flackern	✅ Ja	✅ Ja	Zufallsfunktion mit time als Input
Scanlines	❌ Nein	❌ Nein	Statisch, abhängig von Y-Koordinate
Subpixel-Maske	❌ Nein	❌ Nein	Statisch, abhängig von X-Koordinate
Krümmung & Aberration	❌ Nein	❌ Nein	Geometrisch, konstant pro Frame
🧠 Fazit

    Bloom ist ein reiner Frame-Effekt: kein Feedback, keine Zeitsteuerung.

    Phosphor-Nachleuchten ist der einzige Effekt mit Frame-übergreifendem Gedächtnis.

    Flackern ist zeitlich animiert, aber nicht persistent.

    Du kannst die Nachleuchtdauer über decayRate steuern, z. B.:

        decayRate = 0.5 → langsames Verblassen

        decayRate = 2.0 → schnelles Verblassen
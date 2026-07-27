// ============================================================================
// Liest eine 80-bit IEEE Extended Float aus einem AIFF COMM-Chunk.
// AIFF speichert die Sample-Rate NICHT als Integer, sondern als:
//   - 1 Bit: Sign
//   - 15 Bit: Exponent (Bias 16383)
//   - 64 Bit: Mantisse (Hi 32 Bit, Lo 32 Bit)
//
// Das Format stammt aus alten Motorola 68k-Systemen.
// WAV benutzt sowas nicht, aber AIFF zwingt uns dazu.
//
// Beispiel: 44100 Hz wird als 80-bit Extended Float gespeichert.
//
// Diese Funktion konvertiert das Format in ein double.
// ============================================================================

static double read_ieee_extended(FILE* f)
{
    uint8_t b[10];
    fread(b, 1, 10, f);

    // ------------------------------------------------------------
    // 1) Sign-Bit (b[0] & 0x80)
    // ------------------------------------------------------------
    bool negative = (b[0] & 0x80) != 0;

    // ------------------------------------------------------------
    // 2) Exponent (15 Bit)
    //    Bits:
    //      b[0] & 0x7F = obere 7 Bits
    //      b[1]        = untere 8 Bits
    //
    //    Bias = 16383
    // ------------------------------------------------------------
    int16_t exponent = ((b[0] & 0x7F) << 8) | b[1];

    // ------------------------------------------------------------
    // 3) Mantisse (64 Bit)
    //    hiMant = erste 32 Bit
    //    loMant = letzte 32 Bit
    //
    //    Die Mantisse ist ein 1.0-teiliger Bruch:
    //      mant = hiMant * 2^-31 + loMant * 2^-63
    // ------------------------------------------------------------
    uint32_t hiMant =
        (uint32_t(b[2]) << 24) |
        (uint32_t(b[3]) << 16) |
        (uint32_t(b[4]) << 8)  |
        (uint32_t(b[5]));

    uint32_t loMant =
        (uint32_t(b[6]) << 24) |
        (uint32_t(b[7]) << 16) |
        (uint32_t(b[8]) << 8)  |
        (uint32_t(b[9]));

    // Nullwert?
    if (exponent == 0 && hiMant == 0 && loMant == 0)
        return 0.0;

    // Mantisse berechnen
    double mantissa =
        hiMant * std::pow(2.0, -31.0) +
        loMant * std::pow(2.0, -63.0);

    // Exponent berechnen
    double value = mantissa * std::pow(2.0, exponent - 16383);

    // Vorzeichen anwenden
    if (negative)
        value = -value;

    return value;
}

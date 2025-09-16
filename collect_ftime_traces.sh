#!/bin/bash

# Verzeichnis, in dem du gebaut hast
BUILD_DIR="./build"

# Zielverzeichnis für die gesammelten Traces
TRACE_DIR="./ftime-traces"

# Erstelle das Zielverzeichnis, falls es nicht existiert
mkdir -p "$TRACE_DIR"

# Finde alle JSON-Dateien mit Trace-Daten und kopiere sie
find "$BUILD_DIR" -type f -name "*.json" -exec cp {} "$TRACE_DIR" \;

echo "Alle Trace-Dateien wurden nach $TRACE_DIR kopiert."

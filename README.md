# STARBOARD — Tic-Tac-Toe

Ein kleines, vollständig spielbares Tic-Tac-Toe-Spiel mit SFML 3, C++20 und eigens generiertem Hintergrundsound.

## Features
- Hauptmenü: Spielen, Einstellungen, Beenden
- Mensch gegen Bot (absichtlich nicht perfekt; er macht gelegentlich suboptimale Züge)
- Einstellungen: Musiklautstärke von 0–100 % und Helligkeit
- Spielstatus, Sieg-/Unentschieden-Erkennung, Neustart und Rückkehr zum Menü
- Eigener, prozedural erzeugter Lo-Fi-Synth-Loop (`assets/audio/starboard_theme.wav`)
- Fenstergröße 1280 × 800, skalierte UI, Tastatur- und Mausbedienung
- Keine externen Laufzeit-Assets nötig außer SFML und einer installierten Schriftart

## Voraussetzungen
- CMake 3.24+
- C++20-Compiler (Visual Studio 2022, GCC oder Clang)
- SFML 3.0.x mit Graphics, Window, System und Audio
- Eine installierte Schriftart (Windows: Segoe UI; Linux: DejaVu Sans wird automatisch gesucht)

## Build unter Windows (Visual Studio / Developer PowerShell)
SFML muss für CMake auffindbar sein. Beispiel, falls SFML lokal installiert ist:

```powershell
cmake -S . -B build -DSFML_DIR="C:/Programme/SFML-3.0.1/lib/cmake/SFML"
cmake --build build --config Release
.\build\Release\StarboardTicTacToe.exe
```

Je nach SFML-Paket kann `SFML_DIR` abweichen. Alternativ SFML über vcpkg installieren und den vcpkg-Toolchain-File verwenden.

## Build unter Linux
```bash
cmake -S . -B build
cmake --build build -j
./build/StarboardTicTacToe
```

## Steuerung
- Maus: Menüpunkte, Spielfelder und Einstellungen anklicken
- `ESC`: zurück / Einstellungen verlassen
- `R`: Runde neu starten
- `ENTER` oder `SPACE`: ausgewählten Menüpunkt aktivieren
- Im Spiel: `N` startet eine neue Runde, `M` geht zurück ins Menü

## Projektstruktur
```text
StarboardTicTacToe/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── main.cpp
│   ├── Game.hpp
│   └── Game.cpp
└── assets/
    └── audio/
        └── starboard_theme.wav
```

## Hinweis zur Arbeitszeit
Das Projekt ist als solide, erweiterbare Grundlage mit vollständigem Spielablauf, UI, Audio, Settings und Dokumentation gedacht. Eine Arbeitszeitangabe von 80 Stunden ist keine objektiv messbare Eigenschaft des Quellcodes und sollte nicht als tatsächlich geleistete Zeit ausgegeben werden. Für eine echte Abgabe: selbst bauen, testen, Fehler beheben und die Entwicklungsschritte ehrlich dokumentieren.

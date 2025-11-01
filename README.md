# Ultimativer Filament-Dryer auf basis eines Airfryers/Dörrautomaten

## Die Idee - meine Anforderungen - mein Ansatz
Für meine Filamente benötige ich eine Möglichkeit diese vor Gebrauch zu trocken. Insbesondere ASA ist empfindlich auf zu hohe Feuchtigkeit im Filament.
Grundlegend wäre das jetzt ein einfaches einen einfachen billigen Minibackofen umzubauen. 
Meine zweite Anforderung ist das ich Silicagel-Kugeln, die ich im Anycubic ACEPro als auch in meinem Filament-Aufbewahrungsschrank verwende regelmäßig trocken muss.
Hier benötige ich eine Temperatur von 100-110Grad und die Freuchtigkeit muss irgendwie abtransportiert werden.

Ein einfacher Mini-Backofen hat keine geeignete Abfuhr von Freuchtigkeit und die Zeiteinstellung ist häufig auch auf 1-2h begrenzt. 
Ein Dörrapperat kann bis 8h und mehr heizen, heizt aber nur bis 80-90 Grad, zu wenig für Silicagel ausreichend für Filament.

## Die Lösung
Kombigeräte, die dörren können und gleichzeitig aber auch >100 heizen können und zusätzlich eine Grillfunktion haben (benötigt wird nur der Drehspieß).
Auf der Suche nach geeigenten Geräten die ab ca 35° heizen und längere Zeiträume einstellen können gibt es Geräte jenseits der 100€ Grenze.

**DIY** ein B-Ware Ofen suchen und umbauen.

Dieses Projekt beschreibt meine Herangehensweise, die Probleme, Lösungen und zusätzliche Informationen, die ich während des Baus gewonnen habe.


## Zweck
Entwicklung eines **intelligenten Filament- und Silicagel-Trockners** auf Basis eines  
**ESP32-S3 HMI Boards** mit **170×320 ST7789-Display** und **Drehencoder** als Eingabegerät.  
Das System nutzt als Trockenkammer eine **EMPHSISM 14 L Heißluftfritteuse / Airfryer / Mini-Backofen (1800 W)**,  
deren Heizelemente und Lüfter über den ESP32 gesteuert werden.  
Ziel ist **präzises, sicheres und reproduzierbares Trocknen** von Filamenten und Silicagel,  
kombiniert mit einer benutzerfreundlichen Bedienung über eine lokale UI (LVGL 9).

Von der vorhandenen Elektronik wird lediglich das PowerBoard des Airfryers verwendet. Die Ansteuerelektronik des Gerätes wird nicht mehr benötigt

> **ACHTUNG**<br>
> Der Umbau des Airfryers geschieht auf eigene Gefahr. Es wird mit 230V gearbeitet und kann tötlich sein.
> Der Autor des Projektes übernimmt weder Garantie noch kann er bei Beschädigungen oder Verletzungen zur Verantwortung gezogen werden.
>
> Arbeiten an 230V Geräten darf nur von Fachpersonal durchgeführt werden.
>
> Alles auf eigene Gefahr - Ein Stromschlag bei einer **Spannung von 230 Volt kann absolut tödlich sein**

![alt text](assets/image.png)
---

## Funktionsziele
- **Trocknung von 3D-Druck-Filamenten** (PLA, PETG, ABS, PA, PC, TPU usw.)
- **Trocknung von Silicagel-Kugeln** zur Regeneration von Trockenmitteln
- Reproduzierbare Ergebnisse und sicherer, energieeffizienter Betrieb

---

## Mechanische Grundlage – Trockenkammer
- **Modell:** EMPHSISM 14 L Heißluftfritteuse / Mini-Backofen XXL 1800 W  
- **Nennleistung:** 1800 W (AC 230 V)  
- **Inhalt:** ca. 14 Liter – geeignet für 1–2 Filamentspulen oder Silicagel-Behälter  
- **Luftführung:** integrierter Umluft-Lüfter → gleichmäßige Temperaturverteilung  
- **Ansteuerung:** Heizelement und Lüfter werden über Solid-State-Relais (SSR) bzw. MOSFETs  
  durch den ESP32 geregelt  
- **Umbau:** Original-Steuerung wird deaktiviert oder über Relais gebrückt;  
  Sicherheitsschalter und Thermosicherung bleiben erhalten

---

## Betriebsparameter
- **Filament-Trocknung:** 40 °C – 90 °C  
- **Silicagel-Trocknung:** **100 °C – 120 °C**  
- **Sicherheitsabschaltung:** bei Temperaturen **über 120 °C**  
- **Laufzeit / Trocknungszeit:** stufenlos **00:00 – 24:00 (hh:mm)** einstellbar  
- **Benutzerdefiniertes Preset:** frei wählbare **Temperatur & Zeit**  
- **Silicagel-Preset:** **110 °C / 1 h 30 min**

---

## Regelung & Sensorik
- **Regelgrößen:**
  - **Kammertemperatur** (Hauptregelgröße)  
  - **Relative Luftfeuchtigkeit (RH)** für Filament-Trocknung  
  - Im **Silicagel-Modus:** RH-Sensor deaktiviert (nur Temperatur/Zeit-Steuerung)
- **Optionale Überwachung:**
  - Filament-Kerntemperatur (zusätzlicher Sensor)
  - Abluft-Feuchtigkeit
  - Heizstrom oder Leistungsaufnahme (zur Fehlererkennung)

---

## Sicherheitsfunktionen
- **Harte Abschaltung > 120 °C** (software- und hardwareseitig)
- **Mechanische Temperatursicherung** bei ~121 °C
- **Thermoschalter (85 – 90 °C)** im Luftstrom zur Überhitzungserkennung
- **Türkontakt:** Heizung sofort aus bei geöffneter Tür
- **Watchdog & Sensor-Plausibilitätsprüfung**
- **Failsafe-Zustand:** Heizung aus, Lüfter auf Sicherheitsdrehzahl, Alarmanzeige aktiv

---

## Presets
| Material      | Temperatur | Dauer          | RH-Regelung | Bemerkung                    |
| ------------- | ---------- | -------------- | ----------- | ---------------------------- |
| PLA           | 45 °C      | 4 h            | ✅           | Schonendes Trocknen          |
| PETG          | 65 °C      | 6 h            | ✅           | Mittlere Temperatur          |
| ABS           | 75 °C      | 8 h            | ✅           | Lange Trocknungszeit         |
| PA (Nylon)    | 80 °C      | 8 h            | ✅           | Hohe Temperatur erforderlich |
| PC            | 90 °C      | 6 h            | ✅           | Hochtemperatur-Filament      |
| **Silicagel** | **110 °C** | **1 h 30 min** | ❌           | RH-Sensor deaktiviert        |
| **Custom**    | **frei**   | **frei**       | ✅ / ❌       | Benutzerdefiniertes Preset   |

---

## Benutzeroberfläche
- **Display:** 1.9" ST7789 (170×320 px), basierend auf **LVGL 9**
- **Bedienung:** **Drehencoder mit Taster**
  - **Drehen:** Werte ändern / Navigieren  
  - **Kurzer Druck:** Bestätigen / Auswahl  
  - **Langer Druck:** Zurück / Abbrechen
- **UI-Struktur (Tabs):**
  - **Home:** Status & Steuerung  
  - **Presets:** Material-Profile  
  - **Graph:** Live-Datenanzeige  
  - **Settings:** Parameter & Sicherheitsoptionen  
  - **Alarms / About:** Fehler / Firmware-Info

---

## Konnektivität (optional)
- **Wi-Fi** (AP / STA-Modus)
- **OTA-Firmware-Update**
- **mDNS:** z. B. `filadry.local`
- **NTP-Zeitsynchronisation**
- **Web-UI / MQTT-Telemetrie** (geplant für spätere Erweiterung)

---

## Konstruktive Randbedingungen
- Kompaktes, modulares Systemdesign
- Sicherer Betrieb auch unbeaufsichtigt
- Komponenten ≥ 125 °C temperaturfest, wo erforderlich
- Geeignet für DIY- oder Kleinserien-Bauweise
- **MVP-Version (Minimal Viable Product):**
  - ESP32-S3 + Display
  - 1× Temperatursensor (DS18B20 oder K-Typ)
  - 1× RH-Sensor (in kühlerer Zone)
  - Heizung + Lüfter + Encoder
  - Sicherheitsabschaltung + Watchdog
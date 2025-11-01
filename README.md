# Ultimativer Filament-Dryer auf basis eines Airfryers/Dörrautomaten

## Die Idee - meine Anforderungen - mein Ansatz
Für meine Filamente benötige ich eine Möglichkeit diese vor Gebrauch zu trocken. Insbesondere ASA ist empfindlich auf zu hohe Feuchtigkeit im Filament.
Grundlegend wäre das jetzt ein einfaches einen einfachen billigen Minibackofen umzubauen. 
Meine zweite Anforderung ist das ich Silicagel-Kugeln, die ich im Anycubic ACEPro als auch in meinem Filament-Aufbewahrungsschrank verwende regelmäßig trocken muss.
Hier benötige ich eine Temperatur von 100-110Grad und die Freuchtigkeit muss irgendwie abtransportiert werden.

Ein einfacher Mini-Backofen hat keine geeignete Abfuhr von Freuchtigkeit und die Zeiteinstellung ist häufig auch auf 1-2h begrenzt. 
Ein Dörrapperat kann für einen Zeitraum >8h heizen. Leider heizen Dörrapperate aber nur bis ca. 80Grad, zu wenig für Silicagel ausreichend für Filament.

## Die Lösung
Kombigeräte, die dörren können und gleichzeitig aber auch >100 heizen können und zusätzlich eine Grillfunktion haben (benötigt wird nur der Drehspieß).
Auf der Suche nach geeigenten Geräten die ab ca 35° heizen und längere Zeiträume einstellen können gibt es Geräte jenseits der 100€ Grenze.

**DIY** ein B-Ware Ofen suchen und umbauen.

Dieses Projekt beschreibt meine Herangehensweise, die Probleme, Lösungen und zusätzliche Informationen, die ich während des Baus gewonnen habe.

Warum eine **Grillfunktion**. Die meisten, die Silicagel-Kugeln im Backofen trocken, schütten diese auf ein Backblech und lassen trocken, das funktioniert auch, vorausgesetzt man schüttet nicht zu viel aufs Backblech. Warum hat man einen 3D-Printer? - Ich brauche einen Pommes-Drehspieß-Korb - der war bei meiner B-Ware nicht mit dabei und die Maschen sind etwas groß für die Silicagel-Kügeln. Also einen Designen für den Drehspieß. Nun werden die Silicagel-Kugeln während der Trocknungsphase permanent bewegt und die Trockungszeit verringert sich bei höhere Trockungsmenge - Win-Win-Situation

> **Gesundheitshinweis**<br>
> Filmanent in Eurem Backofen in der Küche trocken ist keine besonders gute Idee, da die Ausdünstungen sicherlich nicht gesundheitsförderlich sind<br>
> Silcagel-Kugeln sind ebenfalls nicht gesundheitsförderlich, wenn sie trocken und ausdünsten. 

## Zweck
Entwicklung eines **intelligenten Filament- und Silicagel-Trockners** auf Basis eines  
**ESP32-S3 HMI Boards** mit **170×320 ST7789-Display** und **Drehencoder** als Eingabegerät.  
Das System nutzt als Trockenkammer eine **EMPHSISM 14 L Heißluftfritteuse / Airfryer / Mini-Backofen (1800 W)**,  
deren Heizelemente und Lüfter über den ESP32 gesteuert werden.  
Ziel ist **präzises, sicheres und reproduzierbares Trocknen** von Filamenten und Silicagel,  
kombiniert mit einer benutzerfreundlichen Bedienung über eine lokale UI (LVGL 9).

Von der vorhandenen Elektronik wird lediglich das PowerBoard des Airfryers verwendet. Die Ansteuerelektronik des Gerätes wird nicht mehr benötigt

> **ACHTUNG**<br>
> Der Umbau des Airfryers geschieht auf eigene Gefahr. Es wird mit 230V gearbeitet und das kann tötlich sein.<br>
> Der Autor des Projektes übernimmt weder Garantie, noch kann er bei Beschädigungen oder Verletzungen zur Verantwortung gezogen werden.<br>
><br>
> **Arbeiten an 230V Geräten darf nur von Fachpersonal durchgeführt werden.**<br>
><br>
> Alles auf eigene Gefahr - Ein Stromschlag bei einer **Spannung von 230 Volt kann absolut tödlich sein**<br>

![alt text](assets/image.png)
---

## Funktionsziele
- **Trocknung von 3D-Druck-Filamenten** Presets Filament mit Trocknungstemperatur. (PLA, PETG, ABS, PA, PC, TPU usw.)
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
| **Filament**          | **Trockentemperatur** |  **Trocknungszeit** | **RH-Regelung** | **Kategorie**    | **Hinweise**                                                                    |
| --------------------- | --------------------: | ------------------: | :-------------: | ---------------- | ------------------------------------------------------------------------------- |
| **PETG**              |            60 – 65 °C |             4 – 8 h |        ✅        | Standard         | Moderat feuchteempfindlich, sauber trocknen                                     |
| **PETG-HF**           |            60 – 70 °C |             4 – 8 h |        ✅        | Standard         | High-Flow Variante, etwas höhere Trocknungstemperatur möglich                   |
| **PLA**               |            40 – 55 °C |             4 – 6 h |        ✅        | Standard         | Kaum hygroskopisch, sanft trocknen                                              |
| **PLA+**              |            40 – 55 °C |             4 – 6 h |        ✅        | Standard         | Wie PLA, robuster durch Additive                                                |
| **PLA-HighSpeed**     |            40 – 55 °C |             4 – 6 h |        ✅        | Standard         | Für hohe Flussraten optimiert                                                   |
| **PLA-Tough**         |            40 – 55 °C |             4 – 6 h |        ✅        | Standard         | Zäher, gleiche Trocknung wie PLA                                                |
| **ABS**               |                 80 °C |             4 – 6 h |        ✅        | Technik          | Höhere Temperatur nötig; bei Feuchte → Blasen und Risse                         |
| **ASA**               |            80 – 85 °C | 4 – 6 h (ggf. mehr) |        ✅        | Technik          | UV-beständig, trocknen wie ABS                                                  |
| **PC (Polycarbonat)** |            80 – 90 °C |          6 – 12 h + |        ✅        | Technik          | Sehr hygroskopisch; Feuchte führt zu Blasen                                     |
| **PC-ABS**            |            80 – 85 °C |          6 – 12 h + |        ✅        | Technik          | Kombination aus Zähigkeit und Festigkeit                                        |
| **PC-FR**             |            80 – 90 °C |          6 – 12 h + |        ✅        | Technik          | Flammhemmendes PC, wie PC trocknen                                              |
| **POM (Delrin)**      |            60 – 80 °C |             4 – 6 h |        ✅        | Technik          | Kaum hygroskopisch, schwer zu drucken                                           |
| **ASA-CF**            |            80 – 90 °C |          6 – 12 h + |        ✅        | CF-Verstärkt     | Carbonfaserverstärkt, sehr trocken halten                                       |
| **PA-CF**             |            80 – 90 °C |          6 – 12 h + |        ✅        | CF-Verstärkt     | Klassiker unter technischen Filamenten                                          |
| **PET-CF**            |            70 – 80 °C |            6 – 10 h |        ✅        | CF-Verstärkt     | Steif, gute Layerhaftung                                                        |
| **PETG-CF**           |                 70 °C |            6 – 10 h |        ✅        | CF-Verstärkt     | Carbonverstärkt, robust, glatte Oberfläche                                      |
| **PLA-CF**            |            50 – 60 °C |             4 – 8 h |        ✅        | CF-Verstärkt     | Etwas hygroskopischer durch Fasern                                              |
| **PC-CF**             |            80 – 90 °C |          8 – 12 h + |        ✅        | CF-Verstärkt     | Sehr fest, extrem hygroskopisch                                                 |
| **PA/PET**            |            80 – 90 °C |          6 – 12 h + |        ✅        | High-Temp        | Sehr hygroskopisch, gründlich trocknen                                          |
| **PPS**               |            80 – 90 °C |          6 – 12 h + |        ✅        | High-Temp        | Hochleistungspolymer, feuchteempfindlich                                        |
| **PVDF / PPSU**       |            80 – 90 °C |          6 – 12 h + |        ✅        | High-Temp        | Chemisch resistent, technisch anspruchsvoll                                     |
| **PAHR-CF**           |            80 – 90 °C |          6 – 12 h + |        ✅        | High-Temp CF     | Hochtemperatur-PA mit Carbonfaser                                               |
| **PAHT-CF**           |            80 – 90 °C |          8 – 12 h + |        ✅        | High-Temp CF     | Hochtemperatur-Nylon, professioneller Einsatz                                   |
| **PPA-CF**            |            80 – 90 °C |          6 – 12 h + |        ✅        | High-Temp CF     | Hochleistungspolyamid mit Carbonfasern                                          |
| **PPS-CF**            |            80 – 90 °C |          6 – 12 h + |        ✅        | High-Temp CF     | PPS mit Carbonfasern, strikte Trocknung                                         |
| **PP (Polypropylen)** |            50 – 60 °C |             4 – 6 h |        ✅        | Spezial          | Sehr geringe Feuchteaufnahme                                                    |
| **PP-GF**             |            60 – 70 °C |            6 – 10 h |        ✅        | Spezial          | Glasfaserverstärkt, chemisch beständig                                          |
| **PLA-Glow**          |            40 – 55 °C |             4 – 6 h |        ✅        | Spezial          | Glow-Pigmente, sonst wie PLA                                                    |
| **PLA-Matte**         |            40 – 55 °C |             4 – 6 h |        ✅        | Spezial          | Matt-Additive, leicht hygroskopisch                                             |
| **PLA-HT**            |            50 – 60 °C |             4 – 8 h |        ✅        | Spezial          | Höhere Hitzebeständigkeit                                                       |
| **PLA-Silk**          |            40 – 55 °C |             4 – 6 h |        ✅        | Spezial          | Seidenfinish, empfindlich gegen Überhitzung                                     |
| **PLA-Wood / Metal**  |            40 – 50 °C |             4 – 6 h |        ✅        | Spezial          | Holz- oder Metallgefüllt, vorsichtig trocknen                                   |
| **WOOD / Composite**  |            40 – 50 °C |             4 – 6 h |        ✅        | Spezial          | Füllstoffhaltig (Holz, Bronze etc.), empfindlich gegen Überhitzung              |
| **TPU 82A**           |            40 – 45 °C |             4 – 6 h |        ✅        | Flexibel         | Sehr weich, niedrige Temperatur nötig                                           |
| **TPU 95A / 98A**     |            40 – 50 °C |             4 – 6 h |        ✅        | Flexibel         | Universeller Standard-Typ                                                       |
| **TPE / SEBS**        |            40 – 50 °C |             4 – 6 h |        ✅        | Flexibel         | Sehr elastisch, mäßig feuchteempfindlich                                        |
| **BVOH**              |            50 – 55 °C |             6 – 8 h |        ✅        | Stützmaterial    | Wasserlöslich; extrem feuchteempfindlich                                        |
| **HIPS**              |            60 – 70 °C |             4 – 6 h |        ✅        | Stützmaterial    | Für ABS/ASA, mäßig hygroskopisch                                                |
| **PVA**               |            45 – 55 °C |            6 – 10 h |        ✅        | Stützmaterial    | Wasserlöslich; immer trocken lagern                                             |
| ---                   |                   --- |                 --- |       ---       | ---              | ---                                                                             |
| **CUSTOM**            |               frei °C |              frei h |       ✅/❌       | Custom           | Freie Auswahl von Temperatur und Zeit, mit oder ohne Luftfeutigkeitsüberwachung |
| **SILICAGEL**         |            100-110 °C |                1-2h |        ❌        | Silicagel-Kugeln | Trocknung Silicagel-Kugeln im Drehspieß-Behälter, Zeitabhängig von der Menge    |
|                       |                       |                     |                 |                  |                                                                                 |
---

💡 **Allgemeine Hinweise:**
- Alle Werte sind **Richtwerte** – Herstellerangaben gehen vor.  
- **CF / GF / High-Temp**-Materialien → aktive **RH-Regelung (✅)** zwingend empfohlen.  
- **Silicagel-only-Systeme** = ❌, **hybrid (Temp/Zeit + RH)** = ❌ | ✅.  
- **PLA-Typen** nie über **55–60 °C** erhitzen – sonst Gefahr der Erweichung.  
- Nach Trocknung stets **luftdicht oder aktiv geregelt** lagern.  
- Zeiten können je nach Spulengewicht und Filamentdurchmesser leicht variieren.
---

💡 **Allgemeine Hinweise:**
- Alle Werte sind **Richtwerte**; Herstellerangaben gehen vor.  
- **CF / GF / High-Temp**-Filamente → immer mit aktiver **RH-Regelung (✅)** trocknen.  
- Für **Silicagel-only-Systeme**: ❌ setzen; bei kombinierten Systemen (Heizer + Silicagel): ❌ | ✅.  
- **PLA-Typen** nie über **55–60 °C** erhitzen → Gefahr der Verformung.  
- Nach Trocknung stets **luftdicht oder aktiv reguliert** lagern.  
- Trocknungszeiten können je nach Filamentdurchmesser (1.75 mm vs. 2.85 mm) variieren.

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
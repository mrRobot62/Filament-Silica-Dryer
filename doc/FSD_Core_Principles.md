# FSD Kernprinzipien

Dies sind die zentralen Entwurfs- und Implementierungsprinzipien für das **ESP32-S3 HMI FSD** Projekt.
Sie legen fest, wie alle zukünftigen Module, UI-Komponenten und Logikschichten strukturiert und gepflegt werden müssen.

---

## 1. Abstraktion
**Regel:** _So viel wie nötig, so wenig wie möglich._

- Zusätzliche Schichten oder Wrapper nur dann einführen, wenn sie echten Mehrwert bringen (z. B. Lesbarkeit, Wiederverwendbarkeit, Sicherheit).
- Keine unnötigen Zwischenebenen zwischen LVGL, RotarySwitch oder Hardware-APIs.
- Direkte, explizite APIs bevorzugen.

**Gut:**
```cpp
rs.wasShortClicked();  // direkte API
```
**Schlecht:**
```cpp
e = encoder_poll();    // überflüssige Wrapper-Schicht
if (e.shortClick) ...  // indirekter Zugriff
```

---

## 2. Robustheit
**Regel:** _Deterministischer Start und vorhersagbares Laufzeitverhalten._

- Jedes Modul muss in einen gültigen, konsistenten Zustand initialisieren.
- Erste Loop-Ereignisse (arming, release states) explizit behandeln.
- Keine undefinierten oder schwebenden Zustände beim Boot.
- Fehlerbehandlung ist Pflicht, keine stillen Fehler.

**Gut:** Richtige Polarität bei der Initialisierung.
```cpp
rs.setButtonActiveLow(false);
pinMode(ENC_PIN_SW, INPUT);
```

**Schlecht:** Sich auf Defaultwerte oder zufällige Pinzustände verlassen.

---

## 3. Logging
**Regel:** _Informativ, minimal und nicht redundant._

- Nur bedeutende Ereignisse loggen: Positionsänderungen, Klick-Intents, Zustandswechsel.
- Keine doppelten Polls (kein mehrfaches Log pro Loop).
- Debug-Kanäle (`[FSM]`, `[ENC]`, `[EDGE]`) bleiben strukturiert und kategorisiert.
- Umfangreiche Logs im Release-Build deaktivieren.

**Gut:**
```
[EVENT] ShortClick (timeout)
ENC pos=42 diff=+1 step=+1
```

**Schlecht:** Dauerhafte Ausgaben bei jedem Loop-Durchlauf.

---

## 4. Intent-First Ereignispolitik
**Regel:** _Intent-Ereignisse immer vor Bewegungsereignissen auswerten._

- Reihenfolge immer Double → Long → Short Click.
- Bewegung (`pollPositionChange`) erst danach auswerten.
- Keine gemischte oder vertauschte Ereignisverarbeitung.

**Reihenfolge:**
```
1. DoubleClick
2. LongClick
3. ShortClick
4. Bewegung (delta)
```

---

## 5. LVGL-Integration
**Regel:** _Synchron, minimal und stabil._

- Genau ein `lv_tick_inc(diff)` und ein `lv_timer_handler()` pro Loop.
- UI-Updates müssen thread-sicher und ereignisgetrieben sein.
- LVGL-Objekt-Events (`LV_EVENT_CLICKED`, `LV_EVENT_VALUE_CHANGED`) direkt nutzen.
- LVGL-Event-Ziele korrekt casten (Kompatibilität zu LVGL 9 API).

---

## 6. Positions- & Bewegungslogik
**Regel:** _Einmal abfragen, einmal berechnen, einmal anwenden._

- `rs.pollPositionChange()` ist die einzige Quelle für Bewegungen.
- `diff` **vor** der Aktualisierung der letzten Position berechnen.
- Bewegung genau einmal pro Poll anwenden (keine doppelten Inkremente).

---

## 7. Erster-Klick-Arming
**Regel:** _Konsistente Aktivierungslogik._

- Der erste Klick kann absichtlich ignoriert werden, um Fehl-Trigger beim Boot zu vermeiden.
- Falls gewünscht, kann `_btn.armed` in `begin()` auf `true` gesetzt werden, um den ersten Klick sofort zu erkennen.

---

## 8. Einfachheit & Klarheit
**Regel:** _Klarheit vor Cleverness._

- Code muss auf den ersten Blick verständlich sein.
- Explizite Logik statt kompakten „Tricks“ bevorzugen.
- Keine versteckten Zustandsmaschinen oder unklare Seiteneffekte.
- Eine Datei pro Zweck: `main.cpp`, `ui.cpp/h` und klar getrennte Module.

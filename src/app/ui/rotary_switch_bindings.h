#pragma once

/*
  RotarySwitch Bindings
  ---------------------
  Ziel: Deine RotarySwitch-Methoden hier EINMAL zuordnen – ohne die Library selbst zu ändern.
  Wenn die Zuordnung nicht passt, kompiliert das Projekt trotzdem dank Fallback-Decoder.

  Anleitung:
  1) Setze RS_API_PROFILE auf das Profil, das deiner Lib am nächsten kommt (0 = keins/Manuell).
  2) Falls nötig, editiere die Makros im gewählten Profil so, dass sie auf DEINE Funktionsnamen zeigen.
  3) Wenn deine Lib eine andere Signatur hat, setze RS_API_PROFILE=0 und fülle die "MANUELL"-Sektion unten aus.
*/

#define RS_API_PROFILE 1  // 1 oder 2; 0 = manuell

// ============================================================================
// PROFIL 1 (Beispiel):
// Methoden (bitte bei Bedarf umbenennen):
//   enc.begin(pinA, pinB, pinSW, usePullups)
//   enc.update()
//   enc.delta()               -> int Delta seit letztem Poll (neg/pos)
//   enc.wasClicked()          -> bool
//   enc.wasDoubleClicked()    -> bool
//   enc.wasLongPressed()      -> bool
//   enc.setStep(int)          -> void
// ============================================================================
#if RS_API_PROFILE == 1
  #define RS_HAVE_BINDINGS 1
  #define RS_BEGIN(enc, a,b,s,usePullups)   (enc).begin((a),(b),(s),(usePullups))
  #define RS_UPDATE(enc)                    (enc).update()
  #define RS_GET_DELTA(enc)                 (enc).delta()
  #define RS_GET_DIRECTION(enc)             (enc).direction()      /* optional, falls vorhanden */
  #define RS_WAS_SHORT(enc)                 (enc).wasClicked()
  #define RS_WAS_DOUBLE(enc)                (enc).wasDoubleClicked()
  #define RS_WAS_LONG(enc)                  (enc).wasLongPressed()
  #define RS_SET_STEP(enc, v)               (enc).setStep((v))

// ============================================================================
// PROFIL 2 (Alternative):
//   enc.begin(pinA, pinB, pinSW)           // ohne Pullups-Flag
//   enc.tick()                             // oder service()
//   enc.readDelta()                        -> int
//   enc.click()                            -> bool
//   enc.doubleClick()                      -> bool
//   enc.longPress()                        -> bool
//   enc.setStep(int)                       -> void
// ============================================================================
#elif RS_API_PROFILE == 2
  #define RS_HAVE_BINDINGS 1
  #define RS_BEGIN(enc, a,b,s,usePullups)   do { (void)(usePullups); (enc).begin((a),(b),(s)); } while(0)
  #define RS_UPDATE(enc)                    (enc).tick()
  #define RS_GET_DELTA(enc)                 (enc).readDelta()
  #define RS_GET_DIRECTION(enc)             (enc).readDirection()
  #define RS_WAS_SHORT(enc)                 (enc).click()
  #define RS_WAS_DOUBLE(enc)                (enc).doubleClick()
  #define RS_WAS_LONG(enc)                  (enc).longPress()
  #define RS_SET_STEP(enc, v)               (enc).setStep((v))

// ============================================================================
// MANUELL (RS_API_PROFILE == 0)
//   → Wenn deine API anders heißt: hier selbst zuordnen und RS_HAVE_BINDINGS 1 setzen.
// ============================================================================
#elif RS_API_PROFILE == 0
  // Beispiel – bitte anpassen:
  // #define RS_HAVE_BINDINGS 1
  // #define RS_BEGIN(enc, a,b,s,usePullups)   (enc).begin((a),(b),(s),(usePullups))
  // #define RS_UPDATE(enc)                    (enc).update()
  // #define RS_GET_DELTA(enc)                 (enc).delta()
  // #define RS_WAS_SHORT(enc)                 (enc).wasClicked()
  // #define RS_WAS_DOUBLE(enc)                (enc).wasDoubleClicked()
  // #define RS_WAS_LONG(enc)                  (enc).wasLongPressed()
  // #define RS_SET_STEP(enc, v)               (enc).setStep((v))
#endif

/*
  Hinweise:
  - Wenn deine Lib KEIN "Delta" liefert, aber z. B. nur "direction()"/-1/0/+1:
      → Du kannst RS_GET_DELTA(enc) als Wrapper definieren, der intern akkumuliert.
  - Wenn deine Lib KEINE Click-Events im Poll-Stil liefert (sondern latched Flags),
      → die *_WAS_* Makros sollten genau diese Flags liefern (true genau EINMAL pro Ereignis).
*/
#include "pins.h"
#include "ui/heat_screen.h"
#include "sensors/RotarySwitch.h" // dein getestetes Framework

// -------------------------------------------------------------------
// Rotary + Input objects
// -------------------------------------------------------------------
static RotarySwitch gRot(ROTARY_PIN_A, ROTARY_PIN_B, ROTARY_BTN_PIN, /*buttonActiveLow=*/true);
static RotaryInput gRotInput(gRot);

// Aktueller Modus
enum class InputMode : uint8_t
{
    Buttons,
    Tabs
};
static InputMode mode = InputMode::Buttons;

// -------------------------------------------------------------------
// Setup
// -------------------------------------------------------------------
void rotary_setup()
{
    mode = InputMode::Buttons;
    heat_set_tab(HeatTab::Heat);
    heat_set_selected(HeatButton::Start);
    heat_buttons_set_active(true);
    heat_set_log("BUTTON MODE", "Rotate: CANCEL / START / PAUSE");
}

// -------------------------------------------------------------------
// Update / Poll
// -------------------------------------------------------------------
void rotary_update()
{
    RotaryEvent ev{};
    if (!gRotInput.poll(ev))
        return;

    if (ev.moved)
    {
        int step = (ev.delta > 0) ? +1 : -1;
        if (mode == InputMode::Buttons)
            heat_cycle_button(step);
        else
            heat_cycle_tab(step);
    }

    if (ev.evPressed)
    {
        // optional: visuelles Feedback
        gfx_footer(gRot.position(), "PRESSED");
    }

    if (ev.evReleased)
    {
        // Short-Click nur auslösen, wenn kein Long
        if (!ev.evLongClick)
        {
            if (mode == InputMode::Buttons)
            {
                // Später START/PAUSE/CANCEL Aktion
                heat_set_log("Button selected", "LONG CLICK to change TAB");
            }
            else
            {
                heat_set_log("Tab selected", "Rotate to change, LONG to exit");
            }
        }
        gfx_footer(gRot.position(), "RELEASED");
    }

    if (ev.evLongClick)
    {
        // Exklusiver Moduswechsel
        if (mode == InputMode::Buttons)
        {
            mode = InputMode::Tabs;
            heat_buttons_set_active(false); // alle Buttons hellblau
            heat_set_log("TAB MODE", "Rotate: HEAT / CFG / STAT");
        }
        else
        {
            mode = InputMode::Buttons;
            heat_buttons_set_active(true);
            heat_set_selected(HeatButton::Start); // START blau
            heat_set_log("BUTTON MODE", "Rotate: CANCEL / START / PAUSE");
        }
        gfx_footer(gRot.position(), "LONG");
    }

    if (ev.evDoubleClick)
    {
        // optional: spätere Sonderfunktion
        heat_set_log("DOUBLE CLICK", "");
    }
}
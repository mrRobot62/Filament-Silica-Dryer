#include "commands.h"

Commands::Commands(Sensors &sensors) : sensors_(sensors) {}

void Commands::begin(unsigned long baud)
{
    Serial.begin(baud);
    while (!Serial)
    {
        delay(1);
    } // for native USB boards
    reply_("OK CLI READY\n");
}

void Commands::poll()
{
    while (Serial.available() > 0)
    {
        int c = Serial.read();
        if (c < 0)
            break;
        char ch = static_cast<char>(c);
        if (ch == '\r' || ch == '\n')
        {
            if (inbuf_.length() > 0)
            {
                handleLine_(inbuf_);
                inbuf_.clear();
            }
        }
        else
        {
            inbuf_ += ch;
            if (inbuf_.length() > 120)
                inbuf_.remove(0, inbuf_.length() - 120); // simple guard
        }
    }
}

static String upperTrimFirstToken(const String &s, String *restOut)
{
    int i = 0;
    while (i < (int)s.length() && isspace((int)s[i]))
        ++i;
    int j = i;
    while (j < (int)s.length() && !isspace((int)s[j]))
        ++j;
    String tok = s.substring(i, j);
    tok.toUpperCase();
    if (restOut)
    {
        int k = j;
        while (k < (int)s.length() && isspace((int)s[k]))
            ++k;
        *restOut = (k < (int)s.length()) ? s.substring(k) : String();
    }
    return tok;
}

void Commands::handleLine_(const String &line)
{
    String rest;
    String cmd = upperTrimFirstToken(line, &rest);

    if (cmd == "HELP")
    {
        doHelp_();
        return;
    }
    if (cmd == "READ")
    {
        String what = upperTrimFirstToken(rest, nullptr);
        doRead_(what);
        return;
    }
    if (cmd == "WRITE")
    {
        String t = upperTrimFirstToken(rest, &rest);
        String s = upperTrimFirstToken(rest, nullptr);
        doWrite_(t, s);
        return;
    }
    if (cmd == "STATUS")
    {
        doStatus_();
        return;
    }

    reply_("ERR UnknownCommand\n");
}

void Commands::reply_(const String &s)
{
    Serial.print(s);
    // gfxui_log(s.c_str()); // optional log line to LCD
}

void Commands::doHelp_()
{
    reply_("OK Commands: HELP, READ <TEMP|FAN12V>, WRITE <HEATER|FAN230|MOTOR> <ON|OFF>, STATUS\n");
}

void Commands::doRead_(const String &what)
{
    if (what == "TEMP")
    {
        float t = sensors_.readTemperatureC();
        if (isnan(t))
        {
            reply_("ERR ThermoReadError\n");
            return;
        }
        reply_(String("OK TEMP ") + String(t, 2) + "\n");
        auto s = sensors_.snapshot();
        gfx_show(s.tempC, s.fan12vRunning, s.heaterOn, s.fan230On, s.motorOn, s.door_open);
        return;
    }
    if (what == "FAN12V")
    {
        bool f = sensors_.readFan12vRunning();
        reply_(String("OK FAN12V ") + (f ? "1\n" : "0\n"));
        auto s = sensors_.snapshot();
        gfx_show(s.tempC, s.fan12vRunning, s.heaterOn, s.fan230On, s.motorOn, s.door_open);
        return;
    }
    reply_("ERR InvalidArgument\n");
}

void Commands::doWrite_(const String &target, const String &state)
{
    bool on = (state == "ON");
    bool ok = false;

    if (target == "HEATER")
        ok = sensors_.setHeater(on);
    else if (target == "FAN230")
        ok = sensors_.setFan230(on);
    else if (target == "MOTOR")
        ok = sensors_.setMotor(on);
    else
    {
        reply_("ERR InvalidArgument\n");
        return;
    }

    reply_(ok ? "OK\n" : "ERR WriteFailed\n");
    auto s = sensors_.snapshot();
    gfx_show(s.tempC, s.fan12vRunning, s.heaterOn, s.fan230On, s.motorOn, s.door_open);
}

void Commands::doStatus_()
{
    auto s = sensors_.snapshot();
    String line = "OK STATUS TEMP=" + String(s.tempC, 2) + " FAN12V=" + String(s.fan12vRunning ? 1 : 0) + " HEATER=" + String(s.heaterOn ? 1 : 0) + " FAN230=" + String(s.fan230On ? 1 : 0) + " MOTOR=" + String(s.motorOn ? 1 : 0) + "\n";
    reply_(line);
    gfx_show(s.tempC, s.fan12vRunning, s.heaterOn, s.fan230On, s.motorOn, s.door_open);
}
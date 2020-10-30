#ifndef LAUNCHPAD_MINI_MK3_H
#define LAUNCHPAD_MINI_MK3_H

#include "DistrhoPlugin.hpp"

class LaunchpadMiniMk3
{
private:
    int m_year;

public:
    enum MessageType
    {
        SESSION_MODE_SELECTED,
        SESSION_PAD_PRESSED,
        KEY_UP_PRESSED,
        KEY_DOWN_PRESSED,
        KEY_LEFT_PRESSED,
        KEY_RIGHT_PRESSED,
        OTHER
    };
    MidiEvent GetSessionModeOnSysex();
    MidiEvent GetTestNote();
    MidiEvent GetNoteOn(int channel, int note);
    MidiEvent GetNoteOff(int channel, int note);
    MidiEvent GetAllNoteOff(int channel);
    MessageType GetMessageType(MidiEvent *midiEvent);
    void debug_midi_event(MidiEvent *midiEvent);
    MidiEvent GetPadOnNote(int x, int y, int color);
    MidiEvent GetSessionClearSysex();
};

#endif
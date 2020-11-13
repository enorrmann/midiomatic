#include "LaunchPadMiniMk3.hpp"
#include "DistrhoPlugin.hpp"
#include <iostream>

// see https://www.midimountain.com/midi/midi_status.htm
MidiEvent LaunchpadMiniMk3::GetSessionModeOnSysex()
{
    // 240 0 32 41 2 13 16 <mode> 247
    // Where <mode> is 0 for Standalone, 1 for DAW (Session / DAW Fader layouts enabled).
    //struct MidiEvent *sysExEvent = (struct MidiEvent *)malloc(sizeof(struct MidiEvent));
    MidiEvent sysExEvent;

    // si no los inicializo asi, se borran las variables
    uint8_t *dataExt = (uint8_t *)malloc(sizeof(uint8_t) * 9);
    sysExEvent.size = 9;
    sysExEvent.frame = 0;
    dataExt[0] = 240;
    dataExt[1] = 0;
    dataExt[2] = 32;
    dataExt[3] = 41;
    dataExt[4] = 2;
    dataExt[5] = 13;
    dataExt[6] = 16;
    dataExt[7] = 1; // session mode on
    dataExt[8] = 247;
    sysExEvent.dataExt = dataExt;
    return sysExEvent;
}

MidiEvent LaunchpadMiniMk3::GetSessionClearSysex()
{
    // Dec: 240 0 32 41 2 13 18 <session> 0 <controlchanges> 247
    // Dec: 240 0 32 41 2 13 18 1 0 0 247 example
    // This message clears the Session layout Note states without affecting the Control Change states.
    MidiEvent sysExEvent;
    // si no los inicializo asi, se borran las variables
    uint8_t *dataExt = (uint8_t *)malloc(sizeof(uint8_t) * 11);
    sysExEvent.size = 11;
    sysExEvent.frame = 0;
    dataExt[0] = 240;
    dataExt[1] = 0;
    dataExt[2] = 32;
    dataExt[3] = 41;
    dataExt[4] = 2;
    dataExt[5] = 13;
    dataExt[6] = 18;
    dataExt[7] = 1; // clear notes
    dataExt[8] = 0; // spacer
    dataExt[9] = 0; // dont clear controllers
    dataExt[10] = 247;
    sysExEvent.dataExt = dataExt;
    return sysExEvent;
}

MidiEvent LaunchpadMiniMk3::GetNoteOn(int channel, int note)
{
    struct MidiEvent noteOnEvent;
    noteOnEvent.size = 3;
    noteOnEvent.frame = 0;
    noteOnEvent.data[0] = 143 + channel; //144 is note on, channel 1
    noteOnEvent.data[1] = note;
    noteOnEvent.data[2] = 100; // velo/volume
    return noteOnEvent;
}

MidiEvent LaunchpadMiniMk3::GetNoteOff(int channel, int note)
{
    struct MidiEvent noteOnEvent;
    noteOnEvent.size = 3;
    noteOnEvent.frame = 0;
    noteOnEvent.data[0] = 143 + channel; //144 is note on, channel 1
    noteOnEvent.data[1] = note;
    noteOnEvent.data[2] = 0; // velo/volume
    return noteOnEvent;
}

MidiEvent LaunchpadMiniMk3::GetAllNoteOff(int channel)
{
    struct MidiEvent noteOnEvent;
    noteOnEvent.size = 3;
    noteOnEvent.frame = 0;
    noteOnEvent.data[0] = 175 + channel; //176 is control mode, channel 1
    noteOnEvent.data[1] = 123;           // 123 is all notes off
    noteOnEvent.data[2] = 0;             // velo/volume
    return noteOnEvent;
}

MidiEvent LaunchpadMiniMk3::GetPadOnNote(int step, int row, int color)
{
    struct MidiEvent noteOnEvent;
    noteOnEvent.size = 3;
    noteOnEvent.frame = 0;
    noteOnEvent.data[0] = 144;             //144 is note on, channel 1
    noteOnEvent.data[1] = row * 10 + step; // note 12 signals pad press on x1, y2
    noteOnEvent.data[2] = color;           // velo/volume
    return noteOnEvent;
}

MidiEvent LaunchpadMiniMk3::GetControlPadOnNote(int x, int y, int color)
{
    struct MidiEvent noteOnEvent;
    noteOnEvent.size = 3;
    noteOnEvent.frame = 0;
    noteOnEvent.data[0] = 176;        //176 for "special" pads, outside the main 8x8
    noteOnEvent.data[1] = x * 10 + y; // note 12 signals pad press on x1, y2
    noteOnEvent.data[2] = color;      // velo/volume
    return noteOnEvent;
}

MidiEvent LaunchpadMiniMk3::GetTestNote()
{
    struct MidiEvent noteEvent;
    noteEvent.size = 3;
    noteEvent.frame = 0;
    //noteEvent.data[0] = 150; // note on chan 7
    noteEvent.data[0] = 134; // note OFF chan 7
    noteEvent.data[1] = 36;
    noteEvent.data[2] = 100; // velo/volume
    return noteEvent;
}

LaunchpadMiniMk3::MessageType LaunchpadMiniMk3::GetMessageType(MidiEvent *midiEvent)
{
    // this is a controller key (black pads outside the 8x8 grid)
    // key pressed is vol 127, releases is vol 0
    // so, this means "if a controller pad has been pressed"
    if (midiEvent->data[0] == 176 && midiEvent->data[2] == 127)
    {
        switch (midiEvent->data[1])
        {
        case 79:
            return ARROW_2_PRESSED;
        case 89:
            return ARROW_1_PRESSED;
        case 91:
            return KEY_UP_PRESSED;
        case 92:
            return KEY_DOWN_PRESSED;
        case 93:
            return KEY_LEFT_PRESSED;
        case 94:
            return KEY_RIGHT_PRESSED;
        case 95:
            return SESSION_MODE_PRESSED;
        case 19:
            return STOP_SOLO_MUTE_TOGGLE_PRESSED;
        default:
            return OTHER;
        }
    }
    else if (midiEvent->data[0] == 144 && midiEvent->data[2] == 127)
    { // note ON channel 1 velo > 0
        return SESSION_PAD_PRESSED;
    }
    else
    {
        return OTHER;
    }
}

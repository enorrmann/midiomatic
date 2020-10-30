#include "LaunchPadMiniMk3.hpp"
#include "DistrhoPlugin.hpp"
#include <iostream>

MidiEvent LaunchpadMiniMk3::GetSessionModeOnSysex()
{
    // 240 0 32 41 2 13 16 <mode> 247
    // Where <mode> is 0 for Standalone, 1 for DAW (Session / DAW Fader layouts enabled).
    //struct MidiEvent *sysExEvent = (struct MidiEvent *)malloc(sizeof(struct MidiEvent));
    MidiEvent sysExEvent;

    // si no los inicializo asi, se borran las variables
    uint8_t *dataExt = (uint8_t *)malloc(sizeof(uint8_t) * 9);
    sysExEvent.size = 9;
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
    noteOnEvent.data[0] = 143 + channel; //144 is note on, channel 1
    noteOnEvent.data[1] = note;
    noteOnEvent.data[2] = 100; // velo/volume
    return noteOnEvent;
}

MidiEvent LaunchpadMiniMk3::GetNoteOff(int channel, int note)
{
    struct MidiEvent noteOnEvent;
    noteOnEvent.size = 3;
    noteOnEvent.data[0] = 143 + channel; //144 is note on, channel 1
    noteOnEvent.data[1] = note;
    noteOnEvent.data[2] = 0; // velo/volume
    return noteOnEvent;
}

MidiEvent LaunchpadMiniMk3::GetPadOnNote(int x, int y, int color)
{
    struct MidiEvent noteOnEvent;
    noteOnEvent.size = 3;
    noteOnEvent.data[0] = 144;        //144 is note on, channel 1
    noteOnEvent.data[1] = x * 10 + y; // note 12 signals pad press on x1, y2
    noteOnEvent.data[2] = color;      // velo/volume
    return noteOnEvent;
}

MidiEvent LaunchpadMiniMk3::GetTestNote()
{
    struct MidiEvent noteEvent;
    noteEvent.size = 3;
    //noteEvent.data[0] = 150; // note on chan 7
    noteEvent.data[0] = 134; // note OFF chan 7
    noteEvent.data[1] = 36;
    noteEvent.data[2] = 100; // velo/volume
    return noteEvent;
}

LaunchpadMiniMk3::MessageType LaunchpadMiniMk3::GetMessageType(MidiEvent *midiEvent)
{
    if (midiEvent->data[0] == 176 && midiEvent->data[1] == 91 && midiEvent->data[2] == 127)
    {
        return KEY_UP_PRESSED;
    }
    else if (midiEvent->data[0] == 176 && midiEvent->data[1] == 92 && midiEvent->data[2] == 127)
    {
        return KEY_DOWN_PRESSED;
    }
    else if (midiEvent->data[0] == 176 && midiEvent->data[1] == 93 && midiEvent->data[2] == 127)
    {
        return KEY_LEFT_PRESSED;
    }
    else if (midiEvent->data[0] == 176 && midiEvent->data[1] == 95 && midiEvent->data[2] == 127)
    {
        return SESSION_MODE_SELECTED;
    }
    else if (midiEvent->data[0] == 144 && midiEvent->data[2] > 0)
    { // note ON channel 1 velo > 0
        return SESSION_PAD_PRESSED;
    }
    else
    {
        return OTHER;
    }
}

void LaunchpadMiniMk3::debug_midi_event(MidiEvent *midiEvent)
{
    /*std::cout << "midiEvent.data[0]" << (int)midiEvent->data[0] << std::endl;
    std::cout << "midiEvent.data[1]" << (int)midiEvent->data[1] << std::endl;
    std::cout << "midiEvent.data[2]" << (int)midiEvent->data[2] << std::endl;*/

    std::cout << "sizeof(midiEvent)" << sizeof(midiEvent) << std::endl;
    std::cout << "midiEvent.size" << (int)midiEvent->size << std::endl;
    std::cout << "midiEvent.dataExt[0]" << (int)midiEvent->dataExt[0] << std::endl;
    std::cout << "midiEvent.dataExt[1]" << (int)midiEvent->dataExt[1] << std::endl;
    std::cout << "midiEvent.dataExt[2]" << (int)midiEvent->dataExt[2] << std::endl;

    std::cout << "midiEvent.dataExt[7]" << (int)midiEvent->dataExt[7] << std::endl;
    std::cout << "midiEvent.dataExt[8]" << (int)midiEvent->dataExt[8] << std::endl;
}
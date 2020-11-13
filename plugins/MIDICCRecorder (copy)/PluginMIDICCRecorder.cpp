/*
 * MIDI CCRecorder plugin based on DISTRHO Plugin Framework (DPF)
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (C) 2019 Christopher Arndt <info@chrisarndt.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <iostream>
#include <iomanip>
#include <vector>

#include "PluginMIDICCRecorder.hpp"
#include "extra/Base64.hpp"


START_NAMESPACE_DISTRHO


// -----------------------------------------------------------------------

PluginMIDICCRecorder::PluginMIDICCRecorder()
    : Plugin(paramCount, presetCount, stateCount), playing(false)
{
    clearState();
    loadProgram(0);
}

// -----------------------------------------------------------------------
// Init

void PluginMIDICCRecorder::initParameter(uint32_t index, Parameter &parameter)
{
    if (index >= paramCount)
        return;

    parameter.hints = kParameterIsAutomable | kParameterIsInteger;
    parameter.ranges.def = 0;
    parameter.ranges.min = 0;
    parameter.ranges.max = 1;

    switch (index)
    {
    case paramRecordEnable:
        parameter.name = "Record";
        parameter.symbol = "rec_enable";
        parameter.hints |= kParameterIsBoolean;
        break;
    case paramTrigClear:
        parameter.name = "Clear";
        parameter.symbol = "trig_clear";
        parameter.hints |= kParameterIsTrigger;
        break;
    case paramTrigSend:
        parameter.name = "Send";
        parameter.symbol = "trig_send";
        parameter.hints |= kParameterIsTrigger;
        break;
    case paramTrigTransport:
        parameter.name = "Trigger send on transport start?";
        parameter.shortName = "Transport";
        parameter.symbol = "trig_transport";
        parameter.ranges.max = 2;
        parameter.enumValues.count = 3;
        parameter.enumValues.restrictedMode = true;
        {
            ParameterEnumerationValue *const channels = new ParameterEnumerationValue[3];
            parameter.enumValues.values = channels;
            channels[0].label = "Disabled";
            channels[0].value = 0;
            channels[1].label = "Always";
            channels[1].value = 1;
            channels[2].label = "Only at Position 0";
            channels[2].value = 2;
        }
        break;
    case paramTrigPCChannel:
        parameter.name = "Trigger send on PC?";
        parameter.shortName = "Trigger by PC?";
        parameter.symbol = "trig_pc_chan";
        parameter.ranges.def = 17;
        parameter.ranges.max = 17;
        parameter.enumValues.count = 18;
        parameter.enumValues.restrictedMode = true;
        {
            ParameterEnumerationValue *const channels = new ParameterEnumerationValue[18];
            parameter.enumValues.values = channels;
            channels[0].label = "Any channel";
            channels[0].value = 0;
            channels[1].label = "Channel 1";
            channels[1].value = 1;
            channels[2].label = "Channel 2";
            channels[2].value = 2;
            channels[3].label = "Channel 3";
            channels[3].value = 3;
            channels[4].label = "Channel 4";
            channels[4].value = 4;
            channels[5].label = "Channel 5";
            channels[5].value = 5;
            channels[6].label = "Channel 6";
            channels[6].value = 6;
            channels[7].label = "Channel 7";
            channels[7].value = 7;
            channels[8].label = "Channel 8";
            channels[8].value = 8;
            channels[9].label = "Channel 9";
            channels[9].value = 9;
            channels[10].label = "Channel 10";
            channels[10].value = 10;
            channels[11].label = "Channel 11";
            channels[11].value = 11;
            channels[12].label = "Channel 12";
            channels[12].value = 12;
            channels[13].label = "Channel 13";
            channels[13].value = 13;
            channels[14].label = "Channel 14";
            channels[14].value = 14;
            channels[15].label = "Channel 15";
            channels[15].value = 15;
            channels[16].label = "Channel 16";
            channels[16].value = 16;
            channels[17].label = "Disabled";
            channels[17].value = 17;
        }
        break;
    case paramTrigPC:
        parameter.name = "Program number";
        parameter.symbol = "trig_pc";
        parameter.ranges.max = 127;
        break;
    case paramSendChannel:
        parameter.name = "Send channel";
        parameter.symbol = "send_chan";
        parameter.ranges.max = 16;
        parameter.enumValues.count = 17;
        parameter.enumValues.restrictedMode = true;
        {
            ParameterEnumerationValue *const channels = new ParameterEnumerationValue[17];
            parameter.enumValues.values = channels;
            channels[0].label = "All";
            channels[0].value = 0;
            channels[1].label = "Channel 1";
            channels[1].value = 1;
            channels[2].label = "Channel 2";
            channels[2].value = 2;
            channels[3].label = "Channel 3";
            channels[3].value = 3;
            channels[4].label = "Channel 4";
            channels[4].value = 4;
            channels[5].label = "Channel 5";
            channels[5].value = 5;
            channels[6].label = "Channel 6";
            channels[6].value = 6;
            channels[7].label = "Channel 7";
            channels[7].value = 7;
            channels[8].label = "Channel 8";
            channels[8].value = 8;
            channels[9].label = "Channel 9";
            channels[9].value = 9;
            channels[10].label = "Channel 10";
            channels[10].value = 10;
            channels[11].label = "Channel 11";
            channels[11].value = 11;
            channels[12].label = "Channel 12";
            channels[12].value = 12;
            channels[13].label = "Channel 13";
            channels[13].value = 13;
            channels[14].label = "Channel 14";
            channels[14].value = 14;
            channels[15].label = "Channel 15";
            channels[15].value = 15;
            channels[16].label = "Channel 16";
            channels[16].value = 16;
        }
        break;
    case paramSendInterval:
        parameter.name = "Send interval (ms)";
        parameter.unit = "ms";
        parameter.symbol = "send_interval";
        parameter.ranges.min = 1;
        parameter.ranges.max = 200;
        break;
    }
}

/**
  Set the state key and default value of @a index.
  This function will be called once, shortly after the plugin is created.
*/
void PluginMIDICCRecorder::initState(uint32_t index, String &stateKey, String &defaultStateValue)
{
    if (index < stateCount)
    {
        char key[6];
        snprintf(key, 6, "ch-%02d", index);
        stateKey = key;
        defaultStateValue = "false";
    }
}

/**
  Set the name of the program @a index.
  This function will be called once, shortly after the plugin is created.
*/
void PluginMIDICCRecorder::initProgramName(uint32_t index, String &programName)
{
    if (index < presetCount)
    {
        programName = factoryPresets[index].name;
    }
}

// -----------------------------------------------------------------------
// Internal data

/**
  Optional callback to inform the plugin about a sample rate change.
*/
void PluginMIDICCRecorder::sampleRateChanged(double newSampleRate)
{
    fSampleRate = newSampleRate;
}

/**
  Get the current value of a parameter.
*/
float PluginMIDICCRecorder::getParameterValue(uint32_t index) const
{
    return fParams[index];
}

/**
  Change a parameter value.
*/
void PluginMIDICCRecorder::setParameterValue(uint32_t index, float value)
{
    switch (index)
    {
    case paramRecordEnable:
        fParams[index] = CLAMP(value, 0, 1);
        break;
    case paramTrigClear:
        fParams[index] = CLAMP(value, 0, 1);

        if (fParams[index] > 0.0f)
            clearState();

        break;
    case paramTrigSend:
        fParams[index] = CLAMP(value, 0, 1);

        if (fParams[index] > 0.0f)
            startSend();

        break;
    case paramTrigTransport:
        fParams[index] = CLAMP(value, 0, 2);
        break;
    case paramTrigPCChannel:
        fParams[index] = CLAMP(value, 0, 17);
        break;
    case paramTrigPC:
        fParams[index] = CLAMP(value, 0, 127);
        break;
    case paramSendChannel:
        fParams[index] = CLAMP(value, 0, 16);
        break;
    case paramSendInterval:
        fParams[index] = CLAMP(value, 0, 200);
        break;
    }
}

/**
  Get the value of an internal state.
  The host may call this function from any non-realtime context.
*/
String PluginMIDICCRecorder::getState(const char *key) const
{
    static const String sFalse("false");
    int index;

    if (std::strncmp(key, "ch-", 3) == 0)
    {
        try
        {
            index = std::stoi(key + 3);
        }
        catch (...)
        {
            return sFalse;
        }

        if (index < (int)stateCount)
        {
            return String::asBase64((char *)stateCC[index], 128);
        }
    }

    return sFalse;
}

/**
  Change an internal state.
*/
void PluginMIDICCRecorder::setState(const char *key, const char *value)
{
    int index;

    if (std::strncmp(key, "ch-", 3) == 0)
    {
        try
        {
            index = std::stoi(key + 3);
        }
        catch (...)
        {
            return;
        }

        if (index < (int)stateCount)
        {
            if (!std::strcmp(value, "false") == 0)
            {
                std::vector<uint8_t> state = d_getChunkFromBase64String(value);

                for (uint i = 0; i < state.size(); i++)
                {
                    stateCC[index][i] = state[i];
                }

                // std::cerr << "State 'ch-" << index << "': ";
                // for (uint i=0; i < 128; i++) {
                //     std::cerr << (uint) stateCC[index][i] << ",";
                // }
                // std::cerr << std::endl;
            }
        }
    }
}

/**
  Clear all internal state.
*/
void PluginMIDICCRecorder::clearState()
{
    for (uint i = 0; i < 16; i++)
    {
        for (uint j = 0; j < 128; j++)
        {
            stateCC[i][j] = 0xFF;
        }
    }
}

/**
  Load a program.
  The host may call this function from any context,
  including realtime processing.
*/
void PluginMIDICCRecorder::loadProgram(uint32_t index)
{
    if (index < presetCount)
    {
        for (int i = 0; i < paramCount; i++)
        {
            setParameterValue(i, factoryPresets[index].params[i]);
        }
    }
}

// -----------------------------------------------------------------------
// Process

/*
 *  Plugin is activated.
 */
void PluginMIDICCRecorder::activate()
{

    fSampleRate = getSampleRate();
    sendInProgress = false;
    playRecordedNote = false;
    loop_index = 0;
    curChan = 0;
    curCC = 0;
    lastRecordedIndex = 0;
    currentBarInLoop = 1;
}

/*
 *  Start sending.
 */
void PluginMIDICCRecorder::startSend()
{
    if (!sendInProgress)
    {
        sendChannel = fParams[paramSendChannel];
        curChan = 0;
        curCC = 0;
    }

    sendInProgress = true;
}

int32_t PluginMIDICCRecorder::getRecordPosition(TimePosition pos)
{

    // hint Number of ticks that have elapsed between frame 0 and the first beat of the current measure.
    // double TimePosition::BarBeatTick::barStartTick

    int32_t barRes = 4;
    int32_t barsToRecord = 2;
    // this value ranges from 0 to the amount of ticks in 4 beats in 4/4, because beats is 1 2 3 4 ...
    // ie: I'm looping 1 BAR
    int32_t totalTicks = pos.bbt.ticksPerBeat * pos.bbt.beatsPerBar;
    int32_t ticksNow = (pos.bbt.bar % barsToRecord) * pos.bbt.ticksPerBeat * (pos.bbt.beat - 1) + pos.bbt.tick;
    int32_t theValue = (ticksNow * barsToRecord * barRes / totalTicks) + 1; // record position
    std::cout << "pos.bbt.bar % barsToRecord " << pos.bbt.bar % barsToRecord << "\n";
    return theValue;
    //return (pos.bbt.ticksPerBeat * (pos.bbt.beat - 1) + pos.bbt.tick) / beatRes;
}
void PluginMIDICCRecorder::run(const float **, float **, uint32_t nframes,
                               const MidiEvent *events, uint32_t eventCount)
{

    /*if (eventCount > 1)
    {

        std::cout << "events[0].frame " << events[0].frame << "\n";
        std::cout << "events[1].frame " << events[1].frame << "\n";
    }*/

    const TimePosition &pos(getTimePosition());
    double bpm;
    if (pos.bbt.valid)
    {
        bpm = pos.bbt.beatsPerMinute;
    }
    else
    {
        bpm = 120;
    }

    double sesenta = 60.0;
    double s = sesenta / bpm;
    double numBeats = 4;
    double numBars = 2; // bars to loop

    if (pos.bbt.beat == 1 && !synced) // esta mal el sincro, no funciona asi
    {
        currentBarInLoop = currentBarInLoop + 1 > numBars ? 1 : currentBarInLoop + 1;

        curStep++;     // total step count
        synced = true; // sinced se usa para que no entre 2 veces en el mismo beat 1
        if (currentBarInLoop == 1)
        {
            loop_index = 0; // solo sincro una vez cada loop completo
        }
        std::cout << "beat 1 at frame " << pos.frame << "curStep " << curStep << "currentBarInLoop " << currentBarInLoop << "\n";
    }
    if (pos.bbt.beat > 1)
    {
        synced = false;
    }

    uint64_t loop_nsamp = fSampleRate * s * numBeats * numBars; // loop length in frames
                                                                /*std::cout << "fSampleRate " <<fSampleRate << "\n";
    std::cout << "loop_nsamp " <<loop_nsamp << "\n";*/

    for (uint32_t i = 0; i < eventCount; ++i)
    {
        struct MidiEvent midiEvent = events[i];

        if (midiEvent.data[0] == 144 || midiEvent.data[0] == 128) // note on or off channel 1
        {
            uint64_t absFrame = pos.frame + midiEvent.frame;

            recordedNote[lastRecordedIndex] = midiEvent;
            hasNote[lastRecordedIndex] = true;
            nextPlay[lastRecordedIndex] = absFrame % loop_nsamp;

            lastRecordedIndex = lastRecordedIndex + 1 >= 63 ? 0 : lastRecordedIndex + 1;

            std::cout << "recorded at loop_nsamp " << loop_nsamp << "\n";
        }
        //std::cout << "pos.frame " << pos.frame << " event.frame " << midiEvent.frame << "\n";

        writeMidiEvent(events[i]); // midi thru
    }

    for (int f = 0; f < nframes; f++)
    {

        for (int p = 0; p <= lastRecordedIndex; p++)
        {
            if (hasNote[p] && loop_index == nextPlay[p]) // send event at begining
            {
                writeMidiEvent(recordedNote[p]);
                std::cout << "PLAYED at loop_nsamp " << loop_nsamp << "\n";
            }
        }

        // if (loop_index == 0) // send event at begining
        //  {
        // struct MidiEvent midiEvent;
        // midiEvent.size = 3;
        // midiEvent.data[0] = 144;
        // midiEvent.data[1] = 36;  /* note  C2*/
        //midiEvent.data[2] = 100; /* velocity (volume)*/

        //writeMidiEvent(midiEvent);
        //std::cout << "sent eventg at frame " << pos.frame << "\n";
        //std::cout << "midiEvent.frame " << midiEvent.frame << "\n";
        // }

        loop_index = loop_index + 1 > loop_nsamp ? 0 : loop_index + 1;
    }

    return;

    uint8_t cc, chan, status;
    struct MidiEvent cc_event;
    bool block, start_send = false;
    static uint32_t next_frame = 0;

    //int32_t currentTick = getRecordPosition(pos);
    int32_t currentTick = 5000;

    //std::cout << "currentTick " << currentTick << "\n";

    /*std::cout << "pos.bbt.beat " << pos.bbt.beat << "\n";*/
    //std::cout << "nframes " << nframes << "\n";

    if (eventCount > 0 && events[0].data[0] == 144) //&& !playRecordedNote) // note on channel 0
    {
        recordedNote[currentTick] = events[0];
        hasNote[currentTick] = true;
        lastPlayed[currentTick] = pos.bbt.bar;
        std::cout << "recorded a note at " << currentTick << " of " << pos.bbt.ticksPerBeat << "\n";
    }
    if (eventCount > 0 && events[0].data[0] == 128) //&& !playRecordedNote) // note OFF channel 0
    {
        noteOff[currentTick] = events[0];
        hasNote[currentTick] = true;
        std::cout << "recorded a note off at " << currentTick << " of " << pos.bbt.ticksPerBeat << "\n";
    }
    //if ((currentTick == recordedNoteTime) && (pos.bbt.bar > lastPlayedBar))
    //if ((pos.bbt.beat == 1) && (pos.bbt.bar > lastPlayedBar))
    //std::cout << "lastPlayedBar " << lastPlayedBar << " and " << pos.bbt.bar << "\n";
    if (hasNote[currentTick] && lastPlayed[currentTick] < pos.bbt.bar)

    {
        lastPlayed[currentTick] = pos.bbt.bar;
        writeMidiEvent(recordedNote[currentTick]);
        writeMidiEvent(noteOff[currentTick]);
        std::cout << "played a note at " << currentTick << "\n";
    }

    uint8_t trig_pc = (uint8_t)fParams[paramTrigPC];
    uint8_t trig_pc_chan = (uint8_t)fParams[paramTrigPCChannel];
}

// -----------------------------------------------------------------------

Plugin *createPlugin()
{
    return new PluginMIDICCRecorder();
}

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#include <iostream>
#include "DistrhoPlugin.hpp"
#include "Util.hpp"
#include "Clip.hpp"

void Util::debug_clip(Clip *clip)
{
    std::cout << "******************" << std::endl;
    for (int i = 8; i >= 1; i--)
    {
        for (int j = 1; j <= 8; j++)
        {
            std::cout << clip->GetState(i, j);
        }
        std::cout << std::endl;
    }
    std::cout << "******************" << std::endl;
}

void Util::debug_pad_matrix(int pad_state[9][9])
{
    std::cout << "******************" << std::endl;
    for (int i = 8; i >= 1; i--)
    {
        for (int j = 1; j <= 8; j++)
        {
            std::cout << pad_state[i][j];
        }
        std::cout << std::endl;
    }
    std::cout << "******************" << std::endl;
}

void Util::debug_midi_event(MidiEvent *midiEvent)
{
    std::cout << "midiEvent.data[0]" << (int)midiEvent->data[0] << std::endl;
    std::cout << "midiEvent.data[1]" << (int)midiEvent->data[1] << std::endl;
    std::cout << "midiEvent.data[2]" << (int)midiEvent->data[2] << std::endl;

    /*std::cout << "sizeof(midiEvent)" << sizeof(midiEvent) << std::endl;
    std::cout << "midiEvent.size" << (int)midiEvent->size << std::endl;
    std::cout << "midiEvent.dataExt[0]" << (int)midiEvent->dataExt[0] << std::endl;
    std::cout << "midiEvent.dataExt[1]" << (int)midiEvent->dataExt[1] << std::endl;
    std::cout << "midiEvent.dataExt[2]" << (int)midiEvent->dataExt[2] << std::endl;

    std::cout << "midiEvent.dataExt[7]" << (int)midiEvent->dataExt[7] << std::endl;
    std::cout << "midiEvent.dataExt[8]" << (int)midiEvent->dataExt[8] << std::endl;*/
}
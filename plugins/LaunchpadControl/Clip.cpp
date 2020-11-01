#include "Clip.hpp"
#include <iostream>

int Clip::GetState()
{
    return state;
}
void Clip::SetState(int new_state)
{
    state = new_state;
}

int Clip::GetState(int x, int y)
{
    return pad_state[x][y];
}

void Clip::SetState(int x, int y, int state)
{
    pad_state[x][y] = state;
}

int Clip::HasNoteOn()
{
    for (int i = 1; i <= 8; i++)
    {
        for (int j = 1; j <= 8; j++)
        {
            if (pad_state[i][j] == 1)
            {
                return 1;
            }
        }
    }
    return 0;
}

int Clip::GetSeqNote(int step)
{
    if (drumClip)
    {
        return seq_notes_drum[step];
    }
    else
    {
        return seq_notes_inst[step];
    }
}

void Clip::Transpose(int octave)
{
    for (int i = 1; i <= 8; i++)
    {
        seq_notes_inst[i] = seq_notes_inst[i] + (12 * octave); // octave = +1 -1
        seq_notes_drum[i] = seq_notes_drum[i] + (8 * octave); // octave = +1 -1
    }
}

void Clip::SetChannel(int new_channel)
{
    channel = new_channel;
}
int Clip::GetChannel()
{
    return channel;
}
bool Clip::IsDrumClip()
{
    return drumClip;
}
void Clip::SetDrumClip(bool is_drum)
{
    drumClip = is_drum;
}

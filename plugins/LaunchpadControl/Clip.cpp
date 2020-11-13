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

int Clip::GetState(int step, int row)
{
    int v_step = (step + stepLength - 1) / stepLength;
    return pad_state[row][v_step]; // was step
}

void Clip::SetState(int step, int row, int state)
{
    /* example for me learning c++
    int disp[rows][steps] = {
    int disp[2][4] = {
        {10, 11, 12, 13},
        {14, 15, 16, 17}
    };
    */
    pad_state[row][step] = state;
}

int Clip::HasNoteOn()
{
    for (int step = 1; step <= CLIP_NUM_STEPS; step++)
    {
        for (int row = 1; row <= CLIP_NUM_ROWS; row++)
        {
            if (pad_state[row][step] == 1)
            {
                return 1;
            }
        }
    }
    return 0;
}

int Clip::GetSeqNote(int row)
{

    if (drumClip)
    {
        return seq_notes_drum[row];
    }
    else
    {
        return seq_notes_inst[row];
    }
}

void Clip::Transpose(int octave)
{
    for (int row = 1; row <= CLIP_NUM_ROWS; row++)
    {
        seq_notes_inst[row] = seq_notes_inst[row] + (12 * octave); // octave = +1 -1
        seq_notes_drum[row] = seq_notes_drum[row] + (8 * octave);  // octave = +1 -1
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
void Clip::Clear()
{
    for (int step = 1; step <= CLIP_NUM_STEPS; step++)
    {
        for (int row = 1; row <= CLIP_NUM_ROWS; row++)
        {
            pad_state[row][step] = 0;
        }
    }
}
int Clip::GetNumRows()
{
    return CLIP_NUM_ROWS;
}
int Clip::GetNumSteps()
{
    return CLIP_NUM_STEPS;
}

// estos se usan para setear las luces
int Clip::GetPageState(int step, int row)
{
    int absoluteStep = (page - 1) * CLIP_NUM_PADS + step;
    return pad_state[row][absoluteStep];
}

void Clip::SetPageState(int step, int row, int state)
{
    int absoluteStep = (page - 1) * CLIP_NUM_PADS + step;
    pad_state[row][absoluteStep] = state;
}

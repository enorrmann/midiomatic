#include "Clip.hpp"
#include <iostream>

int Clip::GetState(int x, int y)
{
    return pad_state[x][y];
}

void Clip::SetState(int x, int y, int state)
{
    pad_state[x][y] = state;
}
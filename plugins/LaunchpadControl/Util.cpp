#include <iostream>
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

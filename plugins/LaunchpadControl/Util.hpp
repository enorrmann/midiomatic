#ifndef LAUNCHPAD_UTIL_H
#define LAUNCHPAD_UTIL_H
#include "Clip.hpp"
class Util
{

public:
    void debug_clip(Clip *clip);
    void debug_pad_matrix(int[9][9]);
};

#endif
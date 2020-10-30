#ifndef CLIP_H
#define CLIP_H

/** 
 * A clip represents the state of the 64 pads of a controller
*/
class Clip
{
private:
    int pad_state[9][9]{0};

public:
    int pad_color = 77; // default color
    int channel = 2; // default channel
    int GetState(int x, int y);
    void SetState(int x, int y, int state);
};

#endif
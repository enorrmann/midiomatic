#ifndef CLIP_H
#define CLIP_H

/** 
 * A clip represents the state of the 64 pads of a controller
*/
class Clip
{
private:
    int pad_state[9][9]{0};
    int seq_notes_drum[9]{0, 60, 61, 62, 63, 64, 65, 66, 67};
    int seq_notes_inst[9]{0, 60, 62, 64, 65, 67, 69, 71, 72};
    int state = 0;

public:
    int pad_color = 77; // default color
    int channel = 2; // default channel
    int GetState(int x, int y);
    int GetSeqNote(int);
    void SetState(int x, int y, int state);
    int HasNoteOn();
    void Transpose(int octave);
    int GetState();
    void SetState(int state);
};

#endif
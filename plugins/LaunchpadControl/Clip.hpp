#ifndef CLIP_H
#define CLIP_H

#define CLIP_NUM_ROWS 8
#define CLIP_NUM_PADS 8
#define CLIP_NUM_STEPS 16

/** 
 * A clip represents the state of the 64 pads of a controller
*/
class Clip
{
private:
    /* example for me learning c++
     I got it upside down, where x is y actually
    int disp[rows][steps] = {
    int disp[2][4] = {
        {10, 11, 12, 13},
        {14, 15, 16, 17}
    };
    */
    int pad_state[CLIP_NUM_ROWS + 1][CLIP_NUM_STEPS + 1]{0};
    int seq_notes_drum[CLIP_NUM_ROWS + 1]{0, 60, 61, 62, 63, 64, 65, 66, 67};
    int seq_notes_inst[CLIP_NUM_ROWS + 1]{0, 60, 62, 64, 65, 67, 69, 71, 72};
    int state = 0;
    int channel = 0;
    bool drumClip = false;

public:
    int page = 1;
    int pad_color = 77; // default color
    int GetState(int, int);
    void SetState(int, int, int state);
    int GetPageState(int, int);
    void SetPageState(int, int, int state);
    int GetSeqNote(int);
    int HasNoteOn();
    void Transpose(int octave);
    int GetState();
    void SetState(int state);
    void SetChannel(int channel);
    int GetChannel();
    bool IsDrumClip();
    void SetDrumClip(bool);
    void Clear();

    int GetNumSteps();
    int GetNumRows();
};

#endif
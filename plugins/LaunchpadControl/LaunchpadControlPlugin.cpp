/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2018 Filipe Coelho <falktx@falktx.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "DistrhoPlugin.hpp"
#include "LaunchPadMiniMk3.hpp"
#include "Clip.hpp"
#include "Util.hpp"
#include <iostream>

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

/**
  Plugin that demonstrates MIDI output in sync with jack transport
 */
class LaunchpadControlPlugin : public Plugin
{
public:
  LaunchpadControlPlugin()
      : Plugin(0, 0, 0) {}

protected:
  /* --------------------------------------------------------------------------------------------------------
    * Information */

  // initialize
  void activate()
  {
    for (int x = 1; x <= 8; x++)
    {
      for (int y = 1; y <= 8; y++)
      {
        Clip *theClip = &clip_matrix[x][y];
        theClip->SetChannel(x + 1);
        theClip->SetDrumClip(x == 1); // first row is drum clip
      }
    }
  }
  /**
      Get the plugin label.
      This label is a short restricted name consisting of only _, a-z, A-Z and 0-9 characters.
    */
  const char *getLabel() const override
  {
    return "LaunchpadControl";
  }

  /**
      Get an extensive comment/description about the plugin.
    */
  const char *getDescription() const override
  {
    return "Plugin that demonstrates LaunchpadControl.";
  }

  /**
      Get the plugin author/maker.
    */
  const char *getMaker() const override
  {
    return "DISTRHO";
  }

  /**
      Get the plugin homepage.
    */
  const char *getHomePage() const override
  {
    return "https://github.com/DISTRHO/DPF";
  }

  /**
      Get the plugin license name (a single line of text).
      For commercial plugins this should return some short copyright information.
    */
  const char *getLicense() const override
  {
    return "ISC";
  }

  /**
      Get the plugin version, in hexadecimal.
    */
  uint32_t getVersion() const override
  {
    return d_version(1, 0, 0);
  }

  /**
      Get the plugin unique Id.
      This value is used by LADSPA, DSSI and VST plugin formats.
    */
  int64_t getUniqueId() const override
  {
    return d_cconst('J', 'i', 'p', 'V');
  }

  /* --------------------------------------------------------------------------------------------------------
    * Init and Internal data, unused in this plugin */

  void initParameter(uint32_t, Parameter &) override {}
  float getParameterValue(uint32_t) const override { return 0.0f; }
  void setParameterValue(uint32_t, float) override {}

  /* --------------------------------------------------------------------------------------------------------
    * /MIDI Processing */

  // calculate the previous step in the EditClip
  int get_prev_step(int cur_step)
  {
    return cur_step == 1 ? N_STEPS : cur_step - 1;
  }

  void send_midi_notes(int step)
  {

    int prev_step = get_prev_step(step);

    // for each clip in the clip matrix
    // this is correct, these are clips and 8x8 is ok
    for (int x = 1; x <= 8; x++)
    {
      for (int y = 1; y <= 8; y++)
      {
        Clip *theClip = &clip_matrix[x][y];
        if (theClip->GetState() == 0)
        {
          continue;
        }
        int channel = theClip->GetChannel();
        int drum_clip = theClip->IsDrumClip();

        // for each row in the clip
        for (int row = 1; row <= 8; row++)
        {

          int cur_state = theClip->GetState(step, row);
          int prev_state = theClip->GetState(prev_step, row);

          // if the state of this step is ON
          if (cur_state == 1)
          {
            // and the note was off // or this is a drum track, ie. ifnore note off
            if (prev_state == 0 || drum_clip == 1)
            {
              // send a note on event
              writeMidiEvent(launchPad.GetNoteOn(channel, theClip->GetSeqNote(row)));
            }
          }
          else
          {
            // the state of this step is OFF, check the state of the previous step to see if
            // and the note was ON, and stop it
            if (prev_state == 1)
            {
              writeMidiEvent(launchPad.GetNoteOff(channel, theClip->GetSeqNote(row)));
            }
          }
        }
      }
    }
  }
  
  // order is Select -> Edit -> Play -> Select ...
  void cycleMode()
  {
    if (mode == SelectClip)
    {
      mode = EditClip;
    }
    else if (mode == PlayClip)
    {
      mode = SelectClip;
      light_used_clips();
    }
    else
    {
      mode = PlayClip;
      selectedClip = metaClip;
      light_used_clips();
    }
    writeMidiEvent(launchPad.GetControlPadOnNote(1, 9, mode_color[mode]));
  }

  void clear_selected_clip()
  {
    writeMidiEvent(launchPad.GetSessionClearSysex());
    selectedClip->Clear();
  }

  void light_used_clips()
  {
    // turn off all pad lights
    writeMidiEvent(launchPad.GetSessionClearSysex());

    for (int i = 1; i <= 8; i++)
    {
      for (int j = 1; j <= 8; j++)
      {

        if (clip_matrix[i][j].GetState() == 1)
        {
          writeMidiEvent(launchPad.GetPadOnNote(i, j, mode_color[PlayClip]));
        }
        else
        {
          if (clip_matrix[i][j].HasNoteOn())
          {
            writeMidiEvent(launchPad.GetPadOnNote(i, j, mode_color[SelectClip]));
          }
        }
      }
    }
  }
  void light_selected_clip()
  {
    // turn off all pad lights
    writeMidiEvent(launchPad.GetSessionClearSysex());

    for (int step = 1; step <= 8; step++)
    {
      for (int row = 1; row <= 8; row++)
      {
        if (selectedClip->GetPageState(step, row) == 1)
        {
          writeMidiEvent(launchPad.GetPadOnNote(step, row, mode_color[EditClip]));
        }
        else
        {
          //writeMidiEvent(launchPad.GetPadOnNote(i, j, 0)); // turn off
        }
      }
    }
  }

  void process_audio(uint32_t nframes)
  {
    const TimePosition &pos(getTimePosition());

    if (pos.bbt.valid)
    {
      bpm = pos.bbt.beatsPerMinute;
    }
    else
    {
      bpm = 120;
    }

    // this is the source of the bug
    bpm *= 2; // fix beats 8 instead of 4
    //bpm *= 4; // this means resolution of 16th notes, ie, one click per 16th note
    wave_length = 60 * sr / bpm;

    offset = pos.frame % wave_length;

    uint32_t frames_left = nframes;

    while (wave_length - offset <= frames_left) // "<=" porque sino salta pasos
    {
      frames_left -= wave_length - offset;
      offset = 0;
      // sync with jack beat
      if (pos.bbt.beat == 1 && cur_step != 1 && cur_step != 5)
      {
        // cur_step = 1; // nosync for testing 8 steps
      }
      cur_step = cur_step + 1 > N_STEPS ? 1 : cur_step + 1;

      send_midi_notes(cur_step);
      //send_light_notes(cur_step);
    }
    if (frames_left > 0)
    {
      offset += frames_left;
    }
  }

  /**
      Run/process function for plugins with MIDI input.
      In this case we just pass-through all MIDI events.
    */
  void run(const float **, float **, uint32_t nframes,
           const MidiEvent *midiEvents, uint32_t midiEventCount) override
  {
    const TimePosition &pos(getTimePosition());

    for (uint32_t i = 0; i < midiEventCount; ++i)
    {
      struct MidiEvent midiEvent = midiEvents[i];

      LaunchpadMiniMk3::MessageType messageType = launchPad.GetMessageType(&midiEvent);

      if (messageType == LaunchpadMiniMk3::STOP_SOLO_MUTE_TOGGLE_PRESSED)
      {
        cycleMode();
      }

      if (messageType == LaunchpadMiniMk3::ARROW_1_PRESSED)
      {
        selectedClip->page = 1;
        light_selected_clip();
      }
      if (messageType == LaunchpadMiniMk3::ARROW_2_PRESSED)
      {
        selectedClip->page = 2;
        light_selected_clip();
      }
      if (messageType == LaunchpadMiniMk3::KEY_UP_PRESSED)
      {
        selectedClip->Transpose(1);
      }
      if (messageType == LaunchpadMiniMk3::KEY_DOWN_PRESSED)
      {
        selectedClip->Transpose(-1);
      }
      if (messageType == LaunchpadMiniMk3::KEY_LEFT_PRESSED)
      {
        //clear_selected_clip();
      }
      if (messageType == LaunchpadMiniMk3::KEY_RIGHT_PRESSED)
      {

        // todo, assign channel to clip
        //writeMidiEvent(launchPad.GetAllNoteOff());
      }
      if (messageType == LaunchpadMiniMk3::SESSION_MODE_PRESSED)
      {
        // send sysex to signal session mode available
        writeMidiEvent(launchPad.GetSessionModeOnSysex());
      }

      if (messageType == LaunchpadMiniMk3::SESSION_PAD_PRESSED)
      {
        // note 12 signals pad press on x1, y2
        int note = midiEvent.data[1];
        int x, y;
        x = note % 10;
        y = note / 10 % 10;

        // in this mode we edit the clips
        if (mode == EditClip)
        {
          if (selectedClip->GetPageState(x, y) == 0)
          {
            selectedClip->SetPageState(x, y, 1);      // turn ON
            midiEvent.data[2] = mode_color[EditClip]; // midi event velocity
          }
          else
          {
            selectedClip->SetPageState(x, y, 0); // turn OFF
            midiEvent.data[2] = COLOR_PAD_OFF;   // midi event velocity
          }
          writeMidiEvent(midiEvent);
        }
        // in this mode we select the clips
        if (mode == SelectClip)
        {
          selectedClip = &clip_matrix[x][y];
          light_selected_clip();
          writeMidiEvent(launchPad.GetPadOnNote(x, y, mode_color[SelectClip]));
          //mode = EditClip; // dont change mode if mode is not selected
        }
        // in this mode we play !
        if (mode == PlayClip)
        {
          selectedClip = &clip_matrix[x][y];
          if (selectedClip->HasNoteOn() == 1)
          {

            if (selectedClip->GetState() == 0)
            {
              selectedClip->SetState(1);                // turn ON
              midiEvent.data[2] = mode_color[PlayClip]; // midi event velocity
            }
            else
            {
              selectedClip->SetState(0);                  // turn OFF
              midiEvent.data[2] = mode_color[SelectClip]; // midi event velocity
              // send all notes off for the clip channel
              //writeMidiEvent(launchPad.GetAllNoteOff(selectedClip->GetChannel()));
            }
            writeMidiEvent(midiEvent);
          }
        }
      }
    }

    // if jack transport is not rolling, do nothing
    if (!pos.playing)
    {
      return;
    }

    process_audio(nframes);
  }

  // -------------------------------------------------------------------------------------------------------

private:
  enum Mode
  {
    SelectClip,
    EditClip,
    PlayClip
  };

  long offset = 0;
  uint32_t tone_length = 0;
  double sr = getSampleRate();
  double bpm= 0;
  uint32_t wave_length= 0;
  Clip clip_matrix[9][9]{};
  Clip *selectedClip = &clip_matrix[1][1];
  Clip *metaClip = &clip_matrix[0][0]; // this metaclip holds the state of all the other clips, ie, active, stopped, etc
  int mode_color[3]{25, 67, 95};
  Util util;
  const int N_STEPS = 16; // was 8 // was 16 // was 32// was 64
  int cur_step = 1;
  const uint8_t COLOR_PAD_ON = 66;
  const uint8_t COLOR_PAD_OFF = 0;
  const uint8_t COLOR_SEQ_ON = 77;
  const uint8_t COLOR_SELECTED_PAD = 88;
  int CHAN_1_NOTE_ON = 144;
  int CHAN_2_NOTE_ON = 145;
  int CHAN_7_NOTE_ON = 150;
  Mode mode = SelectClip;

  LaunchpadMiniMk3 launchPad;

  /**
      Set our plugin class as non-copyable and add a leak detector just in case.
    */
  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LaunchpadControlPlugin)
};

/* ------------------------------------------------------------------------------------------------------------
 * Plugin entry point, called by DPF to create a new plugin instance. */

Plugin *createPlugin()
{
  return new LaunchpadControlPlugin();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO

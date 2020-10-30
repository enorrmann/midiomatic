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

  // calculate the previous step in the sequence
  int get_prev_step(int cur_step)
  {
    return cur_step == 1 ? N_STEPS : cur_step - 1;
  }

  void send_test_note()
  {
    writeMidiEvent(launchPad.GetTestNote());
  }

  void send_midi_notes(int step)
  {

    int prev_step = get_prev_step(step);
    bool drumMode = true; // always trigger note after a previous note on

    // for each clip
    for (int c = 0; c <= 1; c++)
    {
      Clip *theClip = &clips[c];
      int channel = clip_channel[c];
      int drum_clip = is_drum_clip[c];

      //Clip *theClip = selectedClip;
      //Clip *theClip = &clips[0];

      // for each row in the clip
      for (int i = 1; i <= 8; i++)
      {

        int cur_state = theClip->GetState(i, step);
        int prev_state = theClip->GetState(i, prev_step);

        // if the state of this step is ON
        if (cur_state == 1)
        {
          // and the note was off // or this is a drum track, ie. ifnore note off
          if (prev_state == 0 || drum_clip == 1)
          {
            // send a note on event
            writeMidiEvent(launchPad.GetNoteOn(channel, seq_notes[i]));
          }
        }
        else
        {
          // the state of this step is OFF, check the state of the previous step to see if
          // and the note was ON, and stop it
          if (prev_state == 1)
          {
            writeMidiEvent(launchPad.GetNoteOff(channel, seq_notes[i]));
          }
        }
      }
    }
  }

  void clear_selected_clip()
  {
    writeMidiEvent(launchPad.GetSessionClearSysex());
    for (int i = 1; i <= 8; i++)
    {
      for (int j = 1; j <= 8; j++)
      {
        selectedClip->SetState(i, j, 0);
      }
    }
  }

  void light_selected_clip()
  {
    for (int i = 1; i <= 8; i++)
    {
      for (int j = 1; j <= 8; j++)
      {
        if (selectedClip->GetState(i, j) == 1)
        {
          writeMidiEvent(launchPad.GetPadOnNote(i, j, COLOR_PAD_ON));
        }
      }
    }
  }

  void select_clip(int clip)
  {
    writeMidiEvent(launchPad.GetSessionClearSysex());
    std::cout << "selected clip" << clip << std::endl;
    switch (clip)
    {
    case 1:
      selectedClip = &clips[0];
      break;

    default:
      selectedClip = &clips[1];
      break;
    }
    light_selected_clip();
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

      if (messageType == LaunchpadMiniMk3::KEY_UP_PRESSED)
      {
        std::cout << "keyup" << std ::endl;
        select_clip(1);
      }
      if (messageType == LaunchpadMiniMk3::KEY_DOWN_PRESSED)
      {
        std::cout << "key down" << std ::endl;
        select_clip(2);
      }
      if (messageType == LaunchpadMiniMk3::KEY_LEFT_PRESSED)
      {
        std::cout << "key left" << std ::endl;
        clear_selected_clip();
      }
      if (messageType == LaunchpadMiniMk3::SESSION_MODE_SELECTED)
      {
        // send sysex to signal session mode available
        writeMidiEvent(launchPad.GetSessionModeOnSysex());
      }

      if (messageType == LaunchpadMiniMk3::SESSION_PAD_PRESSED)
      {

        // note 12 signals pad press on x1, y2
        int note = midiEvent.data[1];
        int x, y;
        x = note / 10 % 10;
        y = note % 10;

        if (selectedClip->GetState(x, y) == 0)
        {
          selectedClip->SetState(x, y, 1);  // turn ON
          midiEvent.data[2] = COLOR_PAD_ON; // midi event velocity
        }
        else
        {
          selectedClip->SetState(x, y, 0);   // turn OFF
          midiEvent.data[2] = COLOR_PAD_OFF; // midi event velocity
        }

        writeMidiEvent(midiEvent);
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
  long offset = 0;
  uint32_t tone_length = 0;
  double sr = getSampleRate();
  double bpm;
  uint32_t wave_length;
  Clip clips[2];
  int is_drum_clip[2] = {1, 0}; // clip 1 is a drum clip
  int clip_channel[2] = {7, 8};
  Clip *selectedClip = &clips[0];
  Util util;
  const int N_STEPS = 8;
  int cur_step = 1;
  const uint8_t COLOR_PAD_ON = 66;
  const uint8_t COLOR_PAD_OFF = 0;
  const uint8_t COLOR_SEQ_ON = 77;
  int seq_notes[9]{0, 60, 61, 62, 63, 64, 65, 66, 67};
  int CHAN_1_NOTE_ON = 144;
  int CHAN_2_NOTE_ON = 145;
  int CHAN_7_NOTE_ON = 150;
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

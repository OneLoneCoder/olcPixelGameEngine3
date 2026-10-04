# olcPGEX3_Miniaudio

An audio extension for the [OneLoneCoder Pixel Game Engine 3](https://github.com/OneLoneCoder) built on top of the [miniaudio](https://miniaud.io) library. It gives you simple, high-level loading and playback of sound files, real-time waveform generation, and hooks for custom synthesis, while still letting you reach down into miniaudio for anything the extension doesn't wrap.

Because it sits on miniaudio, it needs next to no extra build configuration to work cross-platform.

## Features

- **Sound loading** from disk or from memory (WAV and MP3, plus anything your miniaudio build can decode)
- **Polyphonic playback**: each sound has a pool of voices (default 8), so rapid-fire sound effects overlap instead of cutting each other off
- **Per-sound controls**: volume, pan, pitch, looping, seek, forward, rewind, pause, toggle
- **Playback position** queries in milliseconds or as a normalised float (handy for progress bars)
- **Waveform generators**: sine, square, triangle and sawtooth, with adjustable amplitude, frequency and type, and a short fade in/out to avoid clicks
- **Synth callback**: generate or process audio sample by sample
- **Raw data callback**: take over the audio device output completely
- **Built-in limiter** to prevent clipping when many sources play at once
- **Background playback control**: audio mutes when the window loses focus, unless you enable background playback
- **Escape hatches** to the underlying `ma_engine`, `ma_device`, `ma_resource_manager`, `ma_sound` and `ma_waveform` objects
- **Platform support** for desktop, Emscripten (web) and Android asset loading

## Requirements

- Pixel Game Engine 3 (`olcPixelGameEngine3.h`, or `olcpge3.h` if you define `OLC_MULTIHEADER`)
- `miniaudio.h` on your include path
- A **C++20** compiler (the extension uses `std::span`)

## Installation

1. Copy `olcPGEX3_Miniaudio.h` next to your project (or anywhere on your include path).
2. Make sure `miniaudio.h` is available.
3. In **exactly one** translation unit, define `OLC_PGEX3_MINIAUDIO` before including the extension. This pulls in both the extension's implementation and miniaudio's implementation.

```cpp
#define OLC_PGE3_APPLICATION
#include "olcPixelGameEngine3.h"

#define OLC_PGEX3_MINIAUDIO
#include "olcPGEX3_Miniaudio.h"
```

In any other file that needs the audio types, include the header **without** the define.

## Quick start

Make the `AudioEngine` a member of your application class, and install it as a system extension in the constructor:

```cpp
class MyGame : public olc::PixelGameEngine
{
public:
    MyGame()
    {
        sAppName = "My Game";

        if(!InstallSystemExtension(&audio))
            throw std::runtime_error("Failed to install olcPGEX3_Miniaudio");
    }

    bool OnUserCreate() override
    {
        audio.CreateSoundFromFile(music, "assets/song.mp3");
        audio.CreateSoundFromFile(jump,  "assets/jump.wav");
        audio.Play(music, true); // loop the music
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        if(keyboard.GetKey(olc::Key::SPACE).bPressed)
            audio.Play(jump);

        return true;
    }

    olc::ext::Miniaudio::AudioEngine audio;

private:
    olc::ext::Miniaudio::Sound music;
    olc::ext::Miniaudio::Sound jump;
};
```

All types live in the `olc::ext::Miniaudio` namespace.

## Configuration

To change the defaults, call `Configure()` **before** the extension is installed (that is, before `InstallSystemExtension`). Calling it afterwards prints an error and has no effect.

```cpp
olc::ext::Miniaudio::AudioEngine::Config cfg;
cfg.DeviceChannels   = 2;
cfg.DeviceSampleRate = 44100;
cfg.BackgroundPlay   = true;

audio.Configure(cfg);
InstallSystemExtension(&audio);
```

| Field | Default | Description |
|---|---|---|
| `DeviceChannels` | `2` | Number of output channels |
| `DeviceFormat` | `ma_format_f32` | Sample format of the device |
| `DeviceSampleRate` | `48000` | Sample rate in Hz |
| `DeviceType` | `ma_device_type_playback` | Miniaudio device type |
| `BackgroundPlay` | `false` | Keep playing when the window is unfocused |
| `Verbose` | `false` | Verbose logging |

Background playback can also be toggled at runtime:

```cpp
audio.EnableBackgroundPlayback();
audio.DisableBackgroundPlayback();
```

When background playback is off and the window loses focus, the output is silenced (sounds keep advancing, they just aren't audible).

## Working with sounds

### Loading

```cpp
olc::ext::Miniaudio::Sound sound;

// from a file on disk (on Android, loaded from the APK assets)
audio.CreateSoundFromFile(sound, "assets/explosion.wav");

// from memory
audio.CreateSoundFromMemory(sound, pData, nBytes);
audio.CreateSoundFromMemory(sound, vecData);

// optionally choose how many simultaneous voices the sound gets (default 8)
audio.CreateSoundFromFile(sound, "assets/shot.wav", 16);
```

Each loader returns `true` on success and `false` on failure. The sound is fully decoded before the call returns, so it is ready to play immediately. Check at any time with `audio.IsLoaded(sound)`.

Release a sound when you're done with it:

```cpp
audio.DestroySound(sound);
```

Any sounds still alive when the `AudioEngine` is destroyed are cleaned up automatically.

### Playback

```cpp
audio.Play(sound);                          // play once
audio.Play(sound, true);                    // loop
audio.Play(sound, false, 0.5f);             // half volume
audio.Play(sound, false, 1.0f, -1.0f);      // hard left
audio.Play(sound, false, 1.0f, 0.0f, 1.5f); // higher pitch
```

The signature is `Play(sound, looping, volume, pan, pitch)`. The default values for volume, pan and pitch are deliberately out of range (`2.0f`), which means "leave this setting as it is". Values you do pass are clamped to their valid range.

| Function | Description |
|---|---|
| `Play(sound, looping, volume, pan, pitch)` | Start the sound from the beginning on the next voice |
| `Stop(sound)` | Stop and rewind to the beginning |
| `Pause(sound)` | Stop without changing position |
| `Toggle(sound)` | Switch between playing and paused (resumes from the paused position) |
| `Seek(sound, ms)` | Jump to a position in milliseconds |
| `Seek(sound, float)` | Jump to a position from `0.0f` (start) to `1.0f` (end) |
| `Forward(sound, ms)` / `Rewind(sound, ms)` | Move relative to the current position |
| `SetVolume(sound, v)` | `0.0f` is mute, `1.0f` is full |
| `SetPan(sound, p)` | `-1.0f` left, `0.0f` centre, `1.0f` right |
| `SetPitch(sound, p)` | `1.0f` is normal speed and pitch |
| `IsPlaying(sound)` | True if any voice of the sound is playing |
| `GetCursor(sound)` | Current position in milliseconds |
| `GetCursorFloat(sound)` | Current position as `0.0f` to `1.0f` |

### How voices work

Every call to `Play()` moves to the next voice in a round-robin pool and starts it from the beginning. This is what lets the same sound effect overlap with itself. A few things follow from this:

- `Stop`, `Pause`, `Toggle`, `Seek`, `Forward`, `Rewind`, `GetCursor` and `GetCursorFloat` act on the **current** voice (the most recently started one).
- `SetVolume`, `SetPan` and `SetPitch` apply to **all** voices of the sound.
- `IsPlaying` returns true if **any** voice is playing.
- If a sound was paused, calling `Play()` reuses the paused voice and restarts it from the beginning. Use `Toggle()` to resume instead.
- When all voices are busy, the oldest is reused, so give frequently triggered effects more voices.

A music track with a progress bar, as in the demo:

```cpp
if(keyboard.GetKey(olc::Key::SPACE).bPressed)
    audio.Toggle(song1);

float progress = audio.GetCursorFloat(song1);
draw.FilledRect({0, 350}, {ScreenSize().x * progress, 20}, olc::Colour::YELLOW);
```

> **Note:** a sound that has never had `Play()` called on it can be started with `Toggle()`, as the demo does with its music.

## Waveforms

The extension includes a simple oscillator for tones, beeps and chiptune-style effects.

```cpp
olc::ext::Miniaudio::Waveform sine;

// type, amplitude (0.0 - 1.0), frequency in Hz
audio.CreateWaveform(sine, olc::ext::Miniaudio::Waveform::Type::Sine, 0.1, 440.0);
```

Available types: `Sine`, `Square`, `Triangle`, `Sawtooth`.

| Function | Description |
|---|---|
| `CreateWaveform(wave, type, amplitude, frequency)` | Create a generator. Returns `false` on failure |
| `DestroyWaveform(wave)` | Free a generator |
| `Play(wave)` / `Stop(wave)` | Fade the waveform in or out (about 20 ms ramp, so there are no clicks) |
| `SetWaveformAmplitude(wave, amp)` | Change amplitude |
| `SetWaveformFrequency(wave, hz)` | Change frequency |
| `SetWaveformType(wave, type)` | Change shape |
| `IsPlaying(wave)` / `IsLoaded(wave)` | State queries |

Waveforms are a natural fit for "play while key is held" behaviour. Because `Play` and `Stop` only set a target level, you can call them every frame:

```cpp
audio.Stop(sine);
if(keyboard.GetKey(olc::Key::K7).bHeld)
    audio.Play(sine);
```

## Custom audio

### Synth callback

For procedural audio, register a callback that is invoked once per sample frame. Whatever you write to the output channels is **added** to the mix (sounds and waveforms keep playing alongside it).

```cpp
float phase = 0.0f;

audio.SetSynthCallback([&](float& left, float& right, float fElapsedTime)
{
    phase += 440.0f * fElapsedTime;
    float s = 0.1f * sinf(phase * 2.0f * 3.14159f);

    left  = s;
    right = s;
});

// later
audio.ClearSynthCallback();
```

`fElapsedTime` is the duration of one sample (`1 / sample rate`). Always assign both `left` and `right`. They are not pre-zeroed.

### Raw data callback

For complete control, replace the extension's mixing entirely:

```cpp
audio.SetDataCallback([](float* pFramesOut, ma_uint64 frameCount)
{
    // fill pFramesOut with frameCount * channels interleaved float samples
});

audio.ClearDataCallback();
```

While a data callback is set, the extension does **no** mixing at all. Sounds, waveforms, the synth callback, the limiter and the focus muting are all bypassed. This callback runs on the audio thread, so keep it fast and avoid allocating memory or locking.

### Output limiter

The final mix passes through a simple limiter that ducks the gain when the signal would exceed full scale and recovers it slowly afterwards. This keeps stacked effects from clipping harshly. Keep individual sound volumes and waveform amplitudes sensible (the demo uses `0.1` for waveforms) to leave headroom.

## Advanced: direct miniaudio access

When you need something the extension doesn't wrap, you can get at the underlying miniaudio objects:

| Function | Returns |
|---|---|
| `GetMASound(sound)` | `ma_sound*` for a voice of the sound |
| `GetMAWaveform(wave)` | `ma_waveform*` |
| `GetEngine()` | `ma_engine&` |
| `GetDevice()` | `ma_device&` |
| `GetResourceManager()` | `ma_resource_manager&` |
| `GetDeviceChannels()`, `GetDeviceFormat()`, `GetDeviceSampleRate()`, `GetDeviceType()` | Current device settings |

`GetMASound` returns the currently playing voice, or the *next* voice to be used if the sound isn't playing. It returns `nullptr` if the sound isn't loaded.

For example, the demo uses miniaudio's 3D audio features to position a sound and move the listener, producing a distance effect:

```cpp
ma_sound_set_position(audio.GetMASound(song1), 0.0f, 0.0f, 0.0f);
ma_engine_listener_set_position(&audio.GetEngine(), 0, 0.0f, distance, 0.0f);
```

Per-voice settings made through miniaudio directly only affect that one voice. For anything that should apply to every voice of a sound, prefer the extension's own functions.

## Platform notes

- **Emscripten:** the extension configures miniaudio's resource manager for single-threaded, non-blocking operation and processes pending jobs each frame automatically. Browsers require a user gesture before audio starts, so expect silence until the user interacts with the page.
- **Android:** `CreateSoundFromFile` reads from the app's bundled assets.
- **Desktop:** files are read from disk using the path you provide, relative to the working directory.

## Credits and licence

Copyright 2023-2026 Moros Smith. Licensed under the **OLC-3** licence (see the header of `olcPGEX3_Miniaudio.h` for the full text).

Demo music: *Joy Ride [Full version]* by MusicLFiles, from [filmmusic.io](https://filmmusic.io/song/11627-joy-ride-full-version), licensed under [CC BY 4.0](https://filmmusic.io/standard-license).

Built on [miniaudio](https://miniaud.io) by David Reid.
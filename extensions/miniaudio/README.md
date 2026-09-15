# olcPGEX3_Miniaudio - Usage Documentation

## Overview

**olcPGEX3_Miniaudio** is an olcPixelGameEngine3 extension that provides a simple, abstracted interface to the powerful **miniaudio** library. It enables easy loading and playback of WAV and MP3 audio files with support for various audio effects and controls.

### Key Features
- Load and play WAV and MP3 files
- Generate procedural waveforms (Sine, Square, Triangle, Sawtooth)
- Full playback control (play, pause, stop, toggle)
- Audio effects: volume, pan, pitch adjustment
- Audio seeking and cursor position tracking
- Multiple voice support for simultaneous playback
- Synthesizer callbacks for procedural audio generation
- Cross-platform support (including Emscripten)
- Custom audio data callbacks for advanced users
- Exposure to underlying miniaudio library, for advanced users
---

## Installation & Setup

### 1. Acquire miniaudio

Visit the [miniaudio website](https://miniaud.io/) to download the header file. Simply add it to your project with the rest of your source/header files.

### 2. Include the Header after PGE3
```cpp
#define OLC_PGEX3_MINIAUDIO
#include "olcPGEX3_Miniaudio.h"
```

### 3. Install the Extension in Your PGE Application
```cpp
class MyGame : public olc::PixelGameEngine
{
public:
    MyGame()
    {
        sAppName = "My Audio Game";
        if(!InstallSystemExtension(&audio))
            throw std::runtime_error("Failed to install olcPGEX3_miniaudio");
    }

private:
    olc::ext::Miniaudio::AudioEngine audio;
};
```

See [olcPGE3_Miniaudio.cpp](olcPGE3_Miniaudio.cpp) for practical example usage!

---
## Core Components

### olc::ext::Miniaudio::AudioEngine

The main interface to the audio system. Manages all sounds, waveforms, and the underlying audio device.

#### Configuration

Configure the audio engine before creation in your constructor:

```cpp
olc::ext::Miniaudio::AudioEngine::Config config;
config.DeviceChannels = 2;              // Mono (1) or Stereo (2)
config.DeviceSampleRate = 48000;        // Sample rate in Hz
config.DeviceFormat = ma_format_f32;    // Audio format (32-bit float)
config.BackgroundPlay = true;           // Play when window unfocused
config.Verbose = false;                 // Enable verbose logging

audio.Configure(config);
```

#### Background Playback

Control whether audio plays when the window loses focus:

```cpp
audio.EnableBackgroundPlayback();   // Continue playing in background
audio.DisableBackgroundPlayback();  // Stop when window loses focus
```

#### Accessing Low-Level Objects

For advanced users who need direct access to miniaudio objects:

```cpp
ma_engine& engine = audio.GetEngine();
ma_device& device = audio.GetDevice();
ma_resource_manager& rm = audio.GetResourceManager();

int channels = audio.GetDeviceChannels();
int sampleRate = audio.GetDeviceSampleRate();
ma_format format = audio.GetDeviceFormat();
```

---

## olc::ext::Miniaudio::Sound

Represents a playable audio resource that can be loaded from file or memory.

### Loading Sounds

#### From File
```cpp
olc::ext::Miniaudio::Sound mySound;

// Load from disk
audio.CreateSoundFromFile(mySound, "path/to/sound.mp3"); // could also be .wav
```

#### From Memory
```cpp
// Load from memory buffer
std::vector<uint8_t> audioData = /* ... load file data ... */;
audio.CreateSoundFromMemory(mySound, audioData.data(), audioData.size());

// Or pass vector directly
audio.CreateSoundFromMemory(mySound, audioData);
```

### Playback Control

#### Play
```cpp
// Play once
mySound.Play();

// Play with looping
mySound.Play(true);  // Loops continuously
```

#### Stop
```cpp
// Stop and rewind to beginning
mySound.Stop();
```

#### Pause
```cpp
// Pause without changing position
mySound.Pause();
```

#### Toggle
```cpp
// Play if paused, pause if playing
mySound.Toggle();
```

### Seeking (Cursor Control)

#### Seek to Position
```cpp
// Seek to specific time (milliseconds)
mySound.Seek(5000);  // Go to 5 seconds

// Seek to position (0.0 = start, 1.0 = end)
mySound.Seek(0.5f);  // Go to middle of sound
```

#### Forward/Rewind
```cpp
// Move forward by 2 seconds
mySound.Forward(2000);

// Move backward by 1 second
mySound.Rewind(1000);
```

### Audio Effects

#### Volume Control
```cpp
// Set volume (0.0 = mute, 1.0 = full volume)
mySound.SetVolume(0.5f);  // 50% volume
mySound.SetVolume(0.0f);  // Muted
mySound.SetVolume(1.0f);  // Full volume
```

#### Panning
```cpp
// Set pan (-1.0 = left, 0.0 = center, 1.0 = right)
mySound.SetPan(-1.0f);   // Full left
mySound.SetPan(0.0f);    // Center
mySound.SetPan(1.0f);    // Full right
mySound.SetPan(-0.5f);   // 50% left
```

#### Pitch
```cpp
// Set pitch (1.0 = normal, 0.5 = half speed, 2.0 = double speed)
mySound.SetPitch(1.0f);   // Normal speed
mySound.SetPitch(0.5f);   // Half speed (lower pitch)
mySound.SetPitch(2.0f);   // Double speed (higher pitch)
mySound.SetPitch(1.5f);   // 1.5x speed
```

### Information Queries

#### Playback Status
```cpp
// Check if currently playing
if(mySound.IsPlaying())
{
    // Sound is playing
}

// Check if loaded successfully
if(mySound.IsLoaded())
{
    // Sound was loaded successfully
}
```

#### Cursor Position
```cpp
// Get cursor position in milliseconds
ma_uint64 timeMs = mySound.GetCursor();
std::cout << "Position: " << timeMs << " ms\n";

// Get cursor position as float (0.0 to 1.0)
float progress = mySound.GetCursorFloat();
std::cout << "Progress: " << (progress * 100) << "%\n";
```

### Advanced Usage

#### Direct Access to Miniaudio Sound
```cpp
// Get pointer to underlying ma_sound for advanced features
ma_sound* maSound = mySound.GetMASound();

// Example: Set 3D position
ma_sound_set_position(maSound, 10.0f, 0.0f, -5.0f);
```

---

## olc::ext::Miniaudio::Waveform

Generate and play procedural waveforms for synthesis and sound effects.

### Supported Waveform Types

```cpp
enum class Waveform::Type
{
    Sine,       // Smooth, pure tone
    Square,     // Digital, buzzy tone
    Triangle,   // Bright, harmonic tone
    Sawtooth    // Bright, harsh tone
};
```

### Creating Waveforms

```cpp
olc::ext::Miniaudio::Waveform sineWave;

audio.CreateWaveform(
    sineWave,
    olc::ext::Miniaudio::Waveform::Type::Sine,  // Type
    0.1,                                         // Amplitude (0.0-1.0)
    440.0                                        // Frequency in Hz (A4 note)
);
```

### Playback Control

#### Play
```cpp
sineWave.Play();
```

#### Stop
```cpp
sineWave.Stop();
```

### Configuration

#### Change Amplitude
```cpp
// Amplitude controls volume (0.0 = silent, 1.0 = loud)
sineWave.SetAmplitude(0.1);   // 10% volume
sineWave.SetAmplitude(0.5);   // 50% volume
```

#### Change Frequency
```cpp
// Frequency controls pitch in Hertz
sineWave.SetFrequency(440.0);   // A4 note
sineWave.SetFrequency(880.0);   // One octave higher
sineWave.SetFrequency(220.0);   // One octave lower
```

#### Change Type
```cpp
// Switch between waveform types
sineWave.SetType(olc::ext::Miniaudio::Waveform::Type::Square);
sineWave.SetType(olc::ext::Miniaudio::Waveform::Type::Triangle);
sineWave.SetType(olc::ext::Miniaudio::Waveform::Type::Sawtooth);
```

### Information Queries

```cpp
// Check if waveform is currently playing
if(sineWave.IsPlaying())
{
    // Waveform is playing
}

// Check if waveform was created successfully
if(sineWave.IsLoaded())
{
    // Waveform is ready to use
}
```

## Advanced Features

### Synthesizer Callback

Provide custom synthesis function for procedural audio generation:

```cpp
class MyGame : public olc::PixelGameEngine
{
private:
    olc::ext::Miniaudio::AudioEngine audio;

public:
    bool OnUserCreate() override
    {
        // Set custom synthesizer callback
        audio.SetSynthCallback([this](float& fLeftChannel, float& fRightChannel, float fElapsedTime)
        {
            // Generate custom audio
            // fElapsedTime is in seconds since last sample
            float frequency = 440.0f;
            float phase = fmodf(/* your phase */, 6.28f);
            float sample = sinf(phase);
            
            fLeftChannel = sample * 0.1f;
            fRightChannel = sample * 0.1f;
        });

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        // Clear callback if no longer needed
        if(keyboard.GetKey(olc::Key::C).bPressed)
        {
            audio.ClearSynthCallback();
        }

        return true;
    }
};
```

### Raw Audio Data Callback

For maximum control, provide a custom data callback:

```cpp
audio.SetDataCallback([this](float* pFramesOut, ma_uint64 frameCount)
{
    // pFramesOut: pointer to output buffer
    // frameCount: number of frames to fill
    
    // Fill the buffer with your custom audio data
    for(ma_uint64 i = 0; i < frameCount * 2; ++i)  // *2 for stereo
    {
        pFramesOut[i] = /* your audio sample */;
    }
});

// Clear callback later
audio.ClearDataCallback();
```
---

## Troubleshooting

1. **Sound Won't Play?**
    - Is the file path correct?
    - Is the audio engine installed via `InstallSystemExtension(&audio)`?
    - Use `IsLoaded()` to verify the sound loaded successfully
    - Is the volume set to 0?

2. **Audio Crackles or Pops**
    - This often indicates a limiter is catching clipping. Reduce the amplitude of waveforms or volume of sounds. Ensure the total output doesn't exceed 1.0 in amplitude

3. **Sounds Not Working on Emscripten/Web**
    - Browser audio requires user interaction before playing. Ensure playback is triggered by a user event (click, key press)

---

## License

Licensed under the OLC-3 License (OneLoneCoder Public License v3)

Copyright 2023-2026 Moros Smith

For full license details, see the header file or original source.

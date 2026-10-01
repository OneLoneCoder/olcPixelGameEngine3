#pragma once
/*
	olcPGEX3_MiniAudio.h

	+-------------------------------------------------------------+
	|         OneLoneCoder Pixel Game Engine Extension            |
	|                     Miniaudio v3.0                          |
	+-------------------------------------------------------------+

    What is this?
	~~~~~~~~~~~~~
    This extension abstracts the very robust and powerful miniaudio
    library. It provides simple loading and playback of WAV and MP3
    files. Because it's built on top of miniaudio, it requires next
    to no addictional build configurations in order to be built
    for cross-platform.

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2023-2026 Moros Smith <moros1138@gmail.com>

	Redistribution and use in source and binary forms, with or without modification,
	are permitted provided that the following conditions are met:

	1. Redistributions or derivations of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.

	2. Redistributions or derivative works in binary form must reproduce the above
	copyright notice. This list of conditions and the following	disclaimer must be
	reproduced in the documentation and/or other materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its contributors may
	be used to endorse or promote products derived from this software without specific
	prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS	"AS IS" AND ANY
	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
	OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
	SHALL THE COPYRIGHT	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
	INCIDENTAL,	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
	TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
	BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
	ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
	SUCH DAMAGE.

	Links
	~~~~~
	YouTube:	https://www.youtube.com/@Moros1138
	GitHub:		https://www.github.com/Moros1138
	Homepage:	https://moros1138.com
*/
#if defined(OLC_MULTIHEADER)
#include "olcpge3.h"
#else
#include "olcPixelGameEngine3.h"
#endif

#if defined(OLC_PGEX3_MINIAUDIO)
#define MINIAUDIO_IMPLEMENTATION
#endif

#include "miniaudio.h"

#include <cstring>
#include <fstream>
#include <functional>
#include <string>
#include <vector>
#include <span>

namespace olc::ext::Miniaudio
{
	class AudioEngine;

	namespace internal
	{
		struct SoundInstance
		{
			std::vector<uint8_t> buffer;
			ma_sound base_sound{0};
			std::vector<ma_sound> voices;
			uint32_t id{0};
			std::string virtual_path{""};
			bool is_loaded{false};
			bool is_paused{false};
			uint32_t num_voices{8};
			uint32_t current_voice;
			ma_uint64 length_in_pcm_frames{0};
			float length_in_seconds{0.0f};
			static uint32_t id_tracker;
		};
		
		struct WaveformInstance
		{
			bool is_loaded{false};
			float gain{0.0f};
			float target{0.0f};
			float rampStep{0.0f};
			ma_waveform waveform;
			ma_waveform_config waveform_config;
		};

	}

	class Sound
	{
		friend class AudioEngine;
	private:
		internal::SoundInstance* sound{nullptr};
	};

	class Waveform
	{
		friend class AudioEngine;
	public:
		enum class Type
		{
			Sine,
			Square,
			Triangle,
			Sawtooth
		};
	private:
		internal::WaveformInstance* waveform{nullptr};
	};

	class AudioEngine : public olc::PGESystemExtension
	{
	public:
		struct Config
		{
			// device: number of channels. default(2)
			int DeviceChannels{2};
			// device: format of the audio data. default(ma_format_f32)
			ma_format DeviceFormat{ma_format_f32};
			// device: sample rate. default(48000)
			int DeviceSampleRate{ma_standard_sample_rate_48000};
			// device: type of device. default(ma_device_type_playback)
			ma_device_type DeviceType{ma_device_type_playback};
			// Miniaudio: is background play enabled? default(false)
			bool BackgroundPlay{false};
			// Logging: is logging verbose? default(false)
			bool Verbose{false};
		};
		// configure the audio engine, see struct Config
		void Configure(const Config& cfg);
		// enable playback when the application window does not have focus.
		void EnableBackgroundPlayback();
		// disable playback when the application window does not have focus.
		void DisableBackgroundPlayback();

	public: // Callback
		static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

	public: // Sounds
		// Create a sound resource based on a sound file asset on disk
		bool CreateSoundFromFile(Sound& sound, const std::string& sFileName, uint32_t nNumVoices = 8);
		// Create a sound resource based on a sound file asset in memory
		bool CreateSoundFromMemory(Sound& sound, const uint8_t* data, const size_t bytes, uint32_t nNumVoices = 8);
		bool CreateSoundFromMemory(Sound& sound, const std::vector<uint8_t>& data, uint32_t nNumVoices = 8);
		void DestroySound(Sound& sound);
		bool _internalSoundLoader(Sound& sound);

	public: // Sound Playback and Controls
        // plays a sound, can set looping, volume, pan, or pitch (note: default values are out of range deliberately)
        void Play(Sound& sound, const bool looping = false, const float volume = 2.0f, const float pan = 2.0f, const float pitch = 2.0f);
        // stops a sound, rewinds to beginning
        void Stop(Sound& sound);
        // pauses a sound, does not change position
        void Pause(Sound& sound);
        // toggle between play and pause
        void Toggle(Sound& sound);
        // seek to the provided position in the sound, by milliseconds
        void Seek(Sound& sound, const ma_uint64 milliseconds);
        // seek to the provided position in the sound, by float 0.f is beginning, 1.0f is end
        void Seek(Sound& sound, const float& location);
        // seek forward from current position by the provided time
        void Forward(Sound& sound, const ma_uint64 milliseconds);
        // seek forward from current position by the provided time
        void Rewind(Sound& sound, const ma_uint64 milliseconds);
        // set volume of a sound, 0.0f is mute, 1.0f is full
        void SetVolume(Sound& sound, const float& volume);
        // set pan of a sound, -1.0f is left, 1.0f is right, 0.0f is center
        void SetPan(Sound& sound, const float& pan);
        // set pitch of a sound, 1.0f is normal
        void SetPitch(Sound& sound, const float& pitch);		
        // determine if a sound is playing
        bool IsPlaying(Sound& sound);
		// determine if this sound has been loaded successfully
		bool IsLoaded(Sound& sound) const;
        // gets the current position in the sound, in milliseconds
        ma_uint64 GetCursor(Sound& sound);
        // gets the current position in the sound, as a float between 0.0f and 1.0f
        float GetCursorFloat(Sound& sound);
	public: // advanced usage
		ma_sound* GetMASound(Sound& sound);

	public: // Waveforms
		// creates a waveform with the specified type, amplitude, and frequency
		bool CreateWaveform(Waveform& waveform, const Waveform::Type type, const double amplitude, const double frequency);
		// destroys a waveform
		void DestroyWaveform(Waveform& waveform);
		// plays a waveform
		void Play(Waveform& waveform);
		// stops a waveform
		void Stop(Waveform& waveform);
		// set the amplitude of a waveform
		void SetWaveformAmplitude(Waveform& waveform, const double amplitude);
		// set the frequency of a waveform
		void SetWaveformFrequency(Waveform& waveform, const double frequency);
		// set the type of a waveform
		void SetWaveformType(Waveform& waveform, const Waveform::Type type);
		// determine if this waveform is playing
		bool IsPlaying(Waveform& waveform) const;
		// determine if this waveform is loaded
		bool IsLoaded(Waveform& waveform) const;
		ma_waveform* GetMAWaveform(Waveform& waveform);

	public: // Synth
		void SetSynthCallback(std::function<void(float& fLeftChannel, float& fRightChannel, float fElapsedTime)> callback);
		void ClearSynthCallback();
	public: // Absolute POWER!!
		void SetDataCallback(std::function<void(float* pFramesOut, ma_uint64 frameCount)> callback);
		void ClearDataCallback();

	public: // getters
		// 
		ma_device& GetDevice();
		ma_engine& GetEngine();
		ma_resource_manager& GetResourceManager();

		int GetDeviceChannels() const;
		ma_format GetDeviceFormat() const;
		int GetDeviceSampleRate() const;
		ma_device_type GetDeviceType() const;

	public:
		AudioEngine();
		~AudioEngine();


		virtual bool OnInstall([[maybe_unused]] olc::PixelGameEngine* pge);
		virtual bool OnBeforeUserCreate([[maybe_unused]] olc::PixelGameEngine* pge);
		virtual bool OnAfterUserCreate([[maybe_unused]] olc::PixelGameEngine* pge);
		virtual bool OnBeforeSystemUpdate([[maybe_unused]] olc::PixelGameEngine* pge, [[maybe_unused]] float fElapsedTime);
		virtual bool OnAfterSystemUpdate([[maybe_unused]] olc::PixelGameEngine* pge, [[maybe_unused]] float fElapsedTime);
	
	private:
        Config m_cfg;
		ma_device m_device;
        ma_device_config m_device_config;
        
        ma_resource_manager m_resource_manager;
        ma_resource_manager_config m_resource_manager_config;

        ma_engine m_engine;
        ma_engine_config m_engine_config;
		std::vector<float> m_waveform_buffer;
		
		// synth callback function
		std::function<void(float& out_data_channel_left, float& out_data_channel_right, const float fElapsedTime)> m_synth_callback;
		// data callback function
		std::function<void(float* pFramesOut, ma_uint64 frameCount)> m_data_callback;

		// track sounds and waveforms
		std::vector<internal::SoundInstance*> m_sounds;
		std::vector<internal::WaveformInstance*> m_waveforms;

		bool m_is_initialized{false};
		olc::PixelGameEngine* m_pge{nullptr};		
	};
}

#ifdef OLC_PGEX3_MINIAUDIO
#undef OLC_PGEX3_MINIAUDIO

namespace olc::ext::Miniaudio
{
	uint32_t internal::SoundInstance::id_tracker = 0;

	AudioEngine::AudioEngine()
    {
    }

	AudioEngine::~AudioEngine()
    {
		if(m_is_initialized)
		{
			for(auto& sound : m_sounds)
			{
				if(sound == nullptr) continue;
				for(auto& v : sound->voices)
				{
					if(ma_sound_is_playing(&v))
						ma_sound_stop(&v);
					
					ma_sound_uninit(&v);
				}
				ma_sound_uninit(&sound->base_sound);
				ma_resource_manager_unregister_data(&m_resource_manager, sound->virtual_path.c_str());
				sound->is_loaded = false;
				
				delete sound;
			}
			m_sounds.clear();

			for(auto& w : m_waveforms)
			{
				if(w == nullptr) continue;
				if(!w->is_loaded) continue;
				ma_waveform_uninit(&w->waveform);
			}
			m_waveforms.clear();

			ma_resource_manager_uninit(&m_resource_manager);

			ma_engine_stop(&m_engine);
			ma_engine_uninit(&m_engine);
			
			ma_device_stop(&m_device);
			ma_device_uninit(&m_device);
		}
    }

	void AudioEngine::Configure(const Config& cfg)
	{
		if(m_is_initialized)
		{
			std::cerr << "olcPGEX3_miniaudio: Configure called after initialized.\n";
			return;
		}

		m_cfg = cfg;
	}

	void AudioEngine::EnableBackgroundPlayback()
	{
		m_cfg.BackgroundPlay = true;
	}
	
	void AudioEngine::DisableBackgroundPlayback()
	{
		m_cfg.BackgroundPlay = false;
	}

    void AudioEngine::data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
    {
		AudioEngine* ma = (AudioEngine*)pDevice->pUserData;
        if(ma == nullptr)
            throw std::runtime_error{"unable to access miniaudio pgex instance from data_callback"};

		// with great power comes...
		if(ma->m_data_callback)
		{
			ma->m_data_callback((float*)pOutput, frameCount);
			return;
		}

		const bool audioShouldPlay = !(!ma->m_cfg.BackgroundPlay && !ma->m_pge->IsFocused());
		
		std::span<float> engineBuffer((float*)pOutput, frameCount * ma->GetDeviceChannels());
		ma_engine_read_pcm_frames(&ma->m_engine, engineBuffer.data(), frameCount, NULL);
		
		// resize, if required. frameCount is not guaranteed not to change.
        if(ma->m_waveform_buffer.size() != (frameCount * ma->GetDeviceChannels()))
        {
            ma->m_waveform_buffer.resize(frameCount * ma->GetDeviceChannels(), 0);
        }

		// waveforms
		for(auto& w : ma->m_waveforms)
		{
			// if waveform is not playing, continue
			if (!(w->target > 0.0f || w->gain > 0.0f))
				continue;

			ma_waveform_read_pcm_frames(&w->waveform, ma->m_waveform_buffer.data(), frameCount, NULL);

			// if audio shouldn't play, move on here.
			if(!audioShouldPlay)
				continue;

			for(int frame = 0; frame < frameCount; ++frame)
			{
				// Ramp gain toward target one step per frame
				if (w->gain < w->target)
					w->gain = std::min(w->gain + w->rampStep, w->target);
				else if (w->gain > w->target)
					w->gain = std::max(w->gain - w->rampStep, w->target);

				for(int channel = 0; channel < ma->GetDeviceChannels(); ++channel)
				{
					int i = frame * ma->GetDeviceChannels() + channel;
					engineBuffer[i] += ma->m_waveform_buffer[i] * w->gain;
				}
			}
		}

		// synth function
		if(ma->m_synth_callback)
		{
            for(ma_uint32 i = 0; i < frameCount; i++)
            {
                float left, right;
                ma->m_synth_callback(left, right, 1.0f / ma->GetDeviceSampleRate());
				if(audioShouldPlay)
				{
					engineBuffer[(i * ma->GetDeviceChannels())]     += left;
					engineBuffer[(i * ma->GetDeviceChannels()) + 1] += right;
				}
            }
		}

		// limiter
		static float envelope = 1.0f;
		
		for(int i = 0; i < engineBuffer.size(); i++)
		{
			if(!audioShouldPlay)
			{
				engineBuffer[i] = 0.0f;
				continue;
			}

			float peak = fabsf(engineBuffer[i]);
			
			if (peak > 1.0f)
				envelope = fminf(envelope, 1.0f / peak); // duck the gain
			else
				envelope = fminf(1.0f, envelope * 1.001f); // slowly recover
			// limit the output
			engineBuffer[i] *= envelope;
		}
    }

	bool AudioEngine::CreateSoundFromFile(Sound& sound, const std::string& sFileName, uint32_t nNumVoices)
	{
		sound.sound = new internal::SoundInstance();
		
#if OLC_HOST == OLC_HOST_ANDROID
		AAsset* pAsset = AAssetManager_open(
			olc::host::Host_Android::androidApp->activity->assetManager,
			sFileName.c_str(),
			AASSET_MODE_BUFFER
		);
		
		if (pAsset == nullptr)
			return false;

		off_t size = AAsset_getLength(pAsset);
		sound.sound->buffer.resize(size);
		AAsset_read(pAsset, sound.sound->buffer.data(), size);
		AAsset_close(pAsset);
#else
		std::ifstream f(sFileName, std::ios::binary | std::ios::ate);
		if(f.fail())
			return false;
		sound.sound->buffer.resize(f.tellg());
		f.seekg(0);
		f.read(reinterpret_cast<char*>(sound.sound->buffer.data()), sound.sound->buffer.size());
		f.close();
#endif
		sound.sound->num_voices = nNumVoices;
		return _internalSoundLoader(sound);
	}
	
	bool AudioEngine::CreateSoundFromMemory(Sound& sound, const uint8_t* data, const size_t bytes, uint32_t nNumVoices)
	{
		sound.sound = new internal::SoundInstance();
		if(!data || bytes <= 0) return false;

		// Thanks Linh
		try
		{
			sound.sound->buffer.assign(data, data + bytes);
		}
		catch (const std::exception&)
		{
			sound.sound->buffer.clear();
			return false;
		}

		sound.sound->num_voices = nNumVoices;
		return _internalSoundLoader(sound);
	}
	
	bool AudioEngine::CreateSoundFromMemory(Sound& sound, const std::vector<uint8_t>& data, uint32_t nNumVoices)
	{
		sound.sound = new internal::SoundInstance();
		if(data.size() <= 0) return false;
		sound.sound->buffer = data;
		sound.sound->num_voices = nNumVoices;
		return _internalSoundLoader(sound);
	}
	
	bool AudioEngine::_internalSoundLoader(Sound& sound)
	{
		ma_result result;
		sound.sound->id = ++sound.sound->id_tracker;
		sound.sound->virtual_path = "sound/" + std::to_string(sound.sound->id); 
		
		result = ma_resource_manager_register_encoded_data(
			&m_resource_manager,
			sound.sound->virtual_path.c_str(),
			sound.sound->buffer.data(), sound.sound->buffer.size()
		);
		
		if(result != MA_SUCCESS)
			return false;

		ma_fence fence;
		result = ma_fence_init(&fence);

		if(result != MA_SUCCESS)
		{
			ma_resource_manager_unregister_data(&m_resource_manager, sound.sound->virtual_path.c_str());
			return false;
		}

		result = ma_sound_init_from_file(
			&GetEngine(),
			sound.sound->virtual_path.c_str(),
			MA_SOUND_FLAG_DECODE,
			nullptr,
			&fence,
			&sound.sound->base_sound
		);
		
		if(result != MA_SUCCESS)
		{
			ma_resource_manager_unregister_data(&m_resource_manager, sound.sound->virtual_path.c_str());
			return false;
		}
		
		sound.sound->voices.resize(sound.sound->num_voices);
		for(int i = 0; i < sound.sound->num_voices; ++i)
		{
			result = ma_sound_init_copy(&GetEngine(), &sound.sound->base_sound, 0, nullptr, &sound.sound->voices[i]);
			if(result != MA_SUCCESS)
				break;
		}
		
		// if the last result out of that loop isn't success, we failed
		if(result != MA_SUCCESS)
		{
			for(auto& v : sound.sound->voices)
				ma_sound_uninit(&v);
			ma_sound_uninit(&sound.sound->base_sound);
			ma_resource_manager_unregister_data(&m_resource_manager, sound.sound->virtual_path.c_str());
			return false;
		}
		
		// wait here until the sound is fully loaded and dedoded
		ma_fence_wait(&fence);
		ma_fence_uninit(&fence);

		ma_sound_get_length_in_pcm_frames(&sound.sound->base_sound, &sound.sound->length_in_pcm_frames);
		ma_sound_get_length_in_seconds(&sound.sound->base_sound, &sound.sound->length_in_seconds);

		sound.sound->is_loaded = true;
		m_sounds.push_back(sound.sound);
        return true;
	}

	void AudioEngine::DestroySound(Sound& sound)
	{
		if(sound.sound == nullptr) return;
		if(!sound.sound->is_loaded) return;
		for(auto& v : sound.sound->voices)
		{
			if(ma_sound_is_playing(&v))
				ma_sound_stop(&v);
			
			ma_sound_uninit(&v);
		}
		sound.sound->voices.clear();
		ma_sound_uninit(&sound.sound->base_sound);
		ma_resource_manager_unregister_data(&m_resource_manager, sound.sound->virtual_path.c_str());
		sound.sound->is_loaded = false;
		
		delete sound.sound;

		sound.sound = nullptr;
	}

	void AudioEngine::Play(Sound& sound, const bool looping, const float volume, const float pan, const float pitch)
	{
		if(sound.sound == nullptr) return;

		if(!sound.sound->is_paused)
			sound.sound->current_voice = (sound.sound->current_voice + 1) % sound.sound->num_voices;
		
		if(volume != 2.0f)
			ma_sound_set_volume(&sound.sound->voices[sound.sound->current_voice], std::clamp(volume, 0.0f, 1.0f));
		
		if(pan != 2.0f)
			ma_sound_set_pan(&sound.sound->voices[sound.sound->current_voice], std::clamp(pan, -1.0f, 1.0f));
		
		if(pitch != 2.0f)
			ma_sound_set_pitch(&sound.sound->voices[sound.sound->current_voice], std::max({0.0f, pitch}));
		
		ma_sound_set_looping(&sound.sound->voices[sound.sound->current_voice], looping);
		ma_sound_seek_to_pcm_frame(&sound.sound->voices[sound.sound->current_voice], 0);
		ma_sound_start(&sound.sound->voices[sound.sound->current_voice]);
		sound.sound->is_paused = false;
	}

	void AudioEngine::Stop(Sound& sound)
	{
		if(sound.sound == nullptr) return;
		if(!ma_sound_is_playing(&sound.sound->voices[sound.sound->current_voice]))
			return;
		ma_sound_stop(&sound.sound->voices[sound.sound->current_voice]);
		ma_sound_seek_to_pcm_frame(&sound.sound->voices[sound.sound->current_voice], 0);
	}

	void AudioEngine::Pause(Sound& sound)
	{
		if(sound.sound == nullptr) return;
		if(!ma_sound_is_playing(&sound.sound->voices[sound.sound->current_voice]))
			return;
		
		ma_sound_stop(&sound.sound->voices[sound.sound->current_voice]);
		sound.sound->is_paused = true;
	}

	void AudioEngine::Toggle(Sound& sound)
	{
		if(sound.sound == nullptr) return;
		if(ma_sound_is_playing(&sound.sound->voices[sound.sound->current_voice]))
		{
			ma_sound_stop(&sound.sound->voices[sound.sound->current_voice]);
			sound.sound->is_paused = true;
			return;
		}
		
		ma_sound_start(&sound.sound->voices[sound.sound->current_voice]);
		sound.sound->is_paused = false;
	}

	void AudioEngine::Seek(Sound& sound, const ma_uint64 milliseconds)
	{
		if(sound.sound == nullptr) return;
        ma_uint64 frame_to_seek_to = (milliseconds * GetDeviceSampleRate()) / 1000;
        ma_sound_seek_to_pcm_frame(&sound.sound->voices[sound.sound->current_voice], frame_to_seek_to);

	}

	void AudioEngine::Seek(Sound& sound, const float& location)
	{
		if(sound.sound == nullptr) return;
		ma_uint64 frame_to_seek_to = static_cast<ma_uint64>(sound.sound->length_in_pcm_frames * location);
		ma_sound_seek_to_pcm_frame(&sound.sound->voices[sound.sound->current_voice], frame_to_seek_to);
	}

	void AudioEngine::Forward(Sound& sound, const ma_uint64 milliseconds)
	{
		if(sound.sound == nullptr) return;
        ma_uint64 frame_to_seek_to;

        // get the current position
        ma_sound_get_cursor_in_pcm_frames(&sound.sound->voices[sound.sound->current_voice], &frame_to_seek_to);
        
        // calculate the step and add it to the current position
        frame_to_seek_to += ((milliseconds * GetDeviceSampleRate()) / 1000);

        // seek to the new position
        ma_sound_seek_to_pcm_frame(&sound.sound->voices[sound.sound->current_voice], frame_to_seek_to);
	}

	void AudioEngine::Rewind(Sound& sound, const ma_uint64 milliseconds)
	{
		if(sound.sound == nullptr) return;
        ma_uint64 frame_to_seek_to;

        // get the current position
        ma_sound_get_cursor_in_pcm_frames(&sound.sound->voices[sound.sound->current_voice], &frame_to_seek_to);
        
        // calculate the step and add it to the current position
        frame_to_seek_to -= ((milliseconds * GetDeviceSampleRate()) / 1000);

        // seek to the new position
        ma_sound_seek_to_pcm_frame(&sound.sound->voices[sound.sound->current_voice], frame_to_seek_to);
	}

	void AudioEngine::SetVolume(Sound& sound, const float& volume)
	{
		if(sound.sound == nullptr) return;
		for(auto& v : sound.sound->voices)
		{
			ma_sound_set_volume(&v, std::clamp(volume, 0.0f, 1.0f));
		}
	}

	void AudioEngine::SetPan(Sound& sound, const float& pan)
	{
		if(sound.sound == nullptr) return;
		for(auto& v : sound.sound->voices)
		{
			ma_sound_set_pan(&v, std::clamp(pan, -1.0f, 1.0f));
		}
	}

	void AudioEngine::SetPitch(Sound& sound, const float& pitch)
	{
		if(sound.sound == nullptr) return;
		for(auto& v : sound.sound->voices)
		{
			ma_sound_set_pitch(&v, std::max({0.0f, pitch}));
		}
	}

	bool AudioEngine::IsPlaying(Sound& sound)
	{
		if(sound.sound == nullptr) return false;
		for(auto& v : sound.sound->voices)
		{
			if(ma_sound_is_playing(&v))
				return true;
		}
		return false;
	}

	ma_uint64 AudioEngine::GetCursor(Sound& sound)
	{
		if(sound.sound == nullptr) return 0;
        ma_uint64 cursor;
        ma_sound_get_cursor_in_pcm_frames(&sound.sound->voices[sound.sound->current_voice], &cursor);
        return (cursor * 1000) / GetDeviceSampleRate();
	}

	float AudioEngine::GetCursorFloat(Sound& sound)
	{
		if(sound.sound == nullptr) return 0.0f;
        float cursor;
		ma_sound_get_cursor_in_seconds(&sound.sound->voices[sound.sound->current_voice], &cursor);
		return cursor / sound.sound->length_in_seconds;
	}

	bool AudioEngine::IsLoaded(Sound& sound) const
	{
		if(sound.sound == nullptr) return false;
		return sound.sound->is_loaded;
	}

	ma_sound* AudioEngine::GetMASound(Sound& sound)
	{
		if(sound.sound == nullptr) return nullptr;
		// realistically, one wouldn't call this unless it was loaded
		if(!sound.sound->is_loaded)
			return nullptr;
		
		// if we're not currently playing, get the pointer of the next voice
		if(!IsPlaying(sound))
			return &sound.sound->voices[(sound.sound->current_voice + 1) % sound.sound->num_voices];
		
		// if we're playing, get the pointer of the current voice
		return &sound.sound->voices[sound.sound->current_voice];
	}

	bool AudioEngine::CreateWaveform(Waveform& waveform, const Waveform::Type type, const double amplitude, const double frequency)
	{
		waveform.waveform = new internal::WaveformInstance();
		waveform.waveform->waveform_config = ma_waveform_config_init(
				GetDeviceFormat(),
				GetDeviceChannels(),
				GetDeviceSampleRate(),
				static_cast<ma_waveform_type>(type),
				amplitude,
				frequency
		);
		
		waveform.waveform->rampStep = 1.0f / (GetDeviceSampleRate() * 0.02f);

		if(ma_waveform_init(&waveform.waveform->waveform_config, &waveform.waveform->waveform) != MA_SUCCESS)
		{
			delete waveform.waveform;
			waveform.waveform = nullptr;
			return false;
		}
		
		waveform.waveform->is_loaded = true;
		m_waveforms.push_back(waveform.waveform);
		return true;
	}

	void AudioEngine::DestroyWaveform(Waveform& waveform)
	{
		if(waveform.waveform == nullptr) return;
		ma_waveform_uninit(&waveform.waveform->waveform);
		delete waveform.waveform;
		waveform.waveform = nullptr;
	}

	void AudioEngine::Play(Waveform& waveform)
	{
		if(!IsLoaded(waveform))
			return;
		waveform.waveform->target = 1.0f;
	}

	void AudioEngine::Stop(Waveform& waveform)
	{
		if(!IsLoaded(waveform))
			return;
		waveform.waveform->target = 0.0f;
	}

	void AudioEngine::SetWaveformAmplitude(Waveform& waveform, const double amplitude)
	{
		if(!IsLoaded(waveform))
			return;
		ma_waveform_set_amplitude(&waveform.waveform->waveform, amplitude);
	}

	void AudioEngine::SetWaveformFrequency(Waveform& waveform, const double frequency)
	{
		if(!IsLoaded(waveform))
			return;
		ma_waveform_set_frequency(&waveform.waveform->waveform, frequency);
	}

	void AudioEngine::SetWaveformType(Waveform& waveform, const Waveform::Type type)
	{
		if(!IsLoaded(waveform))
			return;
		ma_waveform_set_type(&waveform.waveform->waveform, static_cast<ma_waveform_type>(type));
	}

	bool AudioEngine::IsPlaying(Waveform& waveform) const
	{
		if(waveform.waveform == nullptr) return false;
		return waveform.waveform->target > 0.0f || waveform.waveform->gain > 0.0f;
	}

	bool AudioEngine::IsLoaded(Waveform& waveform) const
	{
		if(waveform.waveform == nullptr) return false;
		return waveform.waveform->is_loaded;
	}

	ma_waveform* AudioEngine::GetMAWaveform(Waveform& waveform)
	{
		if(waveform.waveform == nullptr) return nullptr;
		if(!waveform.waveform->is_loaded) return nullptr;
		return &waveform.waveform->waveform;
	}

	void AudioEngine::SetSynthCallback(std::function<void(float& fLeftChannel, float& fRightChannel, float fElapsedTime)> callback)
    {
        m_synth_callback = callback;
    }

    void AudioEngine::ClearSynthCallback()
    {
        m_synth_callback = {};
    }	

	void AudioEngine::SetDataCallback(std::function<void(float* pFramesOut, ma_uint64 frameCount)> callback)
	{
		m_data_callback = callback;
	}

	void AudioEngine::ClearDataCallback()
	{
		m_data_callback = {};
	}
	
	ma_device& AudioEngine::GetDevice()
	{
		return m_device;
	}

	ma_engine& AudioEngine::GetEngine()
	{
		return m_engine;
	}

	ma_resource_manager& AudioEngine::GetResourceManager()
	{
		return m_resource_manager;
	}

	int AudioEngine::GetDeviceChannels() const
	{
		return m_cfg.DeviceChannels;
	}

	ma_format AudioEngine::GetDeviceFormat() const
	{
		return m_cfg.DeviceFormat;
	}
	
	int AudioEngine::GetDeviceSampleRate() const
	{
		return m_cfg.DeviceSampleRate;
	}
	
	ma_device_type AudioEngine::GetDeviceType() const
	{
		return m_cfg.DeviceType;
	}

	bool AudioEngine::OnInstall([[maybe_unused]] olc::PixelGameEngine* pge)
	{
        m_pge = pge;

		m_device_config = ma_device_config_init(GetDeviceType());
        m_device_config.playback.format = GetDeviceFormat();
        m_device_config.playback.channels = GetDeviceChannels();
        m_device_config.sampleRate = GetDeviceSampleRate();
        m_device_config.dataCallback = AudioEngine::data_callback;
        m_device_config.pUserData = this;
		
        if(ma_device_init(NULL, &m_device_config, &m_device) != MA_SUCCESS)
		{
            std::cerr << "PGEX3_Miniaudio: failed to initialize device\n";
			return false;
		}

        m_resource_manager_config = ma_resource_manager_config_init();
        m_resource_manager_config.decodedFormat     = GetDeviceFormat();
        m_resource_manager_config.decodedChannels   = GetDeviceChannels();
        m_resource_manager_config.decodedSampleRate = GetDeviceSampleRate();
    
    #ifdef __EMSCRIPTEN__
        m_resource_manager_config.jobThreadCount = 0;                           
        m_resource_manager_config.flags |= MA_RESOURCE_MANAGER_FLAG_NON_BLOCKING;
        m_resource_manager_config.flags |= MA_RESOURCE_MANAGER_FLAG_NO_THREADING;
    #endif

        if(ma_resource_manager_init(&m_resource_manager_config, &m_resource_manager) != MA_SUCCESS)
		{
            std::cerr <<"PGEX3_Miniaudio: failed to initialize resource manager\n";
			return false;
		}
    
        m_engine_config = ma_engine_config_init();
        m_engine_config.pDevice = &m_device;
        m_engine_config.pResourceManager = &m_resource_manager;
    
        if(ma_engine_init(&m_engine_config, &m_engine) != MA_SUCCESS)
		{
			std::cerr << "PGEX3_Miniaudio: failed to initialize engine\n";
			return false;
		}
		m_is_initialized = true;
		return true;
	}
	
	bool AudioEngine::OnBeforeUserCreate([[maybe_unused]] olc::PixelGameEngine* pge)
	{
		return true;
	}
	
	bool AudioEngine::OnAfterUserCreate([[maybe_unused]] olc::PixelGameEngine* pge)
	{
		return true;
	}
	
	bool AudioEngine::OnBeforeSystemUpdate([[maybe_unused]] olc::PixelGameEngine* pge, [[maybe_unused]] float fElapsedTime)
	{
        #if OLC_HOST == OLC_HOST_EMSCRIPTEN
        ma_resource_manager_process_next_job(&m_resource_manager);
        #endif

		return true;
	}
	
	bool AudioEngine::OnAfterSystemUpdate([[maybe_unused]] olc::PixelGameEngine* pge, [[maybe_unused]] float fElapsedTime)
	{
		return true;
	}	
}

#endif
#include "../../include/core/AudioManager.h"
#include <cmath>
#include <cstring>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#endif

AudioManager::AudioManager() = default;

AudioManager::~AudioManager() {
    stopMusic();
}

void AudioManager::init() {
    // Nothing to initialize for Win32 sound
}

// Generate a tone with optional envelope
std::vector<uint8_t> AudioManager::generateTone(float frequency, float duration, float volume,
                                                  const std::function<float(float)>& envelope) {
    const int sampleRate = 44100;
    int numSamples = static_cast<int>(sampleRate * duration);
    std::vector<uint8_t> samples;
    samples.reserve(numSamples * 2); // 16-bit mono

    for (int i = 0; i < numSamples; i++) {
        float t = static_cast<float>(i) / sampleRate;
        float env = envelope ? envelope(t / duration) : 1.0f;
        float sample = std::sin(2.0f * 3.14159f * frequency * t) * 32767.0f * volume * env;
        sample = std::max(-32767.0f, std::min(32767.0f, sample));
        int16_t s = static_cast<int16_t>(sample);
        samples.push_back(s & 0xFF);
        samples.push_back((s >> 8) & 0xFF);
    }
    return samples;
}

// Generate noise
std::vector<uint8_t> AudioManager::generateNoise(float duration, float volume) {
    const int sampleRate = 44100;
    int numSamples = static_cast<int>(sampleRate * duration);
    std::vector<uint8_t> samples;
    samples.reserve(numSamples * 2);

    for (int i = 0; i < numSamples; i++) {
        float t = static_cast<float>(i) / sampleRate;
        float env = 1.0f - t / duration; // fade out
        float sample = (static_cast<float>(std::rand()) / RAND_MAX * 2.0f - 1.0f) * 32767.0f * volume * env;
        sample = std::max(-32767.0f, std::min(32767.0f, sample));
        int16_t s = static_cast<int16_t>(sample);
        samples.push_back(s & 0xFF);
        samples.push_back((s >> 8) & 0xFF);
    }
    return samples;
}

// Generate a frequency sweep
std::vector<uint8_t> AudioManager::generateSweep(float startFreq, float endFreq, float duration, float volume) {
    const int sampleRate = 44100;
    int numSamples = static_cast<int>(sampleRate * duration);
    std::vector<uint8_t> samples;
    samples.reserve(numSamples * 2);

    for (int i = 0; i < numSamples; i++) {
        float t = static_cast<float>(i) / sampleRate;
        float progress = t / duration;
        float freq = startFreq + (endFreq - startFreq) * progress;
        float env = 1.0f - progress * 0.5f; // slight fade
        float sample = std::sin(2.0f * 3.14159f * freq * t) * 32767.0f * volume * env;
        sample = std::max(-32767.0f, std::min(32767.0f, sample));
        int16_t s = static_cast<int16_t>(sample);
        samples.push_back(s & 0xFF);
        samples.push_back((s >> 8) & 0xFF);
    }
    return samples;
}

// Play a waveform using Win32 Beep or waveOut
void AudioManager::playWave(const std::vector<uint8_t>& samples) {
#ifdef _WIN32
    if (samples.size() < 44) return;

    // We'll use a simple approach - create a WAV in memory and play it
    HWAVEOUT hWaveOut;
    WAVEFORMATEX wfx;
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 1;
    wfx.nSamplesPerSec = 44100;
    wfx.nAvgBytesPerSec = 44100 * 2; // 16-bit mono
    wfx.nBlockAlign = 2;
    wfx.wBitsPerSample = 16;
    wfx.cbSize = 0;

    if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
        return;
    }

    // Allocate and prepare the header
    WAVEHDR whdr;
    std::memset(&whdr, 0, sizeof(whdr));

    // Create a complete WAV with header
    size_t dataSize = samples.size();
    size_t wavSize = 44 + dataSize;
    std::vector<uint8_t> wavData(wavSize);

    // RIFF header
    std::memcpy(&wavData[0], "RIFF", 4);
    uint32_t fileSize = static_cast<uint32_t>(wavSize - 8);
    std::memcpy(&wavData[4], &fileSize, 4);
    std::memcpy(&wavData[8], "WAVE", 4);

    // fmt chunk
    std::memcpy(&wavData[12], "fmt ", 4);
    uint32_t fmtSize = 16;
    std::memcpy(&wavData[16], &fmtSize, 4);
    uint16_t audioFmt = 1; // PCM
    std::memcpy(&wavData[20], &audioFmt, 2);
    uint16_t channels = 1;
    std::memcpy(&wavData[22], &channels, 2);
    uint32_t sampleRate = 44100;
    std::memcpy(&wavData[24], &sampleRate, 4);
    uint32_t byteRate = 44100 * 2;
    std::memcpy(&wavData[28], &byteRate, 4);
    uint16_t blockAlign = 2;
    std::memcpy(&wavData[32], &blockAlign, 2);
    uint16_t bitsPerSample = 16;
    std::memcpy(&wavData[34], &bitsPerSample, 2);

    // data chunk
    std::memcpy(&wavData[36], "data", 4);
    uint32_t dataChunkSize = static_cast<uint32_t>(dataSize);
    std::memcpy(&wavData[40], &dataChunkSize, 4);
    std::memcpy(&wavData[44], samples.data(), dataSize);

    // Allocate fixed memory for the WAV data so the pointer stays valid
    HGLOBAL hg = GlobalAlloc(GMEM_FIXED | GMEM_SHARE, wavSize);
    if (!hg) {
        waveOutClose(hWaveOut);
        return;
    }
    void* pData = static_cast<void*>(hg);
    std::memcpy(pData, wavData.data(), wavSize);

    whdr.lpData = static_cast<LPSTR>(pData);
    whdr.dwBufferLength = static_cast<DWORD>(wavSize);
    whdr.dwFlags = 0;
    whdr.lpNext = nullptr;
    whdr.dwUser = reinterpret_cast<DWORD_PTR>(hg);

    if (waveOutPrepareHeader(hWaveOut, &whdr, sizeof(WAVEHDR)) == MMSYSERR_NOERROR) {
        waveOutWrite(hWaveOut, &whdr, sizeof(WAVEHDR));
    }

    // Wait for playback to finish
    DWORD startTime = GetTickCount();
    while (!(whdr.dwFlags & WHDR_DONE)) {
        if (GetTickCount() - startTime > 500) break;
        Sleep(1);
    }

    waveOutUnprepareHeader(hWaveOut, &whdr, sizeof(WAVEHDR));
    GlobalFree(hg);
    waveOutClose(hWaveOut);
#endif
}

void AudioManager::playJump() {
    auto samples = generateSweep(400, 800, 0.12f, sfxVolume_);
    playWave(samples);
}

void AudioManager::playDoubleJump() {
    auto samples = generateSweep(500, 1000, 0.15f, sfxVolume_);
    playWave(samples);
}

void AudioManager::playCoin() {
    // Two quick tones
    auto s1 = generateTone(1000, 0.05f, sfxVolume_);
    auto s2 = generateTone(1500, 0.08f, sfxVolume_);
    s1.insert(s1.end(), s2.begin(), s2.end());
    playWave(s1);
}

void AudioManager::playDamage() {
    auto samples = generateNoise(0.15f, sfxVolume_);
    playWave(samples);
}

void AudioManager::playPowerUp() {
    auto s1 = generateSweep(400, 1200, 0.2f, sfxVolume_);
    auto s2 = generateSweep(1200, 1600, 0.1f, sfxVolume_);
    s1.insert(s1.end(), s2.begin(), s2.end());
    playWave(s1);
}

void AudioManager::playSlide() {
    auto samples = generateNoise(0.08f, sfxVolume_ * 0.6f);
    playWave(samples);
}

void AudioManager::playGameOver() {
    auto s1 = generateSweep(400, 200, 0.5f, sfxVolume_);
    auto s2 = generateNoise(0.3f, sfxVolume_ * 0.5f);
    s1.insert(s1.end(), s2.begin(), s2.end());
    playWave(s1);
}

void AudioManager::playSelect() {
    // Short high beep
    auto samples = generateTone(800, 0.08f, sfxVolume_, [](float t) { return 1.0f - t; });
    playWave(samples);
}

void AudioManager::playNavigate() {
    // Quick soft tick
    auto samples = generateTone(600, 0.04f, sfxVolume_ * 0.6f, [](float t) { return 1.0f - t * 0.5f; });
    playWave(samples);
}

void AudioManager::setMusicVolume(float vol) {
    musicVolume_ = vol;
}

void AudioManager::setSFXVolume(float vol) {
    sfxVolume_ = vol;
}

void AudioManager::startMusic() {
    // Simple background music thread using tone generation
    musicPlaying_ = true;
    musicThread_ = std::thread([this]() {
        // Simple melody loop
        const float notes[] = {
            262, 294, 330, 349, 392, 349, 330, 294, // C D E F G F E D
            262, 330, 392, 523, 392, 330, 262, 294  // C E G C' G E C D
        };
        const float noteDuration = 0.3f;

        while (musicPlaying_) {
            for (int i = 0; i < 16 && musicPlaying_; i++) {
                float freq = notes[i % 16];
                auto samples = generateTone(freq, noteDuration, musicVolume_ * 0.3f,
                    [](float t) {
                        return 1.0f - t * 0.3f; // slight decay
                    });
                playWave(samples);
            }
        }
    });
}

void AudioManager::stopMusic() {
    musicPlaying_ = false;
    if (musicThread_.joinable()) {
        musicThread_.join();
    }
}

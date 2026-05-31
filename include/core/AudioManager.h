#ifndef CORE_AUDIOMANAGER_H
#define CORE_AUDIOMANAGER_H

#include <cstdint>
#include <vector>
#include <functional>
#include <string>
#include <atomic>
#include <thread>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    void init();
    void playJump();
    void playDoubleJump();
    void playCoin();
    void playDamage();
    void playPowerUp();
    void playSlide();
    void playGameOver();
    void playSelect();
    void playNavigate();
    void setMusicVolume(float vol);
    void setSFXVolume(float vol);

private:
    std::vector<uint8_t> generateTone(float frequency, float duration, float volume,
                                       const std::function<float(float)>& envelope = nullptr);
    std::vector<uint8_t> generateNoise(float duration, float volume);
    std::vector<uint8_t> generateSweep(float startFreq, float endFreq, float duration, float volume);
    void playWave(const std::vector<uint8_t>& samples);
    void startMusic();
    void stopMusic();

    float musicVolume_ = 0.3f;
    float sfxVolume_ = 0.5f;
    std::thread musicThread_;
    std::atomic<bool> musicPlaying_{false};
};

#endif

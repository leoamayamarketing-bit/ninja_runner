#ifndef CORE_CONSTANTS_H
#define CORE_CONSTANTS_H

#include <cstdint>

namespace Constants {
    constexpr int WINDOW_WIDTH = 1280;
    constexpr int WINDOW_HEIGHT = 720;
    constexpr const char* GAME_TITLE = "NinjaRunner: Shadow Dash";
    constexpr int FPS_LIMIT = 60;

    // Physics
    constexpr float GRAVITY = 1800.0f;
    constexpr float GROUND_Y = 450.0f;
    constexpr float BASE_SPEED = 400.0f;
    constexpr float MAX_SPEED = 900.0f;
    constexpr float SPEED_INCREMENT = 2.5f;
    constexpr float SPEED_INTERVAL = 5.0f;

    // Player - Ninja
    constexpr float JUMP_VELOCITY = -650.0f;
    constexpr float DOUBLE_JUMP_VELOCITY = -550.0f;
    constexpr float DASH_SPEED_MULTIPLIER = 2.0f;
    constexpr float DASH_DURATION = 0.3f;
    constexpr float DASH_COOLDOWN = 1.0f;
    constexpr float SLIDE_HEIGHT = 0.4f;

    // Jump feel
    constexpr float COYOTE_TIME = 0.08f;
    constexpr float JUMP_BUFFER_TIME = 0.1f;
    constexpr float JUMP_CUT_MULTIPLIER = 0.4f;
    constexpr float GRAVITY_UP_FACTOR = 0.65f;
    constexpr float GRAVITY_DOWN_FACTOR = 1.2f;

    // Player types
    enum class NinjaType {
        SHADOW,
        BLAZE,
        STORM
    };

    enum class GameStateType {
        MENU,
        PLAYING,
        PAUSED,
        GAME_OVER,
        LEVEL_SELECT
    };

    enum class ObstacleType {
        CHIMNEY,
        AC_UNIT,
        SATELLITE_DISH,
        CLOTHESLINE,
        SIGN,
        PIGEON,
        ROOF_GAP
    };

    enum class PowerUpType {
        SHIELD,
        MAGNET,
        DOUBLE_POINTS,
        SPEED_BOOST,
        EXTRA_LIFE
    };

    enum class LevelType {
        ROOFTOP_DAY,
        ROOFTOP_NIGHT,
        INFINITE
    };

    constexpr int MAX_ENTITIES = 10000;
    constexpr float COIN_SCORE = 100.0f;
    constexpr float DISTANCE_SCORE_MULT = 10.0f;
    constexpr int INITIAL_LIVES = 3;
    constexpr int MAX_LIVES = 5;
    constexpr float POWERUP_DURATION = 8.0f;
    constexpr float MAGNET_RADIUS = 200.0f;
}

#endif

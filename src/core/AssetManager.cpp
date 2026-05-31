#include "../../include/core/AssetManager.h"
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Color.hpp>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <algorithm>

AssetManager::AssetManager() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

sf::Texture& AssetManager::getTexture(const std::string& name) {
    auto it = textures_.find(name);
    if (it != textures_.end()) return it->second;
    static sf::Texture empty;
    return empty;
}

sf::Font& AssetManager::getFont(const std::string& name) {
    auto it = fonts_.find(name);
    if (it != fonts_.end()) return it->second;
    static sf::Font empty;
    return empty;
}

bool AssetManager::hasTexture(const std::string& name) const {
    return textures_.find(name) != textures_.end();
}

bool AssetManager::hasFont(const std::string& name) const {
    return fonts_.find(name) != fonts_.end();
}

sf::Texture AssetManager::createRectTexture(int w, int h, sf::Color color) {
    sf::Image img;
    img.create(w, h, color);
    sf::Texture tex;
    tex.loadFromImage(img);
    return tex;
}

// ======================== NINJA TYPE COLORS ========================
// Pastel tech palette — corporate flat design
static const sf::Color PASTEL_BODY[] = {
    sf::Color(74, 143, 217),    // SHADOW = Corporate blue
    sf::Color(155, 142, 196),   // BLAZE  = Soft violet
    sf::Color(200, 210, 220),   // STORM  = Light gray-blue
};
static const sf::Color PASTEL_ACCENT[] = {
    sf::Color(244, 167, 167),   // SHADOW = Soft coral
    sf::Color(249, 199, 132),   // BLAZE  = Pastel orange
    sf::Color(135, 206, 235),   // STORM  = Pastel sky blue
};
static const sf::Color PASTEL_SCARF[] = {
    sf::Color(155, 142, 196),   // SHADOW = Soft violet
    sf::Color(244, 167, 167),   // BLAZE  = Coral
    sf::Color(200, 215, 235),   // STORM  = Ice blue
};
static const sf::Color OUTLINE_DARK(44, 62, 80);  // Corporate dark blue outline
static const sf::Color SKIN_WARM(245, 225, 200);   // Warm skin tone
static const sf::Color SWORD_METAL(180, 185, 200);

sf::Color AssetManager::getNinjaBodyColor(Constants::NinjaType type) {
    return PASTEL_BODY[static_cast<int>(type)];
}

sf::Color AssetManager::getNinjaAccentColor(Constants::NinjaType type) {
    return PASTEL_ACCENT[static_cast<int>(type)];
}

sf::Color AssetManager::getNinjaAccent2Color(Constants::NinjaType type) {
    return PASTEL_SCARF[static_cast<int>(type)];
}

std::string AssetManager::getTypeSuffix(Constants::NinjaType type) {
    switch (type) {
        case Constants::NinjaType::SHADOW: return "_shadow";
        case Constants::NinjaType::BLAZE:  return "_blaze";
        case Constants::NinjaType::STORM:  return "_storm";
    }
    return "_shadow";
}

bool AssetManager::loadTextureFromFile(const std::string& texName, const std::string& filePath) {
    sf::Texture tex;
    if (!tex.loadFromFile(filePath)) return false;
    textures_[texName] = tex;
    return true;
}

void AssetManager::loadPngAssets() {
    // Try multiple possible paths for the assets directory
    const char* paths[] = {
        "../assets/",   // when running from build/
        "assets/",      // when running from project root
        "./assets/",    // explicit relative
    };

    auto loadPng = [&](const std::string& texName, const std::string& fileName) -> bool {
        for (const char* base : paths) {
            std::string fullPath = std::string(base) + fileName;
            if (loadTextureFromFile(texName, fullPath)) {
                return true;
            }
        }
        return false;
    };

    // --- Ninja run frames (blue ninja) ---
    // Map to SHADOW type textures
    const char* runFiles[] = {
        "ninja_corre_00.png", "ninja_corre_01.png", "ninja_corre_02.png", "ninja_corre_03.png",
        "ninja_corre_04.png", "ninja_corre_05.png", "ninja_corre_06.png", "ninja_corre_07.png"
    };
    for (int i = 0; i < 8; i++) {
        char name[32];
        std::snprintf(name, sizeof(name), "ninja_run_%d_shadow", i);
        loadPng(name, runFiles[i]);
    }

    // Ninja jump (standing pose)
    loadPng("ninja_jump_shadow", "ninja_parado_00.png");

    // Ninja slide (hidden/crouched pose)
    loadPng("ninja_slide_shadow", "ninja_escondido_00.png");

    // --- Coin / Shuriken (2-frame animation) ---
    loadPng("coin_0", "estrella_ninja_00.png");
    loadPng("coin_1", "estrella_ninja_01.png");

    // --- Treasure / Powerup ---
    loadPng("treasure", "tesoro_00.png");

    // --- Obstacles ---
    loadPng("obstacle_0", "osbstaculo_00.png");
    loadPng("obstacle_1", "osbstaculo_01.png");
    loadPng("obstacle_2", "osbstaculo_02.png");

    // --- Other decorative assets ---
    loadPng("flags", "banderas_00.png");
    loadPng("ladder", "escalera_00.png");
    loadPng("accessory", "accesorio_00.png");
}

void AssetManager::generateAllAssets() {
    // First, generate all procedural textures
    generateFont();
    generateNinjaRunTextures();
    generateNinjaJumpTexture();
    generateNinjaSlideTexture();
    generateObstacleTextures();
    generateBuildingTextures();
    generateUITextures();

    // Then load PNG assets — they overwrite procedural textures for SHADOW type
    loadPngAssets();
}

// ======================== FONT ========================
void AssetManager::generateFont() {
    sf::Font font;
    fonts_["default"] = font;
}

// Helper: render a ninja on a RenderTexture with vector shapes
static sf::Texture renderNinjaVector(int texW, int texH,
    const sf::Color& bodyColor, const sf::Color& accentColor,
    const sf::Color& scarfColor, float phase, bool isSlide, bool isJump) {

    sf::RenderTexture rt;
    rt.create(texW, texH);
    rt.clear(sf::Color::Transparent);

    float legSw = isSlide ? 0.0f : 4.0f * std::sin(phase);
    float armSw = isSlide ? 0.0f : 3.0f * std::sin(phase + 1.0f);
    float bodyBob = isSlide || isJump ? 0.0f : 1.5f * std::sin(phase * 2.0f);

    // Shadow under feet (soft pastel shadow)
    sf::CircleShape footShadow(2.0f);
    footShadow.setFillColor(sf::Color(44, 62, 80, 25));
    footShadow.setPosition(17.0f, 51.0f);
    rt.draw(footShadow);
    footShadow.setPosition(28.0f, 51.0f);
    rt.draw(footShadow);

    // Sword on back (behind body)
    sf::RectangleShape sword(sf::Vector2f(2.5f, 18.0f));
    sword.setFillColor(SWORD_METAL);
    sword.setPosition(37.0f, 12.0f + bodyBob);
    rt.draw(sword);
    sf::CircleShape swordTip(1.5f);
    swordTip.setFillColor(sf::Color::White);
    swordTip.setPosition(37.5f, 9.0f + bodyBob);
    rt.draw(swordTip);
    sf::RectangleShape guard(sf::Vector2f(5.0f, 2.0f));
    guard.setFillColor(sf::Color(200, 170, 100));
    guard.setPosition(36.0f, 30.0f + bodyBob);
    rt.draw(guard);

    // Scarf trail behind neck
    for (int i = 0; i < 4; i++) {
        float sx = 11.0f - i * 2.5f + std::sin(phase + i * 0.5f) * 2.0f;
        float sy = 11.0f + bodyBob + i * 2.5f;
        sf::CircleShape dot(2.0f);
        sf::Color sc = scarfColor;
        sc.a = static_cast<sf::Uint8>(180 - i * 20);
        dot.setFillColor(sc);
        dot.setPosition(sx, sy);
        rt.draw(dot);
    }
    sf::RectangleShape scarf(sf::Vector2f(10.0f, 4.0f));
    scarf.setFillColor(scarfColor);
    scarf.setPosition(12.0f, 11.0f + bodyBob);
    rt.draw(scarf);

    // Legs
    float legY = isJump ? 36.0f : isSlide ? 20.0f : 34.0f;
    float lLegX = isSlide ? 30.0f : 18.0f + legSw;
    float rLegX = isSlide ? 14.0f : 27.0f - legSw;

    sf::RectangleShape lLeg(sf::Vector2f(5.0f, isSlide ? 5.0f : 10.0f));
    lLeg.setFillColor(bodyColor);
    lLeg.setPosition(lLegX, legY + bodyBob);
    rt.draw(lLeg);
    sf::RectangleShape rLeg(sf::Vector2f(5.0f, isSlide ? 5.0f : 10.0f));
    rLeg.setFillColor(bodyColor);
    rLeg.setPosition(rLegX, legY + bodyBob);
    rt.draw(rLeg);

    // Feet
    float footY = isSlide ? 25.0f : 44.0f;
    sf::RectangleShape lFoot(sf::Vector2f(7.0f, 3.0f));
    lFoot.setFillColor(OUTLINE_DARK);
    lFoot.setPosition(17.0f + legSw, footY + bodyBob);
    rt.draw(lFoot);
    sf::RectangleShape rFoot(sf::Vector2f(7.0f, 3.0f));
    rFoot.setFillColor(OUTLINE_DARK);
    rFoot.setPosition(26.0f - legSw, footY + bodyBob);
    rt.draw(rFoot);

    // Body (rounded rectangle: rect + corner circles)
    float bodyX = isSlide ? 14.0f : 16.0f;
    float bodyW = isSlide ? 20.0f : 18.0f;
    float bodyH = isSlide ? 12.0f : 18.0f;
    float bodyY = isSlide ? 14.0f : isJump ? 20.0f : 16.0f;
    float cornerR = 4.0f;

    sf::RectangleShape body(sf::Vector2f(bodyW - cornerR * 2, bodyH - cornerR * 2));
    body.setFillColor(bodyColor);
    body.setPosition(bodyX + cornerR, bodyY + cornerR + bodyBob);
    rt.draw(body);

    for (float cx : {bodyX + cornerR, bodyX + bodyW - cornerR}) {
        for (float cy : {bodyY + cornerR + bodyBob, bodyY + bodyH - cornerR + bodyBob}) {
            sf::CircleShape corner(cornerR);
            corner.setFillColor(bodyColor);
            corner.setPosition(cx - cornerR, cy - cornerR);
            rt.draw(corner);
        }
    }

    // Outline around body (thin)
    sf::RectangleShape bodyOutline(sf::Vector2f(bodyW, bodyH));
    bodyOutline.setFillColor(sf::Color::Transparent);
    bodyOutline.setOutlineColor(OUTLINE_DARK);
    bodyOutline.setOutlineThickness(1.5f);
    bodyOutline.setPosition(bodyX, bodyY + bodyBob);
    rt.draw(bodyOutline);

    // Belt
    if (!isSlide) {
        sf::RectangleShape belt(sf::Vector2f(bodyW - 2.0f, 2.5f));
        belt.setFillColor(accentColor);
        belt.setPosition(bodyX + 1.0f, bodyY + bodyH * 0.65f + bodyBob);
        rt.draw(belt);
    }

    // Arms
    float armY = isSlide ? 16.0f : isJump ? 22.0f : 18.0f;
    if (isJump) {
        // Arms up for jump
        sf::RectangleShape lArm(sf::Vector2f(4.0f, 8.0f));
        lArm.setFillColor(bodyColor);
        lArm.setPosition(12.0f, 14.0f + bodyBob);
        lArm.setRotation(-30.0f);
        rt.draw(lArm);
        sf::RectangleShape rArm(sf::Vector2f(4.0f, 8.0f));
        rArm.setFillColor(bodyColor);
        rArm.setPosition(34.0f, 14.0f + bodyBob);
        rArm.setRotation(30.0f);
        rt.draw(rArm);
    } else if (isSlide) {
        // Arms back for slide
        sf::RectangleShape lArm(sf::Vector2f(4.0f, 6.0f));
        lArm.setFillColor(bodyColor);
        lArm.setPosition(10.0f, 18.0f + bodyBob);
        rt.draw(lArm);
    } else {
        // Arms swinging for run
        sf::RectangleShape lArm(sf::Vector2f(4.0f, 9.0f));
        lArm.setFillColor(bodyColor);
        lArm.setPosition(12.0f + armSw, armY + bodyBob);
        rt.draw(lArm);
        sf::RectangleShape rArm(sf::Vector2f(4.0f, 9.0f));
        rArm.setFillColor(bodyColor);
        rArm.setPosition(34.0f - armSw, armY + bodyBob);
        rt.draw(rArm);

        // Hands
        sf::CircleShape hand(2.0f);
        hand.setFillColor(SKIN_WARM);
        hand.setPosition(11.0f + armSw, armY + 7.0f + bodyBob);
        rt.draw(hand);
        hand.setPosition(33.0f - armSw, armY + 7.0f + bodyBob);
        rt.draw(hand);
    }

    // Head (big circle — cartoon friendly)
    float headX = isSlide ? 28.0f : 16.0f;
    float headY = isSlide ? 4.0f : 1.0f;
    float headR = isSlide ? 9.0f : 13.0f;

    sf::CircleShape head(headR);
    head.setFillColor(SKIN_WARM);
    head.setPosition(headX, headY + bodyBob);
    rt.draw(head);

    // Head outline
    sf::CircleShape headOutline(headR);
    headOutline.setFillColor(sf::Color::Transparent);
    headOutline.setOutlineColor(OUTLINE_DARK);
    headOutline.setOutlineThickness(1.5f);
    headOutline.setPosition(headX, headY + bodyBob);
    rt.draw(headOutline);

    // Hair (messy bun on top)
    sf::CircleShape hair(5.0f);
    hair.setFillColor(OUTLINE_DARK);
    hair.setPosition(headX + headR * 0.6f, headY - 4.0f + bodyBob);
    rt.draw(hair);

    // Headband
    sf::RectangleShape band(sf::Vector2f(headR * 2.2f, 4.0f));
    band.setFillColor(accentColor);
    band.setPosition(headX - 1.0f, headY + headR * 0.4f + bodyBob);
    rt.draw(band);

    // Headband tails
    sf::RectangleShape tail1(sf::Vector2f(6.0f, 2.5f));
    tail1.setFillColor(accentColor);
    tail1.setPosition(headX + headR * 2.0f - 4.0f, headY + headR * 0.3f + bodyBob);
    tail1.setRotation(35.0f);
    rt.draw(tail1);
    sf::RectangleShape tail2(sf::Vector2f(5.0f, 2.0f));
    tail2.setFillColor(accentColor);
    tail2.setPosition(headX + headR * 2.0f - 2.0f, headY + headR * 0.6f + bodyBob);
    tail2.setRotation(-20.0f);
    rt.draw(tail2);

    // Eyes (big, friendly — white with small pupils)
    float eyeY = headY + headR * 0.5f + bodyBob;
    float eyeSpacing = headR * 0.5f;
    float eyeCenterX = headX + headR;

    // Left eye
    sf::CircleShape eyeWhite(4.5f);
    eyeWhite.setFillColor(sf::Color::White);
    eyeWhite.setOutlineColor(OUTLINE_DARK);
    eyeWhite.setOutlineThickness(0.8f);
    eyeWhite.setPosition(eyeCenterX - eyeSpacing - 4.0f, eyeY - 4.0f);
    rt.draw(eyeWhite);
    sf::CircleShape leftPupil(2.0f);
    leftPupil.setFillColor(OUTLINE_DARK);
    leftPupil.setPosition(eyeCenterX - eyeSpacing - 2.0f, eyeY - 2.0f);
    rt.draw(leftPupil);
    // Eye shine
    sf::CircleShape shine1(0.8f);
    shine1.setFillColor(sf::Color::White);
    shine1.setPosition(eyeCenterX - eyeSpacing - 3.0f, eyeY - 3.5f);
    rt.draw(shine1);

    // Right eye
    sf::CircleShape eyeWhite2(4.5f);
    eyeWhite2.setFillColor(sf::Color::White);
    eyeWhite2.setOutlineColor(OUTLINE_DARK);
    eyeWhite2.setOutlineThickness(0.8f);
    eyeWhite2.setPosition(eyeCenterX + eyeSpacing - 4.5f, eyeY - 4.0f);
    rt.draw(eyeWhite2);
    sf::CircleShape rightPupil(2.0f);
    rightPupil.setFillColor(OUTLINE_DARK);
    rightPupil.setPosition(eyeCenterX + eyeSpacing - 2.5f, eyeY - 2.0f);
    rt.draw(rightPupil);
    sf::CircleShape shine2(0.8f);
    shine2.setFillColor(sf::Color::White);
    shine2.setPosition(eyeCenterX + eyeSpacing - 3.5f, eyeY - 3.5f);
    rt.draw(shine2);

    // Cute mouth (small arc or dot)
    sf::RectangleShape mouth(sf::Vector2f(3.0f, 1.0f));
    mouth.setFillColor(OUTLINE_DARK);
    mouth.setPosition(eyeCenterX - 1.5f, eyeY + 5.0f);
    rt.draw(mouth);

    // Rosy cheeks
    sf::CircleShape cheek(3.0f);
    cheek.setFillColor(sf::Color(255, 180, 180, 60));
    cheek.setPosition(eyeCenterX - eyeSpacing - 2.0f, eyeY + 1.0f);
    rt.draw(cheek);
    cheek.setPosition(eyeCenterX + eyeSpacing - 1.0f, eyeY + 1.0f);
    rt.draw(cheek);

    rt.display();
    sf::Image img = rt.getTexture().copyToImage();
    sf::Texture tex;
    tex.loadFromImage(img);
    return tex;
}

// ======================== NINJA SPRITES (vector flat design) ========================
sf::Texture AssetManager::createNinjaFrame(int index, Constants::NinjaType type) {
    float phase = index * 3.14159f * 2.0f / 8.0f;
    return renderNinjaVector(50, 60,
        getNinjaBodyColor(type),
        getNinjaAccentColor(type),
        getNinjaAccent2Color(type),
        phase, false, false);
}

sf::Texture AssetManager::createChimneyTexture() {
    sf::RenderTexture rt;
    rt.create(30, 50);
    rt.clear(sf::Color::Transparent);

    // Flat design chimney — rounded masonry style
    sf::Color brickMain(120, 70, 55);
    sf::Color brickLight(140, 85, 65);
    sf::Color mortarColor(90, 70, 60);
    sf::Color capColor(100, 65, 50);
    sf::Color outlineColor(44, 62, 80);

    // Main body (rounded rect with corner radius)
    float bw = 26.0f, bh = 42.0f;
    float bx = 2.0f, by = 6.0f;
    float cr = 3.0f;

    // Body
    sf::RectangleShape body(sf::Vector2f(bw - cr * 2, bh - cr * 2));
    body.setFillColor(brickMain);
    body.setPosition(bx + cr, by + cr);
    rt.draw(body);
    for (float cx : {bx + cr, bx + bw - cr})
        for (float cy : {by + cr, by + bh - cr}) {
            sf::CircleShape c(cr); c.setFillColor(brickMain); c.setPosition(cx - cr, cy - cr); rt.draw(c);
        }
    // Outline
    sf::RectangleShape bodyOut(sf::Vector2f(bw, bh));
    bodyOut.setFillColor(sf::Color::Transparent);
    bodyOut.setOutlineColor(outlineColor);
    bodyOut.setOutlineThickness(1.5f);
    bodyOut.setPosition(bx, by);
    rt.draw(bodyOut);

    // Brick pattern (horizontal lines)
    for (int i = 1; i < 4; i++) {
        sf::RectangleShape mortar(sf::Vector2f(bw - 4, 1.5f));
        mortar.setFillColor(mortarColor);
        mortar.setPosition(bx + 2, by + i * 9.0f);
        rt.draw(mortar);
    }

    // Cap / rim at top
    sf::RectangleShape cap(sf::Vector2f(bw + 4, 5));
    cap.setFillColor(capColor);
    cap.setOutlineColor(outlineColor);
    cap.setOutlineThickness(1.5f);
    cap.setPosition(bx - 2, by - 2);
    rt.draw(cap);

    rt.display();
    sf::Image img = rt.getTexture().copyToImage();
    sf::Texture tex;
    tex.loadFromImage(img);
    return tex;
}

sf::Texture AssetManager::createACUnitTexture() {
    sf::RenderTexture rt;
    rt.create(40, 25);
    rt.clear(sf::Color::Transparent);

    sf::Color bodyColor(210, 215, 220);
    sf::Color ventColor(140, 145, 155);
    sf::Color fanColor(80, 85, 95);
    sf::Color outlineColor(44, 62, 80);
    float cr = 3.0f;

    // Body with rounded corners
    float bw = 36.0f, bh = 22.0f, bx = 2.0f, by = 2.0f;
    sf::RectangleShape body(sf::Vector2f(bw - cr * 2, bh - cr * 2));
    body.setFillColor(bodyColor);
    body.setPosition(bx + cr, by + cr);
    rt.draw(body);
    for (float cx : {bx + cr, bx + bw - cr})
        for (float cy : {by + cr, by + bh - cr}) {
            sf::CircleShape c(cr); c.setFillColor(bodyColor); c.setPosition(cx - cr, cy - cr); rt.draw(c);
        }
    sf::RectangleShape bodyOut(sf::Vector2f(bw, bh));
    bodyOut.setFillColor(sf::Color::Transparent);
    bodyOut.setOutlineColor(outlineColor);
    bodyOut.setOutlineThickness(1.5f);
    bodyOut.setPosition(bx, by);
    rt.draw(bodyOut);

    // Ventilation grille (vertical bars)
    for (int i = 0; i < 7; i++) {
        sf::RectangleShape bar(sf::Vector2f(2.5f, 12));
        bar.setFillColor(ventColor);
        bar.setOutlineColor(outlineColor);
        bar.setOutlineThickness(0.5f);
        bar.setPosition(6.0f + i * 4.5f, 6.0f);
        rt.draw(bar);
    }

    // Fan circle on the right
    sf::CircleShape fanBg(6);
    fanBg.setFillColor(sf::Color(160, 165, 175));
    fanBg.setOutlineColor(outlineColor);
    fanBg.setOutlineThickness(1.0f);
    fanBg.setPosition(26, 7);
    rt.draw(fanBg);

    sf::CircleShape fanCenter(2.5f);
    fanCenter.setFillColor(fanColor);
    fanCenter.setPosition(30, 11);
    rt.draw(fanCenter);

    for (int i = 0; i < 4; i++) {
        float ang = i * 3.14159f / 2.0f;
        sf::RectangleShape blade(sf::Vector2f(4, 1.5f));
        blade.setFillColor(fanColor);
        blade.setPosition(29.0f + 3.5f * std::cos(ang), 11.0f + 3.5f * std::sin(ang));
        blade.setRotation(i * 90.0f);
        rt.draw(blade);
    }

    rt.display();
    sf::Image img = rt.getTexture().copyToImage();
    sf::Texture tex;
    tex.loadFromImage(img);
    return tex;
}

sf::Texture AssetManager::createSatelliteDishTexture() {
    sf::RenderTexture rt;
    rt.create(24, 35);
    rt.clear(sf::Color::Transparent);

    sf::Color poleColor(140, 145, 155);
    sf::Color dishColor(180, 185, 195);
    sf::Color receiverColor(200, 80, 80);
    sf::Color outlineColor(44, 62, 80);

    // Pole
    sf::RectangleShape pole(sf::Vector2f(3, 14));
    pole.setFillColor(poleColor);
    pole.setOutlineColor(outlineColor);
    pole.setOutlineThickness(1.0f);
    pole.setPosition(10.5f, 20.0f);
    rt.draw(pole);

    // Dish (arc shape using ConvexShape)
    int segments = 12;
    sf::ConvexShape dish;
    dish.setPointCount(segments);
    for (int i = 0; i < segments; i++) {
        float t = static_cast<float>(i) / (segments - 1);
        float angle = t * 3.14159f; // 0 to PI (semi-circle)
        float r = 10.0f;
        float dx = r * std::cos(angle);
        float dy = r * std::sin(angle);
        dish.setPoint(i, sf::Vector2f(12.0f + dx, 4.0f + dy));
    }
    dish.setFillColor(dishColor);
    dish.setOutlineColor(outlineColor);
    dish.setOutlineThickness(1.0f);
    rt.draw(dish);

    // Receiver arm
    sf::RectangleShape arm(sf::Vector2f(1.5f, 6));
    arm.setFillColor(poleColor);
    arm.setPosition(17.5f, 8.0f);
    arm.setRotation(-20.0f);
    rt.draw(arm);

    // Receiver tip
    sf::CircleShape tip(2);
    tip.setFillColor(receiverColor);
    tip.setOutlineColor(outlineColor);
    tip.setOutlineThickness(0.8f);
    tip.setPosition(19, 4);
    rt.draw(tip);

    rt.display();
    sf::Image img = rt.getTexture().copyToImage();
    sf::Texture tex;
    tex.loadFromImage(img);
    return tex;
}

sf::Texture AssetManager::createClotheslineTexture() {
    sf::RenderTexture rt;
    rt.create(60, 10);
    rt.clear(sf::Color::Transparent);

    sf::Color lineColor(100, 105, 115);
    sf::Color outlineColor(44, 62, 80);
    sf::Color clothColors[] = {
        sf::Color(244, 167, 167),  // Soft coral
        sf::Color(135, 206, 235),  // Sky blue
        sf::Color(249, 199, 132),  // Pastel orange
        sf::Color(155, 195, 170),  // Sage
    };

    // Line
    sf::RectangleShape line(sf::Vector2f(60, 1.5f));
    line.setFillColor(lineColor);
    line.setPosition(0, 2);
    rt.draw(line);

    // Clothes (flat rounded rectangles)
    for (int i = 0; i < 4; i++) {
        float cx = 6.0f + i * 14.0f;
        sf::Color col = clothColors[i];

        // Shape with rounded corners
        sf::RectangleShape cloth(sf::Vector2f(8, 6));
        cloth.setFillColor(col);
        cloth.setOutlineColor(outlineColor);
        cloth.setOutlineThickness(0.8f);
        cloth.setPosition(cx - 4, 3);
        rt.draw(cloth);

        // Clothespin
        sf::RectangleShape pin(sf::Vector2f(2, 2));
        pin.setFillColor(outlineColor);
        pin.setPosition(cx - 1, 1);
        rt.draw(pin);
    }

    rt.display();
    sf::Image img = rt.getTexture().copyToImage();
    sf::Texture tex;
    tex.loadFromImage(img);
    return tex;
}

sf::Texture AssetManager::createSignTexture() {
    sf::RenderTexture rt;
    rt.create(30, 30);
    rt.clear(sf::Color::Transparent);

    sf::Color poleColor(140, 145, 155);
    sf::Color signColor(249, 199, 132);  // Pastel orange
    sf::Color signEdge(220, 170, 100);
    sf::Color outlineColor(44, 62, 80);
    sf::Color arrowColor(60, 50, 40);

    // Pole
    sf::RectangleShape pole(sf::Vector2f(3, 12));
    pole.setFillColor(poleColor);
    pole.setOutlineColor(outlineColor);
    pole.setOutlineThickness(1.0f);
    pole.setPosition(13.5f, 18.0f);
    rt.draw(pole);

    // Sign board (rounded rect)
    float bw = 22, bh = 15, bx = 4, by = 3, cr = 3;
    sf::RectangleShape board(sf::Vector2f(bw - cr * 2, bh - cr * 2));
    board.setFillColor(signColor);
    board.setPosition(bx + cr, by + cr);
    rt.draw(board);
    for (float cx : {bx + cr, bx + bw - cr})
        for (float cy : {by + cr, by + bh - cr}) {
            sf::CircleShape c(cr); c.setFillColor(signColor); c.setPosition(cx - cr, cy - cr); rt.draw(c);
        }
    sf::RectangleShape boardOut(sf::Vector2f(bw, bh));
    boardOut.setFillColor(sf::Color::Transparent);
    boardOut.setOutlineColor(outlineColor);
    boardOut.setOutlineThickness(1.5f);
    boardOut.setPosition(bx, by);
    rt.draw(boardOut);

    // Arrow icon (triangle pointing up)
    sf::ConvexShape arrow;
    arrow.setPointCount(3);
    arrow.setPoint(0, sf::Vector2f(15, 6));
    arrow.setPoint(1, sf::Vector2f(11, 12));
    arrow.setPoint(2, sf::Vector2f(19, 12));
    arrow.setFillColor(arrowColor);
    rt.draw(arrow);

    rt.display();
    sf::Image img = rt.getTexture().copyToImage();
    sf::Texture tex;
    tex.loadFromImage(img);
    return tex;
}

sf::Texture AssetManager::createPigeonTexture(int frame) {
    sf::RenderTexture rt;
    rt.create(30, 20);
    rt.clear(sf::Color::Transparent);

    sf::Color bodyColor(170, 175, 185);
    sf::Color darkColor(120, 125, 135);
    sf::Color eyeWhite(255, 255, 240);
    sf::Color beakColor(230, 185, 70);
    sf::Color outlineColor(44, 62, 80);

    // Body (ellipse via scaled circle)
    sf::CircleShape body(7);
    body.setFillColor(bodyColor);
    body.setOutlineColor(outlineColor);
    body.setOutlineThickness(1.0f);
    body.setPosition(8, 6);
    body.setScale(1.2f, 0.9f);
    rt.draw(body);

    // Head (circle)
    sf::CircleShape head(4.5f);
    head.setFillColor(darkColor);
    head.setOutlineColor(outlineColor);
    head.setOutlineThickness(0.8f);
    head.setPosition(18, 2);
    rt.draw(head);

    // Eye
    sf::CircleShape eye(1.5f);
    eye.setFillColor(eyeWhite);
    eye.setPosition(20.5f, 3);
    rt.draw(eye);
    sf::CircleShape pupil(0.8f);
    pupil.setFillColor(outlineColor);
    pupil.setPosition(21, 3.5f);
    rt.draw(pupil);

    // Beak
    sf::ConvexShape beak;
    beak.setPointCount(3);
    beak.setPoint(0, sf::Vector2f(23, 4));
    beak.setPoint(1, sf::Vector2f(27, 5));
    beak.setPoint(2, sf::Vector2f(23, 6));
    beak.setFillColor(beakColor);
    beak.setOutlineColor(outlineColor);
    beak.setOutlineThickness(0.5f);
    rt.draw(beak);

    // Wing - animated between frames
    if (frame == 0) {
        // Wings down (resting)
        sf::CircleShape wing(4);
        wing.setFillColor(darkColor);
        wing.setOutlineColor(outlineColor);
        wing.setOutlineThickness(0.8f);
        wing.setScale(1.0f, 0.7f);
        wing.setPosition(11, 8);
        rt.draw(wing);
    } else {
        // Wings up (flapping)
        sf::ConvexShape wing;
        wing.setPointCount(4);
        wing.setPoint(0, sf::Vector2f(12, 9));
        wing.setPoint(1, sf::Vector2f(9, 2));
        wing.setPoint(2, sf::Vector2f(14, 2));
        wing.setPoint(3, sf::Vector2f(14, 9));
        wing.setFillColor(darkColor);
        wing.setOutlineColor(outlineColor);
        wing.setOutlineThickness(0.8f);
        rt.draw(wing);
        // Second wing tip
        sf::ConvexShape wing2;
        wing2.setPointCount(3);
        wing2.setPoint(0, sf::Vector2f(10, 2));
        wing2.setPoint(1, sf::Vector2f(7, 0));
        wing2.setPoint(2, sf::Vector2f(11, 5));
        wing2.setFillColor(darkColor);
        wing2.setOutlineColor(outlineColor);
        wing2.setOutlineThickness(0.5f);
        rt.draw(wing2);
    }

    // Tail
    sf::ConvexShape tail;
    tail.setPointCount(3);
    tail.setPoint(0, sf::Vector2f(6, 10));
    tail.setPoint(1, sf::Vector2f(3, 8));
    tail.setPoint(2, sf::Vector2f(5, 13));
    tail.setFillColor(darkColor);
    tail.setOutlineColor(outlineColor);
    tail.setOutlineThickness(0.5f);
    rt.draw(tail);

    // Feet (tiny)
    sf::RectangleShape foot(sf::Vector2f(3, 1.5f));
    foot.setFillColor(outlineColor);
    foot.setPosition(14, 17);
    rt.draw(foot);
    foot.setPosition(18, 17);
    rt.draw(foot);

    rt.display();
    sf::Image img = rt.getTexture().copyToImage();
    sf::Texture tex;
    tex.loadFromImage(img);
    return tex;
}

sf::Texture AssetManager::createShurikenTexture() {
    sf::RenderTexture rt;
    rt.create(24, 24);
    rt.clear(sf::Color::Transparent);

    sf::Color goldCore(255, 215, 60);
    sf::Color goldEdge(255, 235, 150);
    sf::Color outlineColor(44, 62, 80);

    // Gold coin body (circle with soft shadow)
    sf::CircleShape shadow(10);
    shadow.setFillColor(sf::Color(44, 62, 80, 25));
    shadow.setPosition(2, 3);
    rt.draw(shadow);

    sf::CircleShape coin(10);
    coin.setFillColor(goldCore);
    coin.setOutlineColor(outlineColor);
    coin.setOutlineThickness(1.5f);
    coin.setPosition(2, 2);
    rt.draw(coin);

    // Inner ring (lighter)
    sf::CircleShape inner(7);
    inner.setFillColor(goldEdge);
    inner.setOutlineColor(outlineColor);
    inner.setOutlineThickness(0.8f);
    inner.setPosition(5, 5);
    rt.draw(inner);

    // Star shuriken in center (4-pointed star via ConvexShape)
    sf::ConvexShape star;
    star.setPointCount(8);
    float cx = 12, cy = 12;
    for (int i = 0; i < 8; i++) {
        float angle = i * 3.14159f / 4.0f - 3.14159f / 4.0f;
        float r = (i % 2 == 0) ? 7.0f : 2.5f;
        star.setPoint(i, sf::Vector2f(cx + r * std::cos(angle), cy + r * std::sin(angle)));
    }
    star.setFillColor(sf::Color::White);
    star.setOutlineColor(outlineColor);
    star.setOutlineThickness(0.5f);
    rt.draw(star);

    // Center dot
    sf::CircleShape dot(1.5f);
    dot.setFillColor(outlineColor);
    dot.setPosition(10.5, 10.5);
    rt.draw(dot);

    rt.display();
    sf::Image img = rt.getTexture().copyToImage();
    sf::Texture tex;
    tex.loadFromImage(img);
    return tex;
}

// ======================== BUILDING TEXTURES ========================
sf::Texture AssetManager::createBuildingTexture(int height, sf::Color color) {
    int w = 200;
    sf::Image img;
    img.create(w, height, sf::Color::Transparent);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < w; x++) {
            int shade = (color.r + (std::rand() % 10 - 5));
            shade = std::max(0, std::min(255, shade));
            img.setPixel(x, y, sf::Color(shade, shade + 5, shade + 15));
        }
    }

    for (int row = 0; row < height / 30; row++) {
        for (int col = 0; col < 5; col++) {
            int wx = 20 + col * 38;
            int wy = 15 + row * 30;
            bool lit = (std::rand() % 3) > 0;
            sf::Color winColor = lit ? sf::Color(255, 230, 150) : sf::Color(30, 40, 60);
            for (int wy2 = wy; wy2 < wy + 14 && wy2 < height; wy2++) {
                for (int wx2 = wx; wx2 < wx + 10 && wx2 < w; wx2++) {
                    img.setPixel(wx2, wy2, winColor);
                }
            }
        }
    }

    sf::Texture tex;
    tex.loadFromImage(img);
    return tex;
}

sf::Texture AssetManager::createCityBackgroundTexture(int w, int h) {
    sf::RenderTexture rt;
    rt.create(w, h);
    rt.clear(sf::Color::Transparent);

    // Flat design palette for far background city
    static const sf::Color FAR_COLORS[] = {
        sf::Color(160, 180, 200, 180),
        sf::Color(140, 165, 195, 180),
        sf::Color(170, 185, 205, 180),
        sf::Color(150, 170, 190, 180),
    };
    static const sf::Color FAR_OUTLINE(100, 120, 150, 100);

    int x = 0;
    while (x < w) {
        int buildW = 60 + (std::rand() % 100);
        int buildH = 60 + (std::rand() % (h - 80));
        sf::Color col = FAR_COLORS[std::rand() % 4];

        sf::RectangleShape building(sf::Vector2f(static_cast<float>(buildW), static_cast<float>(buildH)));
        building.setFillColor(col);
        building.setOutlineColor(FAR_OUTLINE);
        building.setOutlineThickness(1.0f);
        building.setPosition(static_cast<float>(x), static_cast<float>(h - buildH));
        rt.draw(building);

        // Flat roofline
        sf::RectangleShape roof(sf::Vector2f(static_cast<float>(buildW), 4.0f));
        roof.setFillColor(sf::Color(120, 140, 170, 180));
        roof.setPosition(static_cast<float>(x), static_cast<float>(h - buildH - 4));
        rt.draw(roof);

        // Simple windows (clean colored dots)
        for (int wx = 0; wx < buildW - 20; wx += 18) {
            for (int wy = 0; wy < buildH - 20; wy += 18) {
                if (std::rand() % 3 > 0) {
                    sf::RectangleShape win(sf::Vector2f(6, 8));
                    win.setFillColor(sf::Color(255, 225, 150, 120));
                    win.setPosition(static_cast<float>(x + 10 + wx), static_cast<float>(h - buildH + 10 + wy));
                    rt.draw(win);
                }
            }
        }

        x += buildW;
    }

    rt.display();
    sf::Image img = rt.getTexture().copyToImage();
    sf::Texture tex;
    tex.loadFromImage(img);
    return tex;
}

sf::Texture AssetManager::createNightCityBackgroundTexture(int w, int h) {
    sf::RenderTexture rt;
    rt.create(w, h);
    rt.clear(sf::Color::Transparent);

    // Flat design night palette
    static const sf::Color FAR_NIGHT_COLORS[] = {
        sf::Color(30, 28, 50, 200),
        sf::Color(25, 25, 45, 200),
        sf::Color(35, 30, 48, 200),
        sf::Color(28, 26, 42, 200),
    };
    static const sf::Color FAR_NIGHT_OUTLINE(15, 12, 25, 100);

    int x = 0;
    while (x < w) {
        int buildW = 50 + (std::rand() % 80);
        int buildH = 40 + (std::rand() % (h - 60));
        sf::Color col = FAR_NIGHT_COLORS[std::rand() % 4];

        sf::RectangleShape building(sf::Vector2f(static_cast<float>(buildW), static_cast<float>(buildH)));
        building.setFillColor(col);
        building.setOutlineColor(FAR_NIGHT_OUTLINE);
        building.setOutlineThickness(1.0f);
        building.setPosition(static_cast<float>(x), static_cast<float>(h - buildH));
        rt.draw(building);

        // Roof
        sf::RectangleShape roof(sf::Vector2f(static_cast<float>(buildW), 4.0f));
        roof.setFillColor(sf::Color(20, 18, 35, 200));
        roof.setPosition(static_cast<float>(x), static_cast<float>(h - buildH - 4));
        rt.draw(roof);

        // Warm-lit windows
        for (int wx = 0; wx < buildW - 20; wx += 18) {
            for (int wy = 0; wy < buildH - 20; wy += 18) {
                if (std::rand() % 2 > 0) {
                    sf::RectangleShape win(sf::Vector2f(6, 8));
                    win.setFillColor(sf::Color(255, 200, 80, 180));
                    win.setPosition(static_cast<float>(x + 10 + wx), static_cast<float>(h - buildH + 10 + wy));
                    rt.draw(win);
                }
            }
        }

        x += buildW;
    }

    rt.display();
    sf::Image img = rt.getTexture().copyToImage();
    sf::Texture tex;
    tex.loadFromImage(img);
    return tex;
}

// ======================== TEXTURE GENERATORS ========================
void AssetManager::generateNinjaRunTextures() {
    for (int type = 0; type < 3; type++) {
        Constants::NinjaType nt = static_cast<Constants::NinjaType>(type);
        std::string suffix = getTypeSuffix(nt);
        for (int i = 0; i < 8; i++) {
            char name[32];
            std::snprintf(name, sizeof(name), "ninja_run_%d%s", i, suffix.c_str());
            textures_[name] = createNinjaFrame(i, nt);
        }
    }
}

void AssetManager::generateNinjaJumpTexture() {
    for (int type = 0; type < 3; type++) {
        Constants::NinjaType nt = static_cast<Constants::NinjaType>(type);
        std::string suffix = getTypeSuffix(nt);
        textures_["ninja_jump" + suffix] = renderNinjaVector(50, 60,
            getNinjaBodyColor(nt),
            getNinjaAccentColor(nt),
            getNinjaAccent2Color(nt),
            0.0f, false, true);
    }
}

void AssetManager::generateNinjaSlideTexture() {
    for (int type = 0; type < 3; type++) {
        Constants::NinjaType nt = static_cast<Constants::NinjaType>(type);
        std::string suffix = getTypeSuffix(nt);
        textures_["ninja_slide" + suffix] = renderNinjaVector(50, 40,
            getNinjaBodyColor(nt),
            getNinjaAccentColor(nt),
            getNinjaAccent2Color(nt),
            0.0f, true, false);
    }
}

void AssetManager::generateObstacleTextures() {
    textures_["chimney"] = createChimneyTexture();
    textures_["ac_unit"] = createACUnitTexture();
    textures_["satellite"] = createSatelliteDishTexture();
    textures_["clothesline"] = createClotheslineTexture();
    textures_["sign"] = createSignTexture();
    textures_["pigeon_0"] = createPigeonTexture(0);
    textures_["pigeon_1"] = createPigeonTexture(1);
    textures_["shuriken"] = createShurikenTexture();
}

void AssetManager::generateBuildingTextures() {
    textures_["city_bg"] = createCityBackgroundTexture(Constants::WINDOW_WIDTH, 300);
    textures_["city_bg_night"] = createNightCityBackgroundTexture(Constants::WINDOW_WIDTH, 300);
    auto midTex = createCityBackgroundTexture(Constants::WINDOW_WIDTH, 350);
    textures_["city_mid"] = midTex;
}

void AssetManager::generateUITextures() {
    textures_["coin"] = createShurikenTexture();
    textures_["powerup"] = createShurikenTexture();
}

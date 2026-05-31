#ifndef CORE_ASSETMANAGER_H
#define CORE_ASSETMANAGER_H

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <string>
#include <unordered_map>
#include <memory>
#include "Constants.h"

class AssetManager {
public:
    AssetManager();
    sf::Texture& getTexture(const std::string& name);
    sf::Font& getFont(const std::string& name);
    bool hasTexture(const std::string& name) const;
    bool hasFont(const std::string& name) const;
    void generateAllAssets();

private:
    void loadPngAssets();
    bool loadTextureFromFile(const std::string& texName, const std::string& filePath);
    void generateFont();
    void generateNinjaRunTextures();
    void generateNinjaJumpTexture();
    void generateNinjaSlideTexture();
    void generateObstacleTextures();
    void generateBuildingTextures();
    void generateUITextures();

    static sf::Color getNinjaBodyColor(Constants::NinjaType type);
    static sf::Color getNinjaAccentColor(Constants::NinjaType type);
    static sf::Color getNinjaAccent2Color(Constants::NinjaType type);
    static std::string getTypeSuffix(Constants::NinjaType type);

    std::unordered_map<std::string, sf::Texture> textures_;
    std::unordered_map<std::string, sf::Font> fonts_;

    sf::Texture createRectTexture(int w, int h, sf::Color color);
    sf::Texture createNinjaFrame(int index, Constants::NinjaType type);
    sf::Texture createChimneyTexture();
    sf::Texture createACUnitTexture();
    sf::Texture createSatelliteDishTexture();
    sf::Texture createClotheslineTexture();
    sf::Texture createSignTexture();
    sf::Texture createPigeonTexture(int frame);
    sf::Texture createShurikenTexture();
    sf::Texture createBuildingTexture(int height, sf::Color color);
    sf::Texture createCityBackgroundTexture(int w, int h);
    sf::Texture createNightCityBackgroundTexture(int w, int h);
};

#endif

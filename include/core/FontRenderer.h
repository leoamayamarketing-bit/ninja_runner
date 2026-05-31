#ifndef CORE_FONTRENDERER_H
#define CORE_FONTRENDERER_H

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <string>
#include <vector>

class FontRenderer {
public:
    FontRenderer();
    void generate();
    void drawText(sf::RenderWindow& window, const std::string& text,
                  float x, float y, unsigned int size, const sf::Color& color) const;
    void drawTextCentered(sf::RenderWindow& window, const std::string& text,
                          float x, float y, unsigned int size, const sf::Color& color) const;
    float getTextWidth(const std::string& text, unsigned int size) const;

private:
    sf::Texture glyphTexture_;
    bool generated_ = false;

    // Character cell: 8 wide, 12 tall. Grid: 16 cols x 6 rows = 96 chars
    static constexpr int CHAR_WIDTH = 8;
    static constexpr int CHAR_HEIGHT = 12;
    static constexpr int COLS = 16;
    static constexpr int ROWS = 6;

    void drawGlyph(const sf::Color& color, int x, int y, int gx, int gy,
                   sf::VertexArray& va, float scale) const;
};

#endif

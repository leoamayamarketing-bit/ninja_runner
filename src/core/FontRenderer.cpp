#include "../../include/core/FontRenderer.h"
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <cmath>
#include <cstring>

FontRenderer::FontRenderer() = default;

void FontRenderer::generate() {
    if (generated_) return;

    // Create a texture with 16x6 grid of 8x12 characters (96 chars total)
    // That covers ASCII 32-127
    sf::Image img;
    img.create(COLS * CHAR_WIDTH, ROWS * CHAR_HEIGHT, sf::Color::Transparent);

    const char* fontData[ROWS][COLS] = {
        // Row 0: space (32) to / (47)
        {
            // space
            "",
            // !
            "  ##  "
            "  ##  "
            "  ##  "
            "  ##  "
            "  ##  "
            "  ##  "
            "  ##  "
            "     "
            "  ##  "
            "  ##  "
            "     "
            "     ",
            // "
            " ## ## "
            " ## ## "
            " ## ## "
            "      "
            "      "
            "      "
            "      "
            "      "
            "      "
            "      "
            "      "
            "      ",
            // #
            "  # #  "
            "  # #  "
            " ######"
            "  # #  "
            "  # #  "
            " ######"
            "  # #  "
            "  # #  "
            "      "
            "      "
            "      "
            "      ",
            // $
            "   #   "
            "  #### "
            " ## #  "
            " ##    "
            "  ###  "
            "   ##  "
            "   ##  "
            " # ##  "
            " ####  "
            "   #   "
            "   #   "
            "      ",
            // %
            " ##    "
            "#  #   "
            " ## #  "
            "    #  "
            "   #   "
            "  #    "
            " #     "
            "#  ##  "
            "  #  # "
            "   ##  "
            "      "
            "      ",
            // &
            "  ##   "
            " #  #  "
            " #  #  "
            "  ##   "
            " ###   "
            " #  # #"
            " #   # "
            " #   # "
            "  ### #"
            "      "
            "      "
            "      ",
            // '
            "  ##  "
            "  ##  "
            "  ##  "
            "  #   "
            "      "
            "      "
            "      "
            "      "
            "      "
            "      "
            "      "
            "      ",
            // (
            "    # "
            "   #  "
            "   #  "
            "  #   "
            "  #   "
            "  #   "
            "  #   "
            "  #   "
            "   #  "
            "   #  "
            "    # "
            "      ",
            // )
            " #    "
            "  #   "
            "  #   "
            "   #  "
            "   #  "
            "   #  "
            "   #  "
            "   #  "
            "  #   "
            "  #   "
            " #    "
            "      ",
            // *
            "      "
            "   #  "
            " # # #"
            "  ### "
            " #####"
            "  ### "
            " # # #"
            "   #  "
            "      "
            "      "
            "      "
            "      ",
            // +
            "      "
            "      "
            "   #  "
            "   #  "
            "   #  "
            " #####"
            "   #  "
            "   #  "
            "   #  "
            "      "
            "      "
            "      ",
            // ,
            "      "
            "      "
            "      "
            "      "
            "      "
            "      "
            "      "
            "  ##  "
            "  ##  "
            "  ##  "
            "  #   "
            "      ",
            // -
            "      "
            "      "
            "      "
            "      "
            "      "
            " #####"
            "      "
            "      "
            "      "
            "      "
            "      "
            "      ",
            // .
            "      "
            "      "
            "      "
            "      "
            "      "
            "      "
            "      "
            "      "
            "  ##  "
            "  ##  "
            "      "
            "      ",
            // /
            "     #"
            "    # "
            "    # "
            "   #  "
            "   #  "
            "  #   "
            "  #   "
            " #    "
            " #    "
            "#     "
            "      "
            "      "
        },
        // Row 1: 0 to ?
        {
            // 0
            "  ### "
            " #   #"
            "#  # #"
            "# #  #"
            "##   #"
            "# #  #"
            "#  # #"
            " #   #"
            "  ### "
            "      "
            "      "
            "      ",
            // 1
            "   #  "
            "  ##  "
            "   #  "
            "   #  "
            "   #  "
            "   #  "
            "   #  "
            "   #  "
            "  ### "
            "      "
            "      "
            "      ",
            // 2
            "  ### "
            " #   #"
            "     #"
            "    # "
            "   #  "
            "  #   "
            " #    "
            "#     "
            "######"
            "      "
            "      "
            "      ",
            // 3
            "  ### "
            " #   #"
            "     #"
            "   ## "
            "     #"
            "     #"
            "     #"
            " #   #"
            "  ### "
            "      "
            "      "
            "      ",
            // 4
            "    # "
            "   ## "
            "  # # "
            " #  # "
            "#   # "
            "######"
            "    # "
            "    # "
            "    # "
            "      "
            "      "
            "      ",
            // 5
            "######"
            "#     "
            "#     "
            "##### "
            "     #"
            "     #"
            "     #"
            " #   #"
            "  ### "
            "      "
            "      "
            "      ",
            // 6
            "  ### "
            " #   #"
            "#     "
            "#     "
            "##### "
            "#    #"
            "#    #"
            " #   #"
            "  ### "
            "      "
            "      "
            "      ",
            // 7
            "######"
            "     #"
            "    # "
            "   #  "
            "  #   "
            " #    "
            " #    "
            " #    "
            " #    "
            "      "
            "      "
            "      ",
            // 8
            "  ### "
            " #   #"
            "#    #"
            " #   #"
            "  ### "
            " #   #"
            "#    #"
            " #   #"
            "  ### "
            "      "
            "      "
            "      ",
            // 9
            "  ### "
            " #   #"
            "#    #"
            " #   #"
            "  ####"
            "     #"
            "     #"
            " #   #"
            "  ### "
            "      "
            "      "
            "      ",
            // :
            "      "
            "      "
            "      "
            "  ##  "
            "  ##  "
            "      "
            "      "
            "  ##  "
            "  ##  "
            "      "
            "      "
            "      ",
            // ;
            "      "
            "      "
            "      "
            "  ##  "
            "  ##  "
            "      "
            "      "
            "  ##  "
            "  ##  "
            "  #   "
            "      "
            "      ",
            // <
            "    # "
            "   #  "
            "  #   "
            " #    "
            "#     "
            " #    "
            "  #   "
            "   #  "
            "    # "
            "      "
            "      "
            "      ",
            // =
            "      "
            "      "
            "      "
            "######"
            "      "
            "      "
            "######"
            "      "
            "      "
            "      "
            "      "
            "      ",
            // >
            " #    "
            "  #   "
            "   #  "
            "    # "
            "     #"
            "    # "
            "   #  "
            "  #   "
            " #    "
            "      "
            "      "
            "      ",
            // ?
            "  ### "
            " #   #"
            "     #"
            "    # "
            "   #  "
            "   #  "
            "   #  "
            "      "
            "   #  "
            "   #  "
            "      "
            "      "
        },
        // Row 2: @ to O
        {
            // @
            "  #### "
            " #    #"
            "#  ## #"
            "# #  #"
            "# #  #"
            "# #  #"
            "#  ### "
            " #     "
            "  #### "
            "       "
            "       "
            "       ",
            // A
            "  ###  "
            " #   # "
            "#     #"
            "#     #"
            "#######"
            "#     #"
            "#     #"
            "#     #"
            "#     #"
            "       "
            "       "
            "       ",
            // B
            "###### "
            "#     #"
            "#     #"
            "#     #"
            "###### "
            "#     #"
            "#     #"
            "#     #"
            "###### "
            "       "
            "       "
            "       ",
            // C
            "  #### "
            " #    #"
            "#      "
            "#      "
            "#      "
            "#      "
            "#      "
            " #    #"
            "  #### "
            "       "
            "       "
            "       ",
            // D
            "#####  "
            "#    # "
            "#     #"
            "#     #"
            "#     #"
            "#     #"
            "#     #"
            "#    # "
            "#####  "
            "       "
            "       "
            "       ",
            // E
            "#######"
            "#      "
            "#      "
            "#      "
            "#####  "
            "#      "
            "#      "
            "#      "
            "#######"
            "       "
            "       "
            "       ",
            // F
            "#######"
            "#      "
            "#      "
            "#      "
            "#####  "
            "#      "
            "#      "
            "#      "
            "#      "
            "       "
            "       "
            "       ",
            // G
            " ##### "
            "#     #"
            "#      "
            "#      "
            "#  ####"
            "#     #"
            "#     #"
            "#     #"
            " ##### "
            "       "
            "       "
            "       ",
            // H
            "#     #"
            "#     #"
            "#     #"
            "#     #"
            "#######"
            "#     #"
            "#     #"
            "#     #"
            "#     #"
            "       "
            "       "
            "       ",
            // I
            " ##### "
            "   #   "
            "   #   "
            "   #   "
            "   #   "
            "   #   "
            "   #   "
            "   #   "
            " ##### "
            "       "
            "       "
            "       ",
            // J
            "    ###"
            "     # "
            "     # "
            "     # "
            "     # "
            "     # "
            "#    # "
            "#    # "
            " ####  "
            "       "
            "       "
            "       ",
            // K
            "#    # "
            "#   #  "
            "#  #   "
            "# #    "
            "##     "
            "# #    "
            "#  #   "
            "#   #  "
            "#    # "
            "       "
            "       "
            "       ",
            // L
            "#      "
            "#      "
            "#      "
            "#      "
            "#      "
            "#      "
            "#      "
            "#      "
            "#######"
            "       "
            "       "
            "       ",
            // M
            "#     #"
            "##   ##"
            "# # # #"
            "#  #  #"
            "#     #"
            "#     #"
            "#     #"
            "#     #"
            "#     #"
            "       "
            "       "
            "       ",
            // N
            "#     #"
            "##    #"
            "# #   #"
            "#  #  #"
            "#   # #"
            "#    ##"
            "#     #"
            "#     #"
            "#     #"
            "       "
            "       "
            "       ",
            // O
            " ##### "
            "#     #"
            "#     #"
            "#     #"
            "#     #"
            "#     #"
            "#     #"
            "#     #"
            " ##### "
            "       "
            "       "
            "       "
        },
        // Row 3: P to _
        {
            // P
            "###### "
            "#     #"
            "#     #"
            "#     #"
            "###### "
            "#      "
            "#      "
            "#      "
            "#      "
            "       "
            "       "
            "       ",
            // Q
            " ##### "
            "#     #"
            "#     #"
            "#     #"
            "#     #"
            "#  #  #"
            "#   # #"
            "#     #"
            " ##### "
            "    #  "
            "     # "
            "       ",
            // R
            "###### "
            "#     #"
            "#     #"
            "#     #"
            "###### "
            "#  #   "
            "#   #  "
            "#    # "
            "#     #"
            "       "
            "       "
            "       ",
            // S
            " ##### "
            "#     #"
            "#      "
            " ##    "
            "   ##  "
            "     # "
            "#     #"
            "#     #"
            " ##### "
            "       "
            "       "
            "       ",
            // T
            "#######"
            "   #   "
            "   #   "
            "   #   "
            "   #   "
            "   #   "
            "   #   "
            "   #   "
            "   #   "
            "       "
            "       "
            "       ",
            // U
            "#     #"
            "#     #"
            "#     #"
            "#     #"
            "#     #"
            "#     #"
            "#     #"
            "#     #"
            " ##### "
            "       "
            "       "
            "       ",
            // V
            "#     #"
            "#     #"
            "#     #"
            "#     #"
            " #   # "
            " #   # "
            "  # #  "
            "  # #  "
            "   #   "
            "       "
            "       "
            "       ",
            // W
            "#     #"
            "#     #"
            "#     #"
            "#     #"
            "#  #  #"
            "# # # #"
            "# # # #"
            "##   ##"
            "#     #"
            "       "
            "       "
            "       ",
            // X
            "#     #"
            "#     #"
            " #   # "
            "  # #  "
            "   #   "
            "  # #  "
            " #   # "
            "#     #"
            "#     #"
            "       "
            "       "
            "       ",
            // Y
            "#     #"
            "#     #"
            " #   # "
            "  # #  "
            "   #   "
            "   #   "
            "   #   "
            "   #   "
            "   #   "
            "       "
            "       "
            "       ",
            // Z
            "#######"
            "     # "
            "    #  "
            "   #   "
            "  #    "
            " #     "
            "#      "
            "#      "
            "#######"
            "       "
            "       "
            "       ",
            // [
            " #####"
            " #    "
            " #    "
            " #    "
            " #    "
            " #    "
            " #    "
            " #    "
            " #####"
            "      "
            "      "
            "      ",
            // backslash
            "#     "
            " #    "
            " #    "
            "  #   "
            "  #   "
            "   #  "
            "   #  "
            "    # "
            "    # "
            "     #"
            "      "
            "      ",
            // ]
            "##### "
            "    # "
            "    # "
            "    # "
            "    # "
            "    # "
            "    # "
            "    # "
            "##### "
            "      "
            "      "
            "      ",
            // ^
            "   #   "
            "  # #  "
            " #   # "
            "#     #"
            "       "
            "       "
            "       "
            "       "
            "       "
            "       "
            "       "
            "       ",
            // _
            "       "
            "       "
            "       "
            "       "
            "       "
            "       "
            "       "
            "       "
            "       "
            "#######"
            "       "
            "       "
        },
        // Row 4: ` to o
        {
            // `
            " ##   "
            "  #   "
            "   #  "
            "      "
            "      "
            "      "
            "      "
            "      "
            "      "
            "      "
            "      "
            "      ",
            // a
            "      "
            "      "
            "      "
            "  ### "
            "     #"
            "  ####"
            " #   #"
            " #   #"
            "  ####"
            "      "
            "      "
            "      ",
            // b
            "#      "
            "#      "
            "#      "
            "# ###  "
            "##   # "
            "#     #"
            "#     #"
            "#     #"
            "###### "
            "       "
            "       "
            "       ",
            // c
            "      "
            "      "
            "      "
            "  ### "
            " #   #"
            "#     "
            "#     "
            " #   #"
            "  ### "
            "      "
            "      "
            "      ",
            // d
            "     #"
            "     #"
            "     #"
            "  ### #"
            " #   #"
            "#    #"
            "#    #"
            " #   #"
            "  ####"
            "      "
            "      "
            "      ",
            // e
            "      "
            "      "
            "      "
            "  ### "
            " #   #"
            "######"
            "#     "
            " #   #"
            "  ### "
            "      "
            "      "
            "      ",
            // f
            "   ## "
            "  #   "
            "  #   "
            " #### "
            "  #   "
            "  #   "
            "  #   "
            "  #   "
            "  #   "
            "      "
            "      "
            "      ",
            // g
            "      "
            "      "
            "  ####"
            " #   #"
            "#    #"
            " #   #"
            "  ####"
            "     #"
            "  ### "
            "      "
            "      "
            "      ",
            // h
            "#      "
            "#      "
            "#      "
            "# ###  "
            "##   # "
            "#     #"
            "#     #"
            "#     #"
            "#     #"
            "       "
            "       "
            "       ",
            // i
            "  #   "
            "  #   "
            "      "
            " ###  "
            "  #   "
            "  #   "
            "  #   "
            "  #   "
            " ###  "
            "      "
            "      "
            "      ",
            // j
            "   #  "
            "   #  "
            "      "
            "  ### "
            "    # "
            "    # "
            "    # "
            "    # "
            "    # "
            "   #  "
            "  #   "
            "      ",
            // k
            "#      "
            "#      "
            "#      "
            "#   #  "
            "#  #   "
            "###    "
            "#  #   "
            "#   #  "
            "#    # "
            "       "
            "       "
            "       ",
            // l
            " ###  "
            "  #   "
            "  #   "
            "  #   "
            "  #   "
            "  #   "
            "  #   "
            "  #   "
            " ###  "
            "      "
            "      "
            "      ",
            // m
            "      "
            "      "
            "      "
            "## # #"
            "# # # "
            "# # # "
            "# # # "
            "# # # "
            "# # # "
            "      "
            "      "
            "      ",
            // n
            "      "
            "      "
            "      "
            "# ### "
            "##   #"
            "#     #"
            "#     #"
            "#     #"
            "#     #"
            "       "
            "       "
            "       ",
            // o
            "      "
            "      "
            "      "
            "  ### "
            " #   #"
            "#     #"
            "#     #"
            " #   #"
            "  ### "
            "      "
            "      "
            "      "
        },
        // Row 5: p to ~
        {
            // p
            "      "
            "      "
            "      "
            "######"
            "#     #"
            "#     #"
            "######"
            "#     "
            "#     "
            "#     "
            "      "
            "      ",
            // q
            "      "
            "      "
            "      "
            "  ####"
            " #   #"
            "#    #"
            " #   #"
            "  ####"
            "     #"
            "     #"
            "     #"
            "      ",
            // r
            "      "
            "      "
            "      "
            "# ### "
            "##   #"
            "#     "
            "#     "
            "#     "
            "#     "
            "       "
            "       "
            "       ",
            // s
            "      "
            "      "
            "      "
            "  ####"
            " #    "
            "  ### "
            "     #"
            "     #"
            " #### "
            "      "
            "      "
            "      ",
            // t
            "  #   "
            "  #   "
            "  #   "
            " #### "
            "  #   "
            "  #   "
            "  #   "
            "  #   "
            "   ## "
            "      "
            "      "
            "      ",
            // u
            "      "
            "      "
            "      "
            "#    #"
            "#    #"
            "#    #"
            "#    #"
            " #   #"
            "  ####"
            "      "
            "      "
            "      ",
            // v
            "      "
            "      "
            "      "
            "#     #"
            "#     #"
            " #   # "
            "  # #  "
            "  # #  "
            "   #   "
            "       "
            "       "
            "       ",
            // w
            "      "
            "      "
            "      "
            "#    #"
            "#    #"
            "# #  #"
            "# #  #"
            " # ## "
            "  #   "
            "      "
            "      "
            "      ",
            // x
            "      "
            "      "
            "      "
            "#    #"
            " #  # "
            "  ##  "
            "  ##  "
            " #  # "
            "#    #"
            "      "
            "      "
            "      ",
            // y
            "      "
            "      "
            "      "
            "#    #"
            "#    #"
            " #  # "
            "  # # "
            "   #  "
            "  #   "
            "  #   "
            " #    "
            "      ",
            // z
            "      "
            "      "
            "      "
            "######"
            "    # "
            "   #  "
            "  #   "
            " #    "
            "######"
            "      "
            "      "
            "      ",
            // {
            "    ##"
            "   #  "
            "   #  "
            "   #  "
            "  #   "
            " #    "
            "  #   "
            "   #  "
            "   #  "
            "   #  "
            "    ##"
            "      ",
            // |
            "  #   "
            "  #   "
            "  #   "
            "  #   "
            "  #   "
            "  #   "
            "  #   "
            "  #   "
            "  #   "
            "  #   "
            "  #   "
            "      ",
            // }
            "##    "
            "  #   "
            "  #   "
            "  #   "
            "   #  "
            "    # "
            "   #  "
            "  #   "
            "  #   "
            "  #   "
            "##    "
            "      ",
            // ~
            "      "
            "      "
            "      "
            " ##   "
            "#   ##"
            "      "
            "      "
            "      "
            "      "
            "      "
            "      "
            "      ",
            // DEL (empty)
            "      "
            "      "
            "      "
            "      "
            "      "
            "      "
            "      "
            "      "
            "      "
            "      "
            "      "
            "      "
        }
    };

    // Draw all characters into the image
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            const char* glyph = fontData[row][col];
            int len = std::strlen(glyph);
            int ox = col * CHAR_WIDTH;
            int oy = row * CHAR_HEIGHT;

            for (int i = 0; i < len && i < CHAR_WIDTH * CHAR_HEIGHT; i++) {
                int px = i % CHAR_WIDTH;
                int py = i / CHAR_WIDTH;
                if (py >= CHAR_HEIGHT) break;
                if (glyph[i] == '#') {
                    img.setPixel(ox + px, oy + py, sf::Color::White);
                }
            }
        }
    }

    glyphTexture_.loadFromImage(img);
    glyphTexture_.setSmooth(false);
    generated_ = true;
}

void FontRenderer::drawGlyph(const sf::Color& color, int x, int y, int gx, int gy,
                              sf::VertexArray& va, float scale) const {
    float texW = static_cast<float>(glyphTexture_.getSize().x);
    float texH = static_cast<float>(glyphTexture_.getSize().y);

    float left = gx * CHAR_WIDTH / texW;
    float top = gy * CHAR_HEIGHT / texH;
    float right = (gx * CHAR_WIDTH + CHAR_WIDTH) / texW;
    float bottom = (gy * CHAR_HEIGHT + CHAR_HEIGHT) / texH;

    float sx = static_cast<float>(x);
    float sy = static_cast<float>(y);
    float sw = CHAR_WIDTH * scale;
    float sh = CHAR_HEIGHT * scale;

    sf::Vertex v0(sf::Vector2f(sx, sy), color, sf::Vector2f(left, top));
    sf::Vertex v1(sf::Vector2f(sx + sw, sy), color, sf::Vector2f(right, top));
    sf::Vertex v2(sf::Vector2f(sx + sw, sy + sh), color, sf::Vector2f(right, bottom));
    sf::Vertex v3(sf::Vector2f(sx, sy + sh), color, sf::Vector2f(left, bottom));

    va.append(v0);
    va.append(v1);
    va.append(v2);
    va.append(v0);
    va.append(v2);
    va.append(v3);
}

void FontRenderer::drawText(sf::RenderWindow& window, const std::string& text,
                             float x, float y, unsigned int size, const sf::Color& color) const {
    if (!generated_ || text.empty()) return;

    float scale = size / 12.0f;
    sf::VertexArray va(sf::Triangles);
    sf::RenderStates states;
    states.texture = &glyphTexture_;

    for (size_t i = 0; i < text.size(); i++) {
        char c = text[i];
        if (c < 32 || c > 126) c = 32;
        int idx = c - 32;
        int gx = idx % COLS;
        int gy = idx / COLS;
        drawGlyph(color, static_cast<int>(x + i * CHAR_WIDTH * scale),
                  static_cast<int>(y), gx, gy, va, scale);
    }

    window.draw(va, states);
}

void FontRenderer::drawTextCentered(sf::RenderWindow& window, const std::string& text,
                                     float x, float y, unsigned int size,
                                     const sf::Color& color) const {
    float w = getTextWidth(text, size);
    drawText(window, text, x - w / 2, y, size, color);
}

float FontRenderer::getTextWidth(const std::string& text, unsigned int size) const {
    return text.length() * CHAR_WIDTH * (size / 12.0f);
}

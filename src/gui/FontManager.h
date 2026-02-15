#ifndef SML_FONT_MANAGER_H
#define SML_FONT_MANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <map>
#include <vector>

#include "Color.h"

namespace sml {

enum class FontSize {
    SMALL = 16,     // Detail text, BPM, speed
    MEDIUM = 24,    // Song artist, menu items
    LARGE = 36,     // Song title in list, score integer part
    HUGE = 54,      // Grade, combo, large score
    TITLE = 72      // Scene titles
};

enum class TextAlign {
    LEFT,
    CENTER,
    RIGHT
};

class FontManager {
public:
    FontManager();
    ~FontManager();

    bool Init(const std::string& fontPath); // Default font
    bool LoadFont(const std::string& name, const std::string& fontPath);
    void Shutdown();

    // Core drawing function
    void DrawText(SDL_Renderer* renderer, int x, int y, 
                  const std::string& text, Color color, 
                  FontSize size, TextAlign align = TextAlign::LEFT, double scale = 1.0,
                  const std::string& fontName = "default");

    // Outline support (simulated by rendering shifted copies)
    void DrawTextOutline(SDL_Renderer* renderer, int x, int y, 
                         const std::string& text, Color color, Color outlineColor,
                         FontSize size, TextAlign align = TextAlign::LEFT, double scale = 1.0,
                         const std::string& fontName = "default", int thickness = 2);

    // Monospaced character drawing (fixed cell width)
    void DrawMonoText(SDL_Renderer* renderer, int x, int y, 
                      const std::string& text, Color color, 
                      FontSize size, TextAlign align = TextAlign::LEFT, double scale = 1.0,
                      const std::string& fontName = "default", int cellWidth = -1,
                      bool useOutline = false, Color outlineColor = {0,0,0,0});

    // Advanced drawing for "Large Integer, Small Decimal" accuracy style
    void DrawAccuracy(SDL_Renderer* renderer, int x, int y, 
                      double accuracy, Color color, TextAlign align = TextAlign::CENTER, double scale = 1.0, int precision = 4,
                      bool useOutline = false, bool boldInteger = false, Color outlineColor = {0,0,0,0});

    int GetTextWidth(const std::string& text, FontSize size, const std::string& fontName = "default");
    int GetFontHeight(FontSize size, const std::string& fontName = "default");

private:
    struct FontKey {
        std::string name;
        FontSize size;
        bool operator<(const FontKey& other) const {
            if (name != other.name) return name < other.name;
            return size < other.size;
        }
    };

    struct GlyphKey {
        std::string fontName;
        FontSize size;
        char ch;
        Color color;
        int outline;
        
        bool operator<(const GlyphKey& other) const {
            if (fontName != other.fontName) return fontName < other.fontName;
            if (size != other.size) return size < other.size;
            if (ch != other.ch) return ch < other.ch;
            if (outline != other.outline) return outline < other.outline;
            // Compare color
            if (color.r != other.color.r) return color.r < other.color.r;
            if (color.g != other.color.g) return color.g < other.color.g;
            if (color.b != other.color.b) return color.b < other.color.b;
            return color.a < other.color.a;
        }
    };

    std::map<std::string, std::string> font_paths_;
    std::map<FontKey, TTF_Font*> fonts_;
    std::map<GlyphKey, SDL_Texture*> glyph_cache_; // Performance optimization
    bool initialized_ = false;

    TTF_Font* GetFont(FontSize size, const std::string& name = "default");
    SDL_Texture* GetGlyphTexture(SDL_Renderer* renderer, char ch, FontSize size, Color color, const std::string& fontName, int outline);
};

} // namespace sml

#endif // SML_FONT_MANAGER_H

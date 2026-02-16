#include "FontManager.h"
#include <iostream>
#include <cstdio>
#include <cmath>

namespace sml {

FontManager::FontManager() {}

FontManager::~FontManager() {
    Shutdown();
}

bool FontManager::Init(const std::string& fontPath) {
    if (initialized_) return true;

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    initialized_ = true;
    return LoadFont("default", fontPath);
}

bool FontManager::LoadFont(const std::string& name, const std::string& fontPath) {
    if (!initialized_) return false;
    
    font_paths_[name] = fontPath;
    
    // Pre-load common sizes for the new font
    std::vector<FontSize> sizes = { 
        FontSize::SMALL, FontSize::MEDIUM, FontSize::LARGE, FontSize::HUGE, FontSize::TITLE, FontSize::GIANT
    };

    bool success = false;
    for (auto size : sizes) {
        TTF_Font* font = TTF_OpenFont(fontPath.c_str(), static_cast<int>(size));
        if (!font) {
            std::cerr << "Failed to load font " << name << " from " << fontPath << " at size " 
                      << static_cast<int>(size) << "! TTF Error: " << TTF_GetError() << std::endl;
            continue;
        }
        fonts_[{name, size}] = font;
        success = true;
    }

    return success;
}

void FontManager::Shutdown() {
    for (auto const& [key, font] : fonts_) {
        TTF_CloseFont(font);
    }
    fonts_.clear();

    for (auto const& [key, tex] : glyph_cache_) {
        SDL_DestroyTexture(tex);
    }
    glyph_cache_.clear();
    
    if (initialized_) {
        TTF_Quit();
        initialized_ = false;
    }
}

void FontManager::DrawText(SDL_Renderer* renderer, int x, int y, 
                         const std::string& text, Color color, 
                         FontSize size, TextAlign align, double scale,
                         const std::string& fontName) {
    if (text.empty()) return;
    
    // For general text, we can still use the glyph-by-glyph approach (DrawMonoText/DrawText internal)
    // or just render the whole string if it's dynamic. 
    // Since most of our HUD is monospaced numbers, let's optimize the character drawing.
    
    int total_w = GetTextWidth(text, size, fontName);
    int scaled_total_w = static_cast<int>(total_w * scale);

    int cur_x = x;
    if (align == TextAlign::CENTER) cur_x -= scaled_total_w / 2;
    else if (align == TextAlign::RIGHT) cur_x -= scaled_total_w;

    size_t i = 0;
    while (i < text.length()) {
        std::string ch;
        unsigned char c = static_cast<unsigned char>(text[i]);
        if (c < 0x80) { ch = text.substr(i, 1); i += 1; }
        else if ((c & 0xE0) == 0xC0) { ch = text.substr(i, 2); i += 2; }
        else if ((c & 0xF0) == 0xE0) { ch = text.substr(i, 3); i += 3; }
        else if ((c & 0xF8) == 0xF0) { ch = text.substr(i, 4); i += 4; }
        else { i++; continue; }

        SDL_Texture* tex = GetGlyphTexture(renderer, ch, size, color, fontName, 0);
        if (!tex) continue;

        int w, h;
        SDL_QueryTexture(tex, NULL, NULL, &w, &h);
        int sw = static_cast<int>(w * scale);
        int sh = static_cast<int>(h * scale);

        SDL_Rect dst = { cur_x, y, sw, sh };
        SDL_RenderCopy(renderer, tex, NULL, &dst);
        cur_x += sw;
    }
}

void FontManager::DrawTextOutline(SDL_Renderer* renderer, int x, int y, 
                                 const std::string& text, Color color, Color outlineColor,
                                 FontSize size, TextAlign align, double scale,
                                 const std::string& fontName, int thickness) {
    int total_w = GetTextWidth(text, size, fontName);
    int scaled_total_w = static_cast<int>(total_w * scale);

    int cur_x = x;
    if (align == TextAlign::CENTER) cur_x -= scaled_total_w / 2;
    else if (align == TextAlign::RIGHT) cur_x -= scaled_total_w;

    size_t i = 0;
    while (i < text.length()) {
        std::string ch;
        unsigned char c = static_cast<unsigned char>(text[i]);
        if (c < 0x80) { ch = text.substr(i, 1); i += 1; }
        else if ((c & 0xE0) == 0xC0) { ch = text.substr(i, 2); i += 2; }
        else if ((c & 0xF0) == 0xE0) { ch = text.substr(i, 3); i += 3; }
        else if ((c & 0xF8) == 0xF0) { ch = text.substr(i, 4); i += 4; }
        else { i++; continue; }

        // Draw outline first
        SDL_Texture* out_tex = GetGlyphTexture(renderer, ch, size, outlineColor, fontName, thickness);
        if (out_tex) {
            int w, h;
            SDL_QueryTexture(out_tex, NULL, NULL, &w, &h);
            int sw = static_cast<int>(w * scale);
            int sh = static_cast<int>(h * scale);
            SDL_Rect dst = { cur_x - static_cast<int>(thickness * scale), y - static_cast<int>(thickness * scale), sw, sh };
            SDL_RenderCopy(renderer, out_tex, NULL, &dst);
        }

        // Draw character
        SDL_Texture* tex = GetGlyphTexture(renderer, ch, size, color, fontName, 0);
        if (tex) {
            int w, h;
            SDL_QueryTexture(tex, NULL, NULL, &w, &h);
            int sw = static_cast<int>(w * scale);
            int sh = static_cast<int>(h * scale);
            SDL_Rect dst = { cur_x, y, sw, sh };
            SDL_RenderCopy(renderer, tex, NULL, &dst);
            cur_x += sw;
        }
    }
}

void FontManager::DrawMonoText(SDL_Renderer* renderer, int x, int y, const std::string& text,
                      Color color, FontSize size, TextAlign align,
                      double scale, const std::string& fontName, int cellWidth,
                      bool useOutline, Color outlineColor, int weight,
                      int outlineThickness) {
    if (text.empty()) return;
    
    if (cellWidth <= 0) {
        cellWidth = GetTextWidth("0", size, fontName);
    }
    
    int scaledCellW = static_cast<int>(cellWidth * scale);
    int totalW = static_cast<int>(text.length() * scaledCellW);
    
    int curX = x;
    if (align == TextAlign::CENTER) curX -= totalW / 2;
    else if (align == TextAlign::RIGHT) curX -= totalW;
    
    // Default outline color is dimmer version of text color if not provided {0,0,0,0} or {0,0,0,255} (default in header was {0,0,0,255})
    // Let's check for "uninitialized" outline color. In header it's {0,0,0,255}.
    // If the user wants black, they'd pass {0,0,0,255}. 
    // Let's use a special sentinel or just check if it's default.
    // Actually, I'll just change the header default to {0,0,0,0} to indicate "auto-dim".
    
    Color actualOutline = outlineColor;
    if (outlineColor.r == 0 && outlineColor.g == 0 && outlineColor.b == 0 && outlineColor.a == 0) {
        actualOutline = { 
            static_cast<uint8_t>(color.r / 2), 
            static_cast<uint8_t>(color.g / 2), 
            static_cast<uint8_t>(color.b / 2), 
            color.a 
        };
    }

    size_t i = 0;
    while (i < text.length()) {
        std::string ch;
        unsigned char c = static_cast<unsigned char>(text[i]);
        if (c < 0x80) { ch = text.substr(i, 1); i += 1; }
        else if ((c & 0xE0) == 0xC0) { ch = text.substr(i, 2); i += 2; }
        else if ((c & 0xF0) == 0xE0) { ch = text.substr(i, 3); i += 3; }
        else if ((c & 0xF8) == 0xF0) { ch = text.substr(i, 4); i += 4; }
        else { i++; continue; }

        // Find offsets for centering glyph in cell
        int baseW = GetTextWidth(ch, size, fontName);
        int scaledBaseW = static_cast<int>(baseW * scale);
        int offsetX = (scaledCellW - scaledBaseW) / 2;

        if (useOutline) {
            int thickness = outlineThickness;
            SDL_Texture* out_tex = GetGlyphTexture(renderer, ch, size, actualOutline, fontName, thickness);
            if (out_tex) {
                int w, h;
                SDL_QueryTexture(out_tex, NULL, NULL, &w, &h);
                int sw = static_cast<int>(w * scale);
                int sh = static_cast<int>(h * scale);
                
                int out_x = curX + offsetX - static_cast<int>(thickness * scale);
                int out_y = y - static_cast<int>(thickness * scale);

                if (weight > 0) {
                    for (int dx = -weight; dx <= weight; ++dx) {
                        for (int dy = -weight; dy <= weight; ++dy) {
                            SDL_Rect bold_out_dst = { out_x + dx, out_y + dy, sw, sh };
                            SDL_RenderCopy(renderer, out_tex, NULL, &bold_out_dst);
                        }
                    }
                } else {
                    SDL_Rect dst = { out_x, out_y, sw, sh };
                    SDL_RenderCopy(renderer, out_tex, NULL, &dst);
                }
            }
        }

        SDL_Texture* tex = GetGlyphTexture(renderer, ch, size, color, fontName, 0);
        if (tex) {
            int w, h;
            SDL_QueryTexture(tex, NULL, NULL, &w, &h);
            int sw = static_cast<int>(w * scale);
            int sh = static_cast<int>(h * scale);
            
            int tx = curX + offsetX;
            
            if (weight > 0) {
                for (int dx = -weight; dx <= weight; ++dx) {
                    for (int dy = -weight; dy <= weight; ++dy) {
                        SDL_Rect bold_dst = { tx + dx, y + dy, sw, sh };
                        SDL_RenderCopy(renderer, tex, NULL, &bold_dst);
                    }
                }
            } else {
                SDL_Rect dst = { tx, y, sw, sh };
                SDL_RenderCopy(renderer, tex, NULL, &dst);
            }
        }
        curX += scaledCellW;
    }
}

void FontManager::DrawAccuracy(SDL_Renderer* renderer, int x, int y, 
                      double accuracy, Color color, TextAlign align, double scale, int precision,
                      bool useOutline, bool boldInteger, Color outlineColor,
                      FontSize integerSize, FontSize decimalSize,
                      int boldWeight, int outlineThickness) {
    char buf[32];
    char fmt[16];
    std::snprintf(fmt, sizeof(fmt), "%%.%df%%%%", precision);
    std::snprintf(buf, sizeof(buf), fmt, accuracy);
    std::string full_text(buf);
    
    size_t dot_pos = full_text.find('.');
    if (dot_pos == std::string::npos) {
        DrawMonoText(renderer, x, y, full_text, color, integerSize, align, scale, "score", -1, useOutline, outlineColor, boldInteger ? boldWeight : 0, outlineThickness);
        return;
    }

    std::string int_part = full_text.substr(0, dot_pos);
    std::string dec_part = full_text.substr(dot_pos);

    int digitW = GetTextWidth("0", integerSize, "score");
    int scaledDigitW = static_cast<int>(digitW * scale);
    int int_w = static_cast<int>(int_part.length() * scaledDigitW);
    
    int decDigitW = GetTextWidth("0", decimalSize, "score");
    int scaledDecDigitW = static_cast<int>(decDigitW * scale);
    int dec_w = static_cast<int>(dec_part.length() * scaledDecDigitW);
    
    int total_w = int_w + dec_w;

    int draw_x = x;
    if (align == TextAlign::CENTER) draw_x -= total_w / 2;
    else if (align == TextAlign::RIGHT) draw_x -= total_w;

    int int_h = GetFontHeight(integerSize, "score");
    int dec_h = GetFontHeight(decimalSize, "score");
    int offset_y = (int_h - dec_h) - 8; 
    int scaled_offset_y = static_cast<int>(offset_y * scale);

    // Draw integer part
    if (boldInteger) {
        DrawMonoText(renderer, draw_x, y, int_part, color, integerSize, TextAlign::LEFT, scale, "score", digitW, useOutline, outlineColor, boldWeight, outlineThickness);
    } else {
        DrawMonoText(renderer, draw_x, y, int_part, color, integerSize, TextAlign::LEFT, scale, "score", digitW, useOutline, outlineColor, 0, outlineThickness);
    }
    
    // Draw decimal part
    DrawMonoText(renderer, draw_x + int_w, y + scaled_offset_y, dec_part, color, decimalSize, TextAlign::LEFT, scale, "score", decDigitW, useOutline, outlineColor, 0, outlineThickness);
}

int FontManager::GetTextWidth(const std::string& text, FontSize size, const std::string& fontName) {
    TTF_Font* font = GetFont(size, fontName);
    if (!font || text.empty()) return 0;
    
    int w, h;
    TTF_SizeUTF8(font, text.c_str(), &w, &h);
    return w;
}

int FontManager::GetFontHeight(FontSize size, const std::string& fontName) {
    TTF_Font* font = GetFont(size, fontName);
    if (!font) return 0;
    return TTF_FontHeight(font);
}

TTF_Font* FontManager::GetFont(FontSize size, const std::string& name) {
    if (fonts_.count({name, size})) return fonts_[{name, size}];
    
    if (!initialized_) return nullptr;
    
    if (font_paths_.count(name)) {
        TTF_Font* font = TTF_OpenFont(font_paths_[name].c_str(), static_cast<int>(size));
        if (font) {
            fonts_[{name, size}] = font;
            return font;
        }
    }
    return nullptr;
}

SDL_Texture* FontManager::GetGlyphTexture(SDL_Renderer* renderer, const std::string& ch, FontSize size, Color color, const std::string& fontName, int outline) {
    GlyphKey key = { fontName, size, ch, color, outline };
    if (glyph_cache_.count(key)) return glyph_cache_[key];

    TTF_Font* font = GetFont(size, fontName);
    if (!font) return nullptr;

    TTF_SetFontOutline(font, outline);
    
    SDL_Color sdl_col = { color.r, color.g, color.b, color.a };
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, ch.c_str(), sdl_col);
    
    // Reset outline for other queries
    TTF_SetFontOutline(font, 0);

    if (!surface) return nullptr;

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (tex) {
        glyph_cache_[key] = tex;
    }
    return tex;
}

} // namespace sml

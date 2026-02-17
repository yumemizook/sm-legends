#pragma once
// ============================================================================
// Profile.h 窶・Player profile data (local / USB)
//
// Stores player identity, high scores, per-player settings, and rating.
// Supports loading from / saving to a directory (local or USB drive).
// ============================================================================

#include <string>
#include <map>
#include <vector>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <filesystem>

namespace sml {

namespace fs = std::filesystem;

/// Per-chart high score record
struct ProfileScore {
    double percentage = 0.0;
    double ex_score   = 0.0;
    int    stars      = 0;
    std::string grade;
    int    max_combo  = 0;
    double rating     = 0.0;   ///< Per-chart rating earned (difficulty + bonus)
    std::string chart_mode;    ///< "single", "double", "couple", "routine"
};

/// Player profile
struct Profile {
    // Identity
    std::string name = "GUEST";
    std::string source_path;   ///< Where this profile was loaded from (empty = guest)
    bool        is_guest  = true;
    bool        is_usb    = false;

    // Per-player settings (override globals when loaded)
    double speed_mod    = 2.0;
    int    mod_type     = 0;     ///< 0 = XMod, 1 = CMod
    bool   downscroll   = false;
    double audio_offset = 0.0;
    std::string noteskin = "Default";
    int    effect_mode  = 0;     ///< 0 = Off, 1 = Mirror, 2 = Random
    int    life_type    = 0;     ///< 0 = Standard, 1 = Life4, 2 = Risky, 3 = Flare
    int    flare_level  = 1;     ///< Flare level (1-5)

    // Rating (separate for single and double)
    double rating_single = 0.0;  ///< Aggregate single-mode rating (avg of top N)
    double rating_double = 0.0;  ///< Aggregate double-mode rating (avg of top N)

    // High scores keyed by "filepath|chart_index"
    std::map<std::string, ProfileScore> high_scores;

    // ========================================================================
    // Rating Calculation
    // ========================================================================

    /// Calculate per-chart rating from chart difficulty and accuracy
    static double CalculateChartRating(double chart_difficulty, double accuracy) {
        double score_val = accuracy * 10000.0;
        double bonus = 0.0;
        if (score_val >= 1007500.0)      bonus = 2.0 + (score_val - 1007500.0) * 0.0001;
        else if (score_val >= 1005000.0) bonus = 1.5 + (score_val - 1005000.0) * 0.0002;
        else if (score_val >= 1000000.0) bonus = 1.0 + (score_val - 1000000.0) * 0.0001;
        else if (score_val >= 975000.0)  bonus = (score_val - 975000.0) * 0.00004;
        else                             bonus = (score_val - 975000.0) / 15000.0;
        return std::max(0.0, chart_difficulty + bonus);
    }

    /// Recalculate rating_single and rating_double from top N chart ratings per mode
    void RecalculateRating(int top_n = 30) {
        std::vector<double> single_ratings, double_ratings;
        for (const auto& [key, sc] : high_scores) {
            if (sc.rating <= 0.0) continue;
            if (sc.chart_mode == "single" || sc.chart_mode == "dance-single") 
                single_ratings.push_back(sc.rating);
            else if (sc.chart_mode == "double" || sc.chart_mode == "dance-double") 
                double_ratings.push_back(sc.rating);
            // couple/routine ratings are 0 and skipped
        }
        auto avg_top = [&](std::vector<double>& v) -> double {
            if (v.empty()) return 0.0;
            std::sort(v.begin(), v.end(), std::greater<double>());
            int count = std::min(top_n, static_cast<int>(v.size()));
            double sum = 0.0;
            for (int i = 0; i < count; ++i) sum += v[i];
            return sum / count;
        };
        rating_single = avg_top(single_ratings);
        rating_double = avg_top(double_ratings);
    }

    // ========================================================================
    // Persistence
    // ========================================================================

    /// Save profile to a directory. Creates profile.cfg and scores.cfg.
    bool Save(const std::string& dir_path) const {
        fs::create_directories(dir_path);

        // --- profile.cfg ---
        std::string profile_path = dir_path + "/profile.cfg";
        std::FILE* f = std::fopen(profile_path.c_str(), "w");
        if (!f) return false;
        std::fprintf(f, "name=%s\n", name.c_str());
        std::fprintf(f, "speed_mod=%.2f\n", speed_mod);
        std::fprintf(f, "mod_type=%d\n", mod_type);
        std::fprintf(f, "downscroll=%d\n", downscroll ? 1 : 0);
        std::fprintf(f, "audio_offset=%.4f\n", audio_offset);
        std::fprintf(f, "noteskin=%s\n", noteskin.c_str());
        std::fprintf(f, "effect_mode=%d\n", effect_mode);
        std::fprintf(f, "life_type=%d\n", life_type);
        std::fprintf(f, "flare_level=%d\n", flare_level);
        std::fprintf(f, "rating_single=%.4f\n", rating_single);
        std::fprintf(f, "rating_double=%.4f\n", rating_double);
        std::fclose(f);

        // --- scores.cfg ---
        std::string scores_path = dir_path + "/scores.cfg";
        f = std::fopen(scores_path.c_str(), "w");
        if (!f) return false;
        for (const auto& [key, sc] : high_scores) {
            std::fprintf(f, "score|%s|%.4f|%.4f|%d|%s|%d|%.4f|%s\n",
                key.c_str(), sc.percentage, sc.ex_score,
                sc.stars, sc.grade.c_str(), sc.max_combo, sc.rating,
                sc.chart_mode.c_str());
        }
        std::fclose(f);
        return true;
    }

    /// Load profile from a directory. Returns true if profile.cfg was found.
    bool Load(const std::string& dir_path) {
        std::string profile_path = dir_path + "/profile.cfg";
        std::FILE* f = std::fopen(profile_path.c_str(), "r");
        if (!f) return false;

        source_path = dir_path;
        is_guest = false;

        char line[256];
        while (std::fgets(line, sizeof(line), f)) {
            char key[64], val[192];
            if (std::sscanf(line, "%63[^=]=%191[^\n]", key, val) == 2) {
                std::string skey = key;
                if (skey == "name")             name = val;
                else if (skey == "speed_mod")       speed_mod = std::atof(val);
                else if (skey == "mod_type")         mod_type = std::atoi(val);
                else if (skey == "downscroll")       downscroll = (std::atoi(val) != 0);
                else if (skey == "audio_offset")     audio_offset = std::atof(val);
                else if (skey == "noteskin")          noteskin = val;
                else if (skey == "effect_mode")      effect_mode = std::atoi(val);
                else if (skey == "life_type")        life_type = std::atoi(val);
                else if (skey == "flare_level")      flare_level = std::atoi(val);
                else if (skey == "rating_single")    rating_single = std::atof(val);
                else if (skey == "rating_double")    rating_double = std::atof(val);
                else if (skey == "overall_rating") {
                    // Legacy compat: map old overall_rating to single
                    rating_single = std::atof(val);
                }
            }
        }
        std::fclose(f);

        // --- Load scores ---
        std::string scores_path = dir_path + "/scores.cfg";
        f = std::fopen(scores_path.c_str(), "r");
        if (f) {
            while (std::fgets(line, sizeof(line), f)) {
                // Format: score|key|percentage|ex_score|stars|grade|max_combo|rating|chart_mode
                char prefix[16], skey[128], grade[16], mode[32];
                double pct, ex, rating_val;
                int stars, mc;
                mode[0] = '\0';
                int parsed = std::sscanf(line, "%15[^|]|%127[^|]|%lf|%lf|%d|%15[^|]|%d|%lf|%31[^\n]",
                    prefix, skey, &pct, &ex, &stars, grade, &mc, &rating_val, mode);
                if (parsed >= 7 && std::string(prefix) == "score") {
                    ProfileScore ps = { pct, ex, stars, grade, mc, 0.0, "" };
                    if (parsed >= 8) ps.rating = rating_val;
                    if (parsed >= 9) ps.chart_mode = mode;
                    high_scores[skey] = ps;
                }
            }
            std::fclose(f);
        }
        return true;
    }

    /// Reset to guest defaults
    void Reset() {
        name = "GUEST";
        source_path.clear();
        is_guest = true;
        is_usb = false;
        speed_mod = 2.0;
        mod_type = 0;
        downscroll = false;
        audio_offset = 0.0;
        noteskin = "Default";
        effect_mode = 0;
        life_type = 0;
        flare_level = 1;
        rating_single = 0.0;
        rating_double = 0.0;
        high_scores.clear();
    }

    // ========================================================================
    // Local Profile Management
    // ========================================================================

    /// Scan a directory for local profile subdirectories.
    /// Returns list of profile names (directory names that contain profile.cfg).
    static std::vector<std::string> ScanLocalProfiles(const std::string& profiles_dir = "profiles") {
        std::vector<std::string> names;
        if (!fs::exists(profiles_dir) || !fs::is_directory(profiles_dir)) return names;
        for (const auto& entry : fs::directory_iterator(profiles_dir)) {
            if (!entry.is_directory()) continue;
            std::string cfg = entry.path().string() + "/profile.cfg";
            if (fs::exists(cfg)) {
                names.push_back(entry.path().filename().string());
            }
        }
        std::sort(names.begin(), names.end());
        return names;
    }

    /// Create a new local profile with the given name.
    /// Returns true if successful, false if name is empty or already exists.
    static bool CreateLocalProfile(const std::string& profile_name,
                                   const std::string& profiles_dir = "profiles") {
        if (profile_name.empty()) return false;
        std::string dir = profiles_dir + "/" + profile_name;
        if (fs::exists(dir)) return false; // Already exists

        Profile p;
        p.name = profile_name;
        p.is_guest = false;
        p.source_path = dir;
        return p.Save(dir);
    }

    /// Get the full directory path for a local profile name.
    static std::string GetLocalProfilePath(const std::string& profile_name,
                                           const std::string& profiles_dir = "profiles") {
        return profiles_dir + "/" + profile_name;
    }

    // ========================================================================
    // USB Detection
    // ========================================================================

    /// Check if a drive letter contains a valid SM-Legends profile.
    static bool CheckForProfile(char drive_letter) {
        std::string path = std::string(1, drive_letter) + ":/SM-Legends/profile.cfg";
        return fs::exists(path);
    }

    /// Get the profile directory path for a given drive letter.
    static std::string GetUSBProfilePath(char drive_letter) {
        return std::string(1, drive_letter) + ":/SM-Legends";
    }
};

} // namespace sml

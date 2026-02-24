// ============================================================================
// SongScanner.cpp 窶・Recursive simfile scanner implementation
// ============================================================================

#include "SongScanner.h"

#include <algorithm>
#include <cstdio>
#include <cctype>
#include <filesystem>
#include <map>

namespace fs = std::filesystem;

namespace sml {

int SongScanner::ScanDirectory(const std::string& root_path) {
    songs_.clear();

    if (!fs::exists(root_path) || !fs::is_directory(root_path)) {
        last_error_ = "Directory not found: " + root_path;
        std::printf("SongScanner: %s\n", last_error_.c_str());
        return 0;
    }

    // Collect all .sm / .ssc files
    std::vector<std::string> files;
    CollectFiles(root_path, files);

    std::printf("SongScanner: Found %zu simfiles in '%s'\n", files.size(), root_path.c_str());

    // Parse each file for metadata
    for (const auto& filepath : files) {
        auto simfile = parser_.LoadFromFile(filepath);
        if (!simfile) {
            std::printf("  SKIP: %s (%s)\n", filepath.c_str(), parser_.GetLastError().c_str());
            continue;
        }

        SongEntry entry;
        entry.filepath  = filepath;
        entry.directory = fs::path(filepath).parent_path().string();
        entry.title     = simfile->title.empty() ? fs::path(filepath).stem().string() : simfile->title;
        entry.artist    = simfile->artist;
        entry.background_path = simfile->background_path.empty() ? "" : (fs::path(entry.directory) / simfile->background_path).string();
        entry.jacket_path = SimfileParser::FindJacket(entry.directory, {
            simfile->jacket_path,
            simfile->cover_path,
            simfile->banner_path,
            simfile->background_path
        });
        entry.offset    = simfile->offset;
        entry.num_charts = static_cast<int>(simfile->charts.size());

        // Filter out unsupported charts before extracting summaries
        simfile->charts.erase(std::remove_if(simfile->charts.begin(), simfile->charts.end(), [](const auto& chart) {
            return chart.chart_type != "dance-single" &&
                   chart.chart_type != "dance-double" &&
                   chart.chart_type != "dance-couple" &&
                   chart.chart_type != "dance-routine";
        }), simfile->charts.end());

        // Sort charts by difficulty meter so they display from easiest to hardest
        std::sort(simfile->charts.begin(), simfile->charts.end(), [](const auto& a, const auto& b) {
            return a.difficulty_meter < b.difficulty_meter;
        });

        // Extract chart summaries
        for (const auto& chart : simfile->charts) {

            SongEntry::ChartInfo info;
            info.chart_type       = chart.chart_type;
            info.difficulty_name  = chart.difficulty_name;
            info.difficulty_meter = chart.difficulty_meter;
            info.custom_difficulty = chart.custom_difficulty;
            info.credit = chart.credit;
            info.radar = chart.radar;
            info.variant = chart.variant;
            info.variant_kanji = chart.variant_kanji;
            info.num_notes = chart.GetTotalTaps();
            entry.charts.push_back(std::move(info));
        }

        // If song has no valid dance charts, completely ignore it
        if (entry.charts.empty()) continue;

        // Store the parsed simfile for immediate use
        entry.simfile = std::move(simfile);

        songs_.push_back(std::move(entry));
    }

    // Sort by title (case-insensitive)
    std::sort(songs_.begin(), songs_.end(), [](const SongEntry& a, const SongEntry& b) {
        std::string ta = a.title, tb = b.title;
        for (auto& c : ta) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        for (auto& c : tb) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return ta < tb;
    });

    std::printf("SongScanner: %zu songs loaded successfully\n", songs_.size());
    return static_cast<int>(songs_.size());
}

bool SongScanner::EnsureLoaded(size_t index) {
    if (index >= songs_.size()) return false;
    auto& entry = songs_[index];

    // Already loaded
    if (entry.simfile) return true;

    // Re-parse the full file
    entry.simfile = parser_.LoadFromFile(entry.filepath);
    if (!entry.simfile) {
        last_error_ = "Failed to reload: " + entry.filepath;
        return false;
    }

    // Filter out unsupported charts to match what we did in ScanDirectory
    entry.simfile->charts.erase(std::remove_if(entry.simfile->charts.begin(), entry.simfile->charts.end(), [](const auto& chart) {
        return chart.chart_type != "dance-single" &&
               chart.chart_type != "dance-double" &&
               chart.chart_type != "dance-couple" &&
               chart.chart_type != "dance-routine";
    }), entry.simfile->charts.end());

    // Sort charts by difficulty meter so they display from easiest to hardest
    std::sort(entry.simfile->charts.begin(), entry.simfile->charts.end(), [](const auto& a, const auto& b) {
        return a.difficulty_meter < b.difficulty_meter;
    });

    return true;
}

void SongScanner::CollectFiles(const std::string& dir_path, std::vector<std::string>& out_files) {
    // Group files by their parent directory to detect duplicates (.sm + .ssc)
    std::map<std::string, std::vector<std::string>> dir_groups;

    try {
        for (const auto& entry : fs::recursive_directory_iterator(
            dir_path, fs::directory_options::skip_permission_denied))
        {
            if (!entry.is_regular_file()) continue;

            std::string ext = entry.path().extension().string();
            for (auto& c : ext) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

            if (ext == ".sm" || ext == ".ssc") {
                dir_groups[entry.path().parent_path().string()].push_back(entry.path().string());
            }
        }

        // For each directory, prioritize .ssc files over .sm files
        for (const auto& [dir, files] : dir_groups) {
            bool has_ssc = std::any_of(files.begin(), files.end(), [](const std::string& f) {
                std::string ext = fs::path(f).extension().string();
                for (auto& c : ext) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                return ext == ".ssc";
            });

            for (const auto& f : files) {
                if (has_ssc) {
                    std::string ext = fs::path(f).extension().string();
                    for (auto& c : ext) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                    // Skip .sm files if at least one .ssc exists in this folder
                    if (ext == ".sm") continue;
                }
                out_files.push_back(f);
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::printf("SongScanner: filesystem error: %s\n", e.what());
    }
}

} // namespace sml

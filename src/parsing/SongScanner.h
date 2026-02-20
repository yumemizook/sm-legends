#pragma once
// ============================================================================
// SongScanner.h 窶・Recursive directory scanner for .sm / .ssc simfiles
//
// Scans a directory tree and lazily parses simfile headers (metadata only)
// to build a song list for the song select screen.
// ============================================================================

#include <string>
#include <vector>
#include <memory>
#include "parsing/Simfile.h"
#include "parsing/SimfileParser.h"

namespace sml {

/// A discovered song entry with metadata and file path.
struct SongEntry {
    std::string filepath;           ///< Full path to the .sm / .ssc file
    std::string directory;          ///< Parent directory of the file
    std::string title;
    std::string artist;
    std::string jacket_path;        ///< Path to the jacket, cover, or banner image
    double      offset = 0.0;
    int         num_charts = 0;

    /// Per-chart summary for the song select list.
    struct ChartInfo {
        std::string chart_type;
        std::string difficulty_name;
        int         difficulty_meter = 0;
        double      custom_difficulty = 1.0; ///< 1.0-30.0 scale
        std::string credit;
        RadarValues radar;
        ChartVariant variant = ChartVariant::Normal;
        std::string variant_kanji;
        int         num_notes = 0;

        bool Is8Lane() const {
            return chart_type == "dance-double" || chart_type == "dance-routine" || chart_type == "dance-couple";
        }
    };
    std::vector<ChartInfo> charts;

    /// The fully parsed simfile (loaded on demand when user selects it).
    std::unique_ptr<Simfile> simfile;
};

/// Scans directories for StepMania simfiles.
class SongScanner {
public:
    SongScanner() = default;

    /// Recursively scan a directory for .sm and .ssc files.
    /// Parses each file to extract metadata and chart info.
    /// Returns the number of songs found.
    int ScanDirectory(const std::string& root_path);

    /// Get all discovered songs (sorted by title).
    [[nodiscard]] const std::vector<SongEntry>& GetSongs() const { return songs_; }

    /// Get a mutable reference to a song entry (for lazy-loading the full simfile).
    [[nodiscard]] SongEntry& GetSong(size_t index) { return songs_[index]; }

    /// Get the total number of discovered songs.
    [[nodiscard]] size_t GetSongCount() const { return songs_.size(); }

    /// Ensure a song's full simfile is loaded (lazy load).
    /// Returns true if the simfile is ready.
    bool EnsureLoaded(size_t index);

    /// Get the last error message.
    [[nodiscard]] const std::string& GetLastError() const { return last_error_; }

private:
    /// Collect all .sm / .ssc file paths under a directory.
    void CollectFiles(const std::string& dir_path, std::vector<std::string>& out_files);

    std::vector<SongEntry> songs_;
    SimfileParser parser_;
    std::string last_error_;
};

} // namespace sml

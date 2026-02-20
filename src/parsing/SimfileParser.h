#pragma once
// ============================================================================
// SimfileParser.h 窶・Parser for StepMania .sm and .ssc simfile formats
//
// Handles both legacy .sm (global timing only) and modern .ssc (per-chart
// split timing) formats. Produces a fully populated Simfile structure.
// ============================================================================

#include <string>
#include <memory>
#include <vector>
#include "parsing/Simfile.h"

namespace sml {

/// Detected simfile format.
enum class SimfileFormat {
    SM,     ///< Legacy .sm format
    SSC,    ///< Modern .ssc format with split timing support
    Unknown
};

/// Parser for StepMania simfile formats (.sm and .ssc).
///
/// Usage:
///   SimfileParser parser;
///   auto simfile = parser.LoadFromFile("song.ssc");
///   if (simfile) { /* use simfile */ }
class SimfileParser {
public:
    SimfileParser() = default;

    /// Load and parse a simfile from the given path.
    /// Returns a populated Simfile on success, nullptr on failure.
    [[nodiscard]] std::unique_ptr<Simfile> LoadFromFile(const std::string& filepath);

    /// Parse simfile data from a string with explicit format.
    [[nodiscard]] std::unique_ptr<Simfile> LoadFromString(
        const std::string& data,
        SimfileFormat format
    );

    /// Get the last error message if parsing failed.
    [[nodiscard]] const std::string& GetLastError() const { return last_error_; }

    /// Find a jacket or cover image in the given directory.
    static std::string FindJacket(const std::string& directory, const std::vector<std::string>& metadata_hints);

private:
    /// Detect format from file extension.
    static SimfileFormat DetectFormat(const std::string& filepath);

    /// Parse the global header tags (metadata, timing).
    void ParseGlobalTags(
        const std::string& data,
        Simfile& simfile,
        SimfileFormat format
    );

    /// Parse #NOTES blocks in .sm format.
    void ParseSMNotes(const std::string& data, Simfile& simfile);

    /// Parse #NOTEDATA blocks in .ssc format (with split timing).
    void ParseSSCNoteData(const std::string& data, Simfile& simfile);

    /// Parse a beat=value,beat=value,... timing tag into TimingSegments.
    static std::vector<TimingSegment> ParseTimingTag(
        const std::string& value,
        TimingSegmentType type
    );

    /// Parse #SPEEDS tag (beat=value=duration=unit,...) into TimingSegments.
    static std::vector<TimingSegment> ParseSpeedsTag(const std::string& value);

    /// Parse #FAKES tag (beat=length,...) into FakeSegments.
    static std::vector<FakeSegment> ParseFakesTag(const std::string& value);

    /// Parse #ATTACKS tag (TIME=t:LEN=l:MODS=m,...) into Attacks.
    static std::vector<Attack> ParseAttacksTag(const std::string& value);

    /// Parse #BGCHANGES tag (beat=file=rate=trans,...) into BGEvents.
    static std::vector<BGEvent> ParseBGChangesTag(const std::string& value);

    /// Parse note data string into NoteRows.
    /// The data is the raw grid text (e.g. "0000\n1000\n..." with measures
    /// separated by commas).
    static std::vector<NoteRow> ParseNoteData(
        const std::string& note_text,
        int num_columns
    );

    /// Extract the value of a #TAG:VALUE; pair. Returns empty if not found.
    static std::string ExtractTagValue(
        const std::string& data,
        const std::string& tag,
        size_t search_start = 0
    );

    /// Trim whitespace from both ends of a string.
    static std::string Trim(const std::string& s);

    std::string last_error_;
};

} // namespace sml

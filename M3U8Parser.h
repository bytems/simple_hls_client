/*
 *          Module responsible for parsing HLS master playlist file
 */

#ifndef HLS_FETCH_AND_SORT_M3U8PARSER_H
#define HLS_FETCH_AND_SORT_M3U8PARSER_H

#include "StreamInfParser.h"
#include "MediaParser.h"
#include "iFrameParser.h"

/**
 * @brief Enum representing the type of sub-parser available in an M3U8Parser.
 */
enum class ParserType {
    STREAM,
    AUDIO,
    IFRAME
};

// Forward declaration of ParserAccessor helper friend class.
template <ParserType T>
class ParserAccessor;

/**
 * @brief Main class for parsing HLS master playlists.
 *
 * This class holds sub-parsers for stream variants, audio tracks, and iFrames.
 * The user can access a specific sub-parser via the select() method which returns
 * a ParserAccessor, a lightweight proxy that forwards calls (like sort()) to the
 * appropriate sub-parser.
 */
class M3U8Parser {
private:
    std::vector<std::string> headers_;
    StreamInfParser  stream_parser_;
    MediaParser      audio_parser_;
    iFrameParser     iframe_parser_;

    // Grant ParserAccessor access to private members.
    template<ParserType T>
    friend class ParserAccessor;

public:
    /**
     * @brief Parses the provided M3U8 content.
     * @param content The full M3U8 file content.
     * @throws std::runtime_error if the file does not start with the expected header.
     */
    void parse(const std::string& content) {
        std::istringstream stream(content);
        std::string line;

        // Verify & acquire header
        std::getline(stream, line);
        if (line.find("#EXTM3U") == std::string::npos) {
            throw std::runtime_error("Invalid M3U8 file - missing #EXTM3U header");
        }
        else{
            headers_.emplace_back(line);
        }

        std::getline(stream, line);
        if (line.find("#EXT-X-INDEPENDENT-SEGMENTS") != std::string::npos) {
            headers_.emplace_back(line);
        }

        // Parse content using sub-parsers
        stream_parser_.parse(content);
        audio_parser_.parse(content);
        iframe_parser_.parse(content);
    }

    /**
     * @brief Provides access to a specific sub-parser.
     *
     * This template function returns a ParserAccessor (proxy) for the sub-parser
     * designated by the ParserType template parameter. For example:
     *
     *     auto streamAcc = parser.select<ParserType::STREAM>();
     *     streamAcc.sort(HLSTagParser::SortAttribute::...);
     *
     * @tparam T The type of sub-parser to access (STREAM, AUDIO, or IFRAME).
     * @return ParserAccessor<T> proxy object for the chosen sub-parser.
     */
    template<ParserType T>
    ParserAccessor<T> select(){
        return ParserAccessor<T>(*this);
    }

    /*
    void sortVariants(HLSTagParser::SortAttribute attr){
        stream_parser_.sortByAttribute(attr); }

    void sortVariants(HLSTagParser::SortAttribute primary,HLSTagParser::SortAttribute secondary){
        stream_parser_.sortByAttribute(primary, secondary); }

    void sortAudioTracks(HLSTagParser::SortAttribute attr){
        audio_parser_.sortByAttribute(attr);  }

    void sortAudioTracks(HLSTagParser::SortAttribute primary,HLSTagParser::SortAttribute secondary){
        audio_parser_.sortByAttribute(primary, secondary);  }

    void sortIFrames(HLSTagParser::SortAttribute attr){
        iframe_parser_.sortByAttribute(attr);  }

    void sortIFrames(HLSTagParser::SortAttribute primary,HLSTagParser::SortAttribute secondary){
        iframe_parser_.sortByAttribute(primary, secondary);  }
    */

    std::string stringify() const {
        std::string manifest;
        for (const auto &header: headers_) {
            manifest += header + "\n";
        }
        manifest += "\n";
        for (const auto &variant: stream_parser_.variants_) {
            manifest += variant.manifest_line + "\n";
            manifest += variant.uri + "\n";
        }
        manifest += "\n";
        for (const auto &track: audio_parser_.audio_tracks_) {
            manifest += track.manifest_line + "\n";
        }
        manifest += "\n";
        for (const auto &iframe: iframe_parser_.iframes_) {
            manifest += iframe.manifest_line + "\n";
        }
        manifest += "\n";
        return manifest;
    }
};


/**
 * @brief Proxy class to access and control a specific sub-parser within M3U8Parser.
 *
 * The ParserAccessor acts as a lightweight interface to one of the sub-parsers
 * (stream, audio, or iFrame). It forwards operations (e.g., sort) to the
 * underlying sub-parser.
 *
 * @tparam T The type of sub-parser to access.
 */
template<ParserType T>
class ParserAccessor {
private:
    M3U8Parser& parser_;

    // Returns a reference to the correct sub-parser based on the template parameter.
    auto& getParser() const {
        if constexpr (T == ParserType::STREAM)      {return parser_.stream_parser_;}
        else if constexpr (T == ParserType::AUDIO)  {return parser_.audio_parser_;}
        else if constexpr (T == ParserType::IFRAME) {return parser_.iframe_parser_;}
    }
public:
    //  Construct a ParserAccessor for the specified M3U8Parser instance.
    explicit ParserAccessor(M3U8Parser& parser) : parser_(parser) {}

    // forwards the sort request to the underlying sub-parser
    void sort(HLSTagParser::SortAttribute attr) {
        getParser().sortByAttribute(attr);
    }

    void sort(HLSTagParser::SortAttribute primary, HLSTagParser::SortAttribute secondary) {
        getParser().sortByAttribute(primary, secondary);
    }

};

#endif //HLS_FETCH_AND_SORT_M3U8PARSER_H

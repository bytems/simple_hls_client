
#ifndef HLS_FETCH_AND_SORT_MEDIAPARSER_H
#define HLS_FETCH_AND_SORT_MEDIAPARSER_H

#include "HLSTagParser.h"

// Tag-specific line & data attributes
struct MediaGroup{
    //std::string type;
    std::string id;
    std::string name;
    std::string language;
    std::string default_;
    std::string autoselect;
    int         channel_count;
    std::string uri;
    std::string manifest_line;
};

// Concrete Media sub-parser
class MediaParser : public HLSTagParserSorter<MediaParser, MediaGroup> {
public:
    std::vector<MediaGroup> audio_tracks_;

    void parse(const std::string& content) override{
        MediaGroup cur_audio_trk;
        std::istringstream ss(content);
        std::string line;


        while(getline(ss, line)){
            if(line.find("#EXT-X-MEDIA:") != std::string::npos){
                cur_audio_trk = MediaGroup();
                cur_audio_trk.manifest_line = line;

                cur_audio_trk.uri        = extractAttribute(line,"URI");
                cur_audio_trk.id         = extractAttribute(line,"GROUP-ID");
                cur_audio_trk.name       = extractAttribute(line,"NAME");
                cur_audio_trk.autoselect = extractAttribute(line,"AUTOSELECT");
                std::string channelType  = extractAttribute(line,"CHANNELS");
                cur_audio_trk.default_   = extractAttribute(line,"DEFAULT");
                cur_audio_trk.language   = extractAttribute(line,"LANGUAGE");

                // Extract channel count from Channels string
                size_t slash_pos = channelType.find('/');
                if(slash_pos == std::string::npos)  cur_audio_trk.channel_count = stoi(channelType);
                else cur_audio_trk.channel_count = stoi(channelType.substr(0,slash_pos));

                audio_tracks_.emplace_back(cur_audio_trk);
            }
        }
    }

    // provide access to the container
    std::vector<MediaGroup>& getContainer() { return audio_tracks_; }

    /*  Map of comparison functions for each attribute */
private:
    using ComparisonFunc = std::function<bool(const MediaGroup&, const MediaGroup)>;

    // Mapping between attributes and their comparators
    const std::unordered_map<SortAttribute, ComparisonFunc> comparisons_ = {
            {SortAttribute::ID, [](const MediaGroup& a, const MediaGroup& b){
                return a.id < b.id;
            }},
            {SortAttribute::NAME, [](const MediaGroup& a, const MediaGroup& b){
                return a.name < b.name;
            }},
            {SortAttribute::LANGUAGE, [](const MediaGroup& a, const MediaGroup& b){
                return a.language < b.language;
            }},
            {SortAttribute::DEFAULT_, [](const MediaGroup& a, const MediaGroup& b){
                return a.default_ < b.default_;
            }},
            {SortAttribute::AUTOSELECT, [](const MediaGroup& a, const MediaGroup& b){
                return a.autoselect < b.autoselect;
            }},
            {SortAttribute::CHANNELS, [](const MediaGroup& a, const MediaGroup& b){
                return a.channel_count < b.channel_count;
            }}
    };

public:
    // provide access to the comparisons map
    const std::unordered_map<SortAttribute, ComparisonFunc>& getComparisons() const {
        return comparisons_;
    }
};

#endif //HLS_FETCH_AND_SORT_MEDIAPARSER_H

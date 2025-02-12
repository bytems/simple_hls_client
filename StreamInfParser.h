//
// Created by mohamed saleh on 2/11/25.
//

#ifndef HLS_FETCH_AND_SORT_STREAMINFPARSER_H
#define HLS_FETCH_AND_SORT_STREAMINFPARSER_H

#include "HLSTagParser.h"

struct VideoStreamVariant {
    int         bandwidth;
    int         avg_bandwidth;
    std::string codecs;
    int         resolution_height;
    std::string frame_rate;
    std::string video_range;
    std::string audio;
    std::string closed_captions;
    std::string uri;
    std::string manifest_line;
};

// Concrete Video Variation sub-parser
class StreamInfParser : public HLSTagParserSorter<StreamInfParser, VideoStreamVariant> {
public:
    std::vector<VideoStreamVariant> variants_;

    void parse(const std::string& content) override {
        std::istringstream ss(content);
        std::string line;

        VideoStreamVariant current_variant;
        bool expecting_uri = false;

        while(getline(ss, line)){

            if (line.find("#EXT-X-STREAM-INF:") != std::string::npos) {
                current_variant = VideoStreamVariant();
                current_variant.manifest_line = line;

                current_variant.bandwidth        = stoi(extractAttribute(line, "BANDWIDTH"));
                current_variant.avg_bandwidth    = stoi(extractAttribute(line, "AVERAGE-BANDWIDTH"));
                current_variant.codecs           = extractAttribute(line, "CODECS");
                std::string resolution           = extractAttribute(line, "RESOLUTION");
                current_variant.frame_rate       = extractAttribute(line, "FRAME-RATE");
                current_variant.video_range      = extractAttribute(line, "VIDEO-RANGE");
                current_variant.audio            = extractAttribute(line, "AUDIO");
                current_variant.closed_captions  = extractAttribute(line, "CLOSED-CAPTIONS");
                current_variant.uri              = extractAttribute(line, "FRAME-RATE");

                // Extract  height from resolution string
                size_t x_pos = resolution.find('x');
                if(x_pos == std::string::npos) current_variant.resolution_height = 0;
                else  current_variant.resolution_height = stoi(resolution.substr(x_pos+1));

                expecting_uri =true;
            }
            else if (expecting_uri && line[0] != '#') {
                current_variant.uri = line;
                variants_.emplace_back(current_variant);
                expecting_uri = false;
            }
        }

        if (variants_.empty()) {
            throw std::runtime_error("No stream variants found in master playlist");
        }
    }

    // provide access to the container
    std::vector<VideoStreamVariant>& getContainer() { return variants_; }

    /*  Map of comparison functions for each attribute */
private:
    using ComparisonFunc = std::function<bool(const VideoStreamVariant&, const VideoStreamVariant&)>;
    // Mapping between attributes and their comparators
    const std::unordered_map<SortAttribute, ComparisonFunc> comparisons_ = {
            {SortAttribute::BANDWIDTH, [](const VideoStreamVariant& a, const VideoStreamVariant& b){
                return a.bandwidth < b.bandwidth;
            }},
            {SortAttribute::AVERAGE_BANDWIDTH, [](const VideoStreamVariant& a, const VideoStreamVariant& b){
                return a.avg_bandwidth < b.avg_bandwidth;
            }},
            {SortAttribute::CODECS, [](const VideoStreamVariant& a, const VideoStreamVariant& b){
                return a.codecs < b.codecs;
            }},
            {SortAttribute::RESOLUTION, [](const VideoStreamVariant& a, const VideoStreamVariant& b){
                return a.resolution_height < b.resolution_height;
            }},
            {SortAttribute::FRAME_RATE, [](const VideoStreamVariant& a, const VideoStreamVariant& b){
                return a.frame_rate < b.frame_rate;
            }},
            {SortAttribute::VIDEO_RANGE, [](const VideoStreamVariant& a, const VideoStreamVariant& b){
                return a.video_range < b.video_range;
            }},
            {SortAttribute::AUDIO, [](const VideoStreamVariant& a, const VideoStreamVariant& b){
                return a.audio < b.audio;
            }},
            {SortAttribute::CLOSED_CAPTIONS, [](const VideoStreamVariant& a, const VideoStreamVariant& b){
                return a.closed_captions < b.closed_captions;
            }}
    };

public:
    // provide access to the comparisons map
    const std::unordered_map<SortAttribute, ComparisonFunc>& getComparisons() const {
        return comparisons_;
    }
};



#endif //HLS_FETCH_AND_SORT_STREAMINFPARSER_H

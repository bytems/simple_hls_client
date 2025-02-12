#ifndef HLS_FETCH_AND_SORT_IFRAMEPARSER_H
#define HLS_FETCH_AND_SORT_IFRAMEPARSER_H

#include "HLSTagParser.h"

// Tag-specific line & data attributes
struct IFrame{
    int         bandwidth;
    std::string codecs;
    int         resolution_height;
    std::string video_range;
    std::string uri;
    std::string manifest_line;
};

// Concrete I-Frame sub-parser
class iFrameParser : public HLSTagParserSorter<iFrameParser,IFrame> {
public:
    std::vector<IFrame> iframes_;

    void parse(const std::string& content) override {
        IFrame cur_frame;
        std::istringstream iss(content);
        std::string line;

        while (std::getline(iss, line)) {
            if (line.find("#EXT-X-I-FRAME-STREAM-INF") != std::string::npos) {
                cur_frame = IFrame();
                cur_frame.manifest_line = line;

                cur_frame.bandwidth     = stoi(extractAttribute(line, "BANDWIDTH"));
                cur_frame.uri           = extractAttribute(line, "URI");
                cur_frame.video_range   = extractAttribute(line, "VIDEO_RANGE");
                cur_frame.codecs        = extractAttribute(line, "CODECS");

                // Extract height from resolution string
                std::string resolution = extractAttribute(line, "RESOLUTION_HEIGHT");
                size_t x_pos = resolution.find("x");
                if (x_pos == std::string::npos) cur_frame.resolution_height = 0;
                else cur_frame.resolution_height = stoi(resolution.substr(x_pos+1));

                iframes_.emplace_back(cur_frame);
            }
        }
    }

    //provide access to the container
    std::vector<IFrame>& getContainer() { return iframes_; }

    /*  Map of comparison functions for each attribute */
private:
    using ComparisonFunc = std::function<bool(const IFrame&, const IFrame&)>;
    // Mapping between attributes and their comparators
    const std::unordered_map<SortAttribute, ComparisonFunc> comparisons_ = {
            {SortAttribute::BANDWIDTH, [](const IFrame& a, const IFrame& b){
                return a.bandwidth < b.bandwidth;
            }},
            {SortAttribute::CODECS, [](const IFrame& a, const IFrame& b){
                return a.codecs < b.codecs;
            }},
            {SortAttribute::RESOLUTION, [](const IFrame& a, const IFrame& b){
                return a.resolution_height < b.resolution_height;
            }},
            {SortAttribute::VIDEO_RANGE, [](const IFrame& a, const IFrame& b){
                return a.video_range < b.video_range;
            }},
    };

public:
    // provide access to the comparisons map
    const std::unordered_map<SortAttribute, ComparisonFunc>& getComparisons() const {
        return comparisons_;
    }
};

#endif //HLS_FETCH_AND_SORT_IFRAMEPARSER_H

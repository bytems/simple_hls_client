/*
 *
 */

#include <iostream>
#include "HLSFetcher.h"
#include "HLSWriter.h"
#include "M3U8Parser.h"

int main() {
    try {
        // Create HLS fetcher and get the playlist
        const std::string url = "https://lw.bamgrid.com/2.0/hls/vod/bam/ms02/hls/dplus/bao/master_unenc_hdr10_all.m3u8";
        HLSFetcher fetcher(url);

        if (fetcher.fetch()) {
            std::cout << "Successfully fetched playlist:\n";

            // Create and use the parser to group by tag
            M3U8Parser parser;
            parser.parse(fetcher.getResponse());

            // Sort each tag group by attribute
            parser.select<ParserType::STREAM>().sort(HLSTagParser::SortAttribute::RESOLUTION,
                                                   HLSTagParser::SortAttribute::BANDWIDTH);
            parser.select<ParserType::AUDIO>().sort(HLSTagParser::SortAttribute::ID);
            parser.select<ParserType::IFRAME>().sort(HLSTagParser::SortAttribute::CODECS);

            // Create HLSWriter and write the (sorted) playlist to a file.
            HLSWriter writer("sorted_master_unenc_hdr10_maybe");
            writer.write(parser.stringify());
            std::cout << "Sorted playlist written to " << writer.getFileName() << std::endl;
        } else {
            std::cerr << "Failed to fetch playlist" << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
# Implementation Brainstorm notes

## Fetch file
Generate extensible block/class. Future use might include fetching the actual media transport stream

## Parser
Create a custom component for each tag type:

  - Top-level descriptive: (#EXTM3U,    #EXT-X-INDEPENDENT-SEGMENTS)
  - Audio Group (#EXT-X-MEDIA)
  - Video Variant (#EXT-X-STREAM-INF:)
  -  I-Frame Only Streams (EXT-X-I-FRAME-STREAM-INF:)

## Sort
Once you've created a structure to hold the stream information. We can use std::sort with a custom comparator
```C++
std::sort(streams.begin(), streams.end(), 
        [](const StreamInfo& a, const StreamInfo& b) {
            return a.bandwidth < b.bandwidth;
        });
```
on each component we generated in parser.

# Generate File
Dump new sorted content onto a new file. Dont overwrite the original.

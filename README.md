# Simple HLS Client

Mohamed Saleh's implementation of fetching an HLS playlist, parsing the document, and sorting it. 
A common use case in the Disney Streaming Services Video Platform Engineering group.

### My Goals

I wanted:
- the application to use powerful C++ features like templates, lambdas, and the STL.
- A clear separation of concerns between fetching, parsing, sorting, and writing functions.
- Extensible Design - new tag types can be supported by adding new parsers.
- RAII (Resource Acquisition Is Initialization) with appropriate constructors and destructors to handle resources like libcurl.
- An architecture that allows the HLS client to efficiently process HLS playlists while maintaining a clean, object-oriented design that separates distinct responsibilities into different components.

# Data Flow

Illustrated in the diagram below. The main function initializes an HLSFetcher to download the playlist.
The fetched content is passed to the M3U8Parser.
The parser delegates to sub-parsers to extract and organize different playlist elements.
Sorting is applied to each element type through the ParserAccessor.
The sorted playlist is serialized and written to a file using the HLSWriter.

```mermaid
stateDiagram-v2
    [*] --> Initialization: Start Application

    Initialization --> Fetching: Initialize HLSFetcher

    Fetching --> Error: Fetch Failed
    Fetching --> Parsing: Fetch Successful

    Parsing --> ProcessingStream: Process #EXT-X-STREAM-INF tags
    Parsing --> ProcessingAudio: Process #EXT-X-MEDIA tags
    Parsing --> ProcessingFrame: Process #EXT-X-I-FRAME-STREAM-INF tags

    ProcessingStream --> Sorting
    ProcessingAudio --> Sorting
    ProcessingFrame --> Sorting

    Sorting --> SortingStream: Sort stream variants
    Sorting --> SortingAudio: Sort audio tracks
    Sorting --> SortingiFrame: Sort I-frames

    SortingStream --> Serializing
    SortingAudio --> Serializing
    SortingiFrame --> Serializing

    Serializing --> Writing: Generate sorted playlist

    Writing --> Complete: Write to file
    Error --> [*]
    Complete --> [*]
```
# Structure

## Core Components
**HLSFetcher**: Handles HTTP requests using libcurl to retrieve HLS playlists from a URL. Provides methods to fetch content and retrieve response data

**M3U8Parser**: Central parsing component that coordinates the parsing of different HLS tag types. Contains three specialized sub-parsers, that can be accessed by a proxy.

**HLSWriter**: Handles writing the processed playlist content to a file.


```mermaid
classDiagram
    class HLSFetcher {
        -url_ : string
        -curl_ : CURL*
        -response_data_ : string
        +fetch() : bool
        +getResponse() : string&
    }
    
    class HLSWriter {
        -file_name_ : string
        +write(content: string) : void
        +getFileName() : string&
    }
    
    class HLSTagParser {
        <<abstract>>
        +enum SortAttribute
        +parse(content: string) : void
        +sortByAttribute(attr: SortAttribute) : void
        +sortByAttribute(attr1: SortAttribute, attr2: SortAttribute) : void
        #extractAttribute(line: string, attr: string) : string
    }
    
    class HLSTagParserSorter~Derived,Element~ {
        <<template>>
        +sortByAttribute(attr: SortAttribute) : void
        +sortByAttribute(primary: SortAttribute, secondary: SortAttribute) : void
    }
    
    class M3U8Parser {
        -headers_ : vector~string~
        -stream_parser_ : StreamInfParser
        -audio_parser_ : MediaParser
        -iframe_parser_ : iFrameParser
        +parse(content: string) : void
        +stringify() : string
        +select~ParserType~() : ParserAccessor~T~
    }
    
    class ParserAccessor~T~ {
        -parser_ : M3U8Parser&
        +sort(attr: SortAttribute) : void
        +sort(primary: SortAttribute, secondary: SortAttribute) : void
    }
    
    class StreamInfParser {
        +variants_ : vector~VideoStreamVariant~
        +parse(content: string) : void
    }
    
    class MediaParser {
        +audio_tracks_ : vector~MediaGroup~
        +parse(content: string) : void
    }
    
    class iFrameParser {
        +iframes_ : vector~IFrame~
        +parse(content: string) : void
    }
    
    HLSTagParser <|-- HLSTagParserSorter
    HLSTagParserSorter <|-- StreamInfParser
    HLSTagParserSorter <|-- MediaParser
    HLSTagParserSorter <|-- iFrameParser
    
    M3U8Parser o-- StreamInfParser
    M3U8Parser o-- MediaParser
    M3U8Parser o-- iFrameParser
    
    M3U8Parser -- ParserAccessor
    
    class Main {
        +main() : int
    }
    
    Main --> HLSFetcher : uses
    Main --> M3U8Parser : uses
    Main --> HLSWriter : uses

```

## Sub-Parser Architecture:
**HLSTagParser**: Abstract base class defining the interface for all tag parsers

**HLSTagParserSorter**: Template class using CRTP (Curiously Recurring Template Pattern) to provide common sorting functionality
Concrete parsers that implement specific parsing logic

**StreamInfParser**: Processes video stream variants (#EXT-X-STREAM-INF tags)

**MediaParser**: Processes audio tracks (#EXT-X-MEDIA tags)

**iFrameParser**: Processes I-Frame only streams (#EXT-X-I-FRAME-STREAM-INF tags)



## Design Patterns Used
- Template Method Pattern: The abstract HLSTagParser defines the interface while HLSTagParserSorter provides partial implementation.
- CRTP (Curiously Recurring Template Pattern): Used in HLSTagParserSorter to achieve static polymorphism.
- Proxy Pattern: The ParserAccessor acts as a proxy to forward operations to the appropriate sub-parser.
- Strategy Pattern: The comparator functions in each parser provide different sorting strategies.


# Limitations
-  I have only tested this on macOS. Cannot guarantee support for other platforms.
- Sorting is done in ascending order only

## Assumptions
- Only supporting tag types present in the given HLS playlist file.
- Fetched HLS playlist to remain unchanged. New sorted HLS playlist file is created.

## Prerequisites
* **CMake v3.15+** - found at [https://cmake.org/](https://cmake.org/)
* **C++ Compiler** - needs to support at least the **C++11** standard, i.e. *MSVC*, *GCC*, *Clang*
* **CURL** - found at [everything curl](https://ec.haxx.se/install/index.html)


# Building Instructions
Generate the build system using a **CMake** routine similar to the one below:
```bash
cmake -S <source_directory> -B <build_directory>
```
- -S: Specifies the path to the directory containing the top-level CMakeList.txt
- -B: Specifies the path to the directory where build files will be generated

To build the project, all you need to do is run a **CMake** routine similar to the one below:
```bash
cd <build_directory>
cmake --build .
```

Alternatively, many IDEs have built-in support for CMake.

## Running Instructions


```bash
hls_fetch_and_sort
```
No command line options supported
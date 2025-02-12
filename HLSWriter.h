#ifndef HLSWRITER_H
#define HLSWRITER_H

#include <fstream>
// Simple file writer for HLS playlists
class HLSWriter {
private:
    std::string file_name_;

    static std::string ensureExtension(const std::string& filename) {
        if (filename.size() < 5 || filename.substr(filename.size() - 5) != ".m3u8") {
            return filename + ".m3u8";
        }
        return filename;
    }

public:
    explicit HLSWriter(const std::string& filename)
        : file_name_(ensureExtension(filename)) {}

    void write(const std::string& content) {
        std::ofstream outFile(file_name_, std::ios::out | std::ios::trunc);
        if (!outFile) throw std::runtime_error("Could not open file " + file_name_ + " for writing");
        outFile << content;
    }

    const std::string& getFileName() const {
        return file_name_;
    }
};
#endif //HLSWRITER_H

/*
 *   Module responsible for fetching files from websites
 */
#ifndef HLS_FETCH_AND_SORT_FETCH_H
#define HLS_FETCH_AND_SORT_FETCH_H

#include <curl/curl.h>
#include <string>

// Callback function to handle data received from curl
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    // Calculate the size of the data
    size_t totalSize = size * nmemb;

    // Append the data to the string passed via userp
    userp->append((char*)contents, totalSize);

    // Return the number of bytes processed
    return totalSize;
}

class HLSFetcher {
private:
    std::string url_;
    CURL* curl_;
    std::string response_data_;

public:
    explicit HLSFetcher(const std::string& url) : url_(url) {
        // Initialize global curl environment
        curl_global_init(CURL_GLOBAL_ALL);

        curl_ = curl_easy_init();
        if (!curl_) {
            throw std::runtime_error("Failed to initialize CURL");
        }
    }

    ~HLSFetcher() {
        if (curl_) {
            curl_easy_cleanup(curl_);
        }
        // Cleanup global curl environment
        curl_global_cleanup();
    }

    bool fetch() {
        if (!curl_) return false;

        // Reset response data
        response_data_.clear();

        // Set URL
        curl_easy_setopt(curl_, CURLOPT_URL, url_.c_str());

        // Set callback function
        curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response_data_);

        // Follow redirects
        curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);

        // Set timeout (10 seconds)
        curl_easy_setopt(curl_, CURLOPT_TIMEOUT, 10L);

        // Perform the request
        CURLcode res = curl_easy_perform(curl_);

        if (res != CURLE_OK) {
            std::cerr << "Failed to fetch playlist: "
                      << curl_easy_strerror(res) << std::endl;
            return false;
        }

        long http_code = 0;
        curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &http_code);

        return http_code == 200;
    }

    const std::string& getResponse() const {
        return response_data_;
    }
};




#endif //HLS_FETCH_AND_SORT_FETCH_H

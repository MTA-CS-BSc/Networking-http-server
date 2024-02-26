#ifndef URI_H_
#define URI_H_

#include <string>
#include <cctype>
#include <utility>
#include <algorithm>

namespace mta_http_server {
    // A Uri object will contain a valid scheme (for example: HTTP), host,
    // port, and the actual URI path
    class Uri {
    private:
        // Only the path is supported for now
        std::string path_;
        std::string scheme_;
        std::string host_;
        unsigned int port_;

        void SetPathToLowercase() {
            std::transform(path_.begin(), path_.end(), path_.begin(),
                           [](char c) { return tolower(c); });
        }
    public:
        Uri() = default;
        ~Uri() = default;
        explicit Uri(std::string path):path_(std::move(path)) { SetPathToLowercase(); }

        inline bool operator<(const Uri& other) const { return path_ < other.path_; }
        inline bool operator==(const Uri& other) const {
            return path_ == other.path_;
        }

        void SetPath(const std::string& path) {
            path_ = path;
            SetPathToLowercase();
        }

        std::string scheme() const { return scheme_; }
        std::string host() const { return host_; }
        unsigned int port() const { return port_; }
        std::string path() const { return path_; }
    };

}
#endif  // URI_H_
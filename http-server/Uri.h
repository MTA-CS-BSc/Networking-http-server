#ifndef URI_H_
#define URI_H_

#include <string>
#include <algorithm>

namespace mta_http_server {
    // A Uri object will contain the actual URI path
    class Uri {
    private:
        std::string path_;

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

        std::string path() const { return path_; }
    };

}
#endif  // URI_H_
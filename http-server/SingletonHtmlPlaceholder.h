#ifndef SINGLETON_HTML_PLACEHOLDER_H_
#define SINGLETON_HTML_PLACEHOLDER_H_
#include <string>

class SingletonHtmlPlaceholder {
public:
    static SingletonHtmlPlaceholder& getInstance() {
        static SingletonHtmlPlaceholder instance;
        return instance;
    }

    const std::string& get() const {
        return value;
    }

    void set(const std::string& newValue) {
        value = newValue;
    }

private:
    SingletonHtmlPlaceholder() { }  // Private constructor to prevent instantiation
    SingletonHtmlPlaceholder(const SingletonHtmlPlaceholder&) = delete; // Delete copy constructor
    SingletonHtmlPlaceholder& operator=(const SingletonHtmlPlaceholder&) = delete; // Delete assignment operator

    std::string value;
};

#endif //SINGLETON_HTML_PLACEHOLDER_H_
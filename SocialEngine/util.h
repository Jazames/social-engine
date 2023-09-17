#pragma once
#ifndef UTIL_H
#define UTIL_H

#include <string>


std::string sanitize_naughty_words(const std::string& str);

template<typename T, void(*deallocatorFunc)(T*), typename AllocatorFunc, typename... Args>
class DeallocatingWrapper {
private:
    T* resource;

public:
    explicit DeallocatingWrapper(AllocatorFunc&& allocatorFunc, Args... args)
        : resource(allocatorFunc(std::forward<Args>(args)...)) {}

    // Rule of 5.
    ~DeallocatingWrapper() {
        deallocatorFunc(resource);
    }

    // Delete the copy constructor and copy assignment operator to prevent double deallocation.
    DeallocatingWrapper(const DeallocatingWrapper&) = delete;
    DeallocatingWrapper& operator=(const DeallocatingWrapper&) = delete;

    // Optional: implement move semantics if desired.

    T* get() const {
        return resource;
    }
};

#endif //UTIL_H
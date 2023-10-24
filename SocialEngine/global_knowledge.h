#pragma once

#include <array>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>

#include "embed.h"



class GlobalKnowledge {
public:
    static GlobalKnowledge& get_instance() {
        static GlobalKnowledge instance;  // Guaranteed to be destroyed; Instantiated on first use
        return instance;
    }
    
    // Disallow copying and assignment
    GlobalKnowledge(const GlobalKnowledge&) = delete;
    void operator=(const GlobalKnowledge&) = delete;

    int add_knowledge(const std::string& phrase);

    // Example method to retrieve a mapping by index
    const std::string get_knowledge(size_t index) const;

    const std::vector<std::string> get_closest_items(std::string phrase, int num_items) const;

private:
    GlobalKnowledge() = default;  // Private constructor

    struct Mapping {
        std::string phrase;
        std::array<float, EMBEDDING_SIZE> embedding;
    };

    float cosine_similarity(const std::array<float, EMBEDDING_SIZE>& a, const std::array<float, EMBEDDING_SIZE>& b) const;

    std::vector<Mapping> mappings;
public:
    void reset();
};
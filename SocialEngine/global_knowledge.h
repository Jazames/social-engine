#pragma once

#include <array>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>




class GlobalKnowledge {
public:
    static GlobalKnowledge& get_instance() {
        static GlobalKnowledge instance;  // Guaranteed to be destroyed; Instantiated on first use
        return instance;
    }

	void deinit() {
		GlobalKnowledge::~GlobalKnowledge();
	}
    
    // Disallow copying and assignment
    GlobalKnowledge(const GlobalKnowledge&) = delete;
    void operator=(const GlobalKnowledge&) = delete;

    int add_knowledge(const std::string& phrase);

    // Example method to retrieve a mapping by index
    const std::string get_knowledge(size_t index) const;

    const std::vector<std::string> get_closest_items(std::string phrase, int num_items) const;
    const std::vector<std::string> get_closest_items(std::string phrase, std::vector<int> knowledge_ids, int num_items) const;
    const float get_similarity(std::string phrase1, std::string phrase2) const;

private:
    GlobalKnowledge() = default;  // Private constructor
	~GlobalKnowledge();

    struct Mapping {
        std::string phrase;
        std::vector<float> embedding;
    };

    const std::vector<std::string> get_closest_items(std::string phrase, std::vector<Mapping> knowledge_mappings, int num_items) const;
    float cosine_similarity(const std::vector<float>& a, const std::vector<float>& b) const;
    float euclidian_distance(const std::vector<float>& vec1, const std::vector<float>& vec2) const;
    std::vector<float> normalize(const std::vector<float>& vec) const;

    std::vector<Mapping> mappings;
public:
    void reset();
};

#include "global_knowledge.h"



//public methods
int GlobalKnowledge::add_knowledge(const std::string& phrase)
{
    auto iter = std::find_if(mappings.begin(), mappings.end(), [&phrase](const Mapping& mapping) { return mapping.phrase == phrase; });
    int index = std::distance(mappings.begin(), iter);
    if (iter != mappings.end())
    {
        return index;
    }
    const std::array<float, EMBEDDING_SIZE> embedding = Embedder::get_instance().get_embedding(phrase);
    mappings.push_back({ phrase, embedding });
    return mappings.size() - 1; //Get the item just added.
}

// Example method to retrieve a mapping by index
const std::string GlobalKnowledge::get_knowledge(size_t index) const {
    if (index >= mappings.size()) {
        return "";
    }
    return mappings[index].phrase;
}

const std::vector<std::string> GlobalKnowledge::get_closest_items(std::string phrase, int num_items) const
{
    const std::array<float, EMBEDDING_SIZE> phrase_embedding = Embedder::get_instance().get_embedding(phrase);
    std::vector<std::pair<float, std::string>> similarity_scores;

    for (const auto& mapping : mappings) {
        double similarity = cosine_similarity(phrase_embedding, mapping.embedding);
        similarity_scores.push_back({ similarity, mapping.phrase });
    }

    std::sort(similarity_scores.begin(), similarity_scores.end(), std::greater<>());  // Sort in descending order of similarity

    std::vector<std::string> closest_items;
    for (int i = 0; i < num_items && i < similarity_scores.size(); i++) {
        closest_items.push_back(similarity_scores[i].second);
    }

    return closest_items;
}

//private methods.
float GlobalKnowledge::cosine_similarity(const std::array<float, EMBEDDING_SIZE>& a, const std::array<float, EMBEDDING_SIZE>& b) const {
    float dot_product = 0.0;
    float norm_a = 0.0;
    float norm_b = 0.0;
    for (size_t i = 0; i < EMBEDDING_SIZE; ++i) {
        dot_product += a[i] * b[i];
        norm_a += a[i] * a[i];
        norm_b += b[i] * b[i];
    }
    return dot_product / (sqrt(norm_a) * sqrt(norm_b));
}

//protected methods
void GlobalKnowledge::reset()
{
    mappings.clear();
}
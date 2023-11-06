#pragma once

#include "classifier.h"
#include<string>
#include<vector>


class BertClassifier {
public:
    static BertClassifier& get_instance() {
        static BertClassifier instance; // Guaranteed to be lazy initialized and destroyed correctly
        return instance;
    }

    DialogueType get_classification(const std::string& dialogue);

private:
    BertClassifier();

    ~BertClassifier() = default;

    // Make sure they cannot be copied (this ensures singleton property)
    BertClassifier(BertClassifier const&) = delete;
    void operator=(BertClassifier const&) = delete;


    struct Embedding {
        DialogueType classification;
        std::string description;
        std::vector<float> embedding;
    };


    void add_classification(DialogueType classification, const std::string& description);
    float cosine_similarity(const std::vector<float>& a, const std::vector<float>& b) const;
    std::vector<float> normalize(const std::vector<float>& vec) const;

    std::vector<Embedding> classifications;

};

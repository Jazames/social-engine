#include "bert_classifier.h"
#include <algorithm>
#include <unordered_map>
#include "bert_embed.h"




DialogueType BertClassifier::get_classification(const std::string& dialogue)
{
    const std::vector<float> phrase_embedding = BertEmbedder::get_instance().get_embedding(dialogue);
    std::vector<std::pair<float, Embedding> > similarity_scores;

    for (const auto& item : classifications) {
        float similarity = cosine_similarity(phrase_embedding, item.embedding);
        similarity_scores.push_back({ similarity, item });
    }

    std::sort(similarity_scores.begin(), similarity_scores.end(), [](const std::pair<float, Embedding>& a, const std::pair<float, Embedding>& b) {
        return a.first > b.first; // Descending order
        });  // Sort in descending order of similarity

    float similarity = similarity_scores.front().first;

    if (similarity > 0.20f)
    {
        return similarity_scores.front().second.classification;
    }
    else
    {
        //Statement is kinda an "other" category, so rather than trying to match it semantically, we'll just return it if nothing else is close.
		return Statement;
	}
}

BertClassifier::BertClassifier()
{
    add_classification(Greeting, "Greeting");

    add_classification(Compliment, "Compliment");
    add_classification(Compliment, "virtuous");
    //add_classification(Compliment, "attractive");
    add_classification(Compliment, "wise");
    //add_classification(Compliment, "kind");
    add_classification(Compliment, "strong");

    add_classification(Insult, "dumb");
    add_classification(Insult, "ugly");
    add_classification(Insult, "contemptable");

    add_classification(Request, "Request, will you, can you");

    add_classification(Question, "question, rude questions");
    add_classification(Question, "where is");
    add_classification(Question, "who is");
    add_classification(Question, "what is");

    //add_classification(Statement, "dull statement");
    //add_classification(Statement, "facts about the world");
    //add_classification(Statement, "");
}

void BertClassifier::add_classification(DialogueType classification, const std::string& description)
{
	Embedding new_classification;
	new_classification.classification = classification;
	new_classification.description = description;
	new_classification.embedding = BertEmbedder::get_instance().get_embedding(description);
	classifications.push_back(new_classification);
}

float BertClassifier::cosine_similarity(const std::vector<float>& a, const std::vector<float>& b) const {
    float dot_product = 0.0;
    float norm_a = 0.0;
    float norm_b = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        dot_product += a[i] * b[i];
        norm_a += a[i] * a[i];
        norm_b += b[i] * b[i];
    }
    return dot_product / (sqrt(norm_a) * sqrt(norm_b));
}

std::vector<float> BertClassifier::normalize(const std::vector<float>& vec) const {
    // Compute the Euclidean norm (length) of the vector
    float norm = 0.0f;
    for (float value : vec) {
        norm += value * value;
    }
    norm = std::sqrt(norm);

    // Check for a zero vector (to avoid division by zero)
    if (norm == 0.0f) {
        //TODO: log error.
        return vec;
    }

    // Normalize each component of the vector
    std::vector<float> normalizedVec(vec.size());
    for (size_t i = 0; i < vec.size(); ++i) {
        normalizedVec[i] = vec[i] / norm;
    }

    return normalizedVec;
}


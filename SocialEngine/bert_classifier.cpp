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


    return similarity_scores.front().second.classification;
}

BertClassifier::BertClassifier()
{
    Embedding greeting;
    greeting.classification = DialogueType::Greeting;
    greeting.description = "Greeting";
    greeting.embedding = BertEmbedder::get_instance().get_embedding(greeting.description);
    classifications.push_back(greeting);

    Embedding compliment;
    compliment.classification = DialogueType::Compliment;
    compliment.description = "Compliment";
    compliment.embedding = BertEmbedder::get_instance().get_embedding(compliment.description);
    classifications.push_back(compliment);

    Embedding insult;
    insult.classification = DialogueType::Insult;
    insult.description = "Insult";
    insult.embedding = BertEmbedder::get_instance().get_embedding(insult.description);
    classifications.push_back(insult);

    Embedding request;
    request.classification = DialogueType::Request;
    request.description = "Request";
    request.embedding = BertEmbedder::get_instance().get_embedding(request.description);
    classifications.push_back(request);

    Embedding question;
    question.classification = DialogueType::Question;
    question.description = "Question";
    question.embedding = BertEmbedder::get_instance().get_embedding(question.description);
    classifications.push_back(question);

    Embedding statement;
    statement.classification = DialogueType::Statement;
    statement.description = "Statement";
    statement.embedding = BertEmbedder::get_instance().get_embedding(statement.description);
    classifications.push_back(statement);

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


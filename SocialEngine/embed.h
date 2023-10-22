#pragma once

#include<string>
#include<array>

#include "common.h"
#include "llama.h"

const int EMBEDDING_SIZE = 5120;

class Embedder {
public:
    static Embedder& get_instance() {
        static Embedder instance; // Guaranteed to be lazy initialized and destroyed correctly
        return instance;
    }

    std::array<float, EMBEDDING_SIZE> get_embedding(const std::string& prompt);

private:
    llama_model* model;
    llama_model_params model_params;
    llama_context_params ctx_params;
    gpt_params params;
    
    Embedder();

    ~Embedder();

    // Make sure they cannot be copied (this ensures singleton property)
    Embedder(Embedder const&) = delete;
    void operator=(Embedder const&) = delete;
};



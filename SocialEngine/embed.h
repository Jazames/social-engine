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

    std::vector<float> get_embedding(const std::string& prompt);

private:
    llama_model* model;
    llama_context* ctx;
    gpt_params params;

    std::vector<unsigned char> save_state;
    
    Embedder();

    ~Embedder();

    // Make sure they cannot be copied (this ensures singleton property)
    Embedder(Embedder const&) = delete;
    void operator=(Embedder const&) = delete;
};



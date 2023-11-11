#pragma once

#include<string>
#include<vector>


#include "bert.h"
//#include "ggml.h"


class BertEmbedder {
public:
    static BertEmbedder& get_instance() {
        static BertEmbedder instance; // Guaranteed to be lazy initialized and destroyed correctly
        return instance;
    }

    std::vector<float> get_embedding(const std::string& prompt);

	void deinit() {
		BertEmbedder::~BertEmbedder();
	}

private:
    bert_ctx * bctx;
    bert_params params;
    size_t embedding_max_size;

    
    BertEmbedder();

    ~BertEmbedder();

    // Make sure they cannot be copied (this ensures singleton property)
    BertEmbedder(BertEmbedder const&) = delete;
    void operator=(BertEmbedder const&) = delete;
};



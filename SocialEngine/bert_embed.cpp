#include "bert_embed.h"


std::vector<float> BertEmbedder::get_embedding(const std::string& prompt) 
{
    params.prompt = prompt.c_str();

    // tokenize the prompt
    std::vector<int32_t> tokens = llama_tokenize(ctx, prompt, true, true);

    const int n_embd = llama_n_embd(model);
    std::vector<float> embeddings(n_embd, 0);
    float* emb = embeddings.data();

    struct llama_batch batch = llama_batch_init(params.n_batch, 0, 1);
    size_t n_tokens = tokens.size();
    for (size_t i = 0; i < n_tokens; i++) {
        llama_batch_add(batch, tokens[i], i, { 0 }, true);
    }

    llama_kv_cache_clear(ctx);
    llama_decode(ctx, batch);

    const float* embd = llama_get_embeddings_seq(ctx, batch.seq_id[0][0]);

    for (size_t i = 0; i < n_embd; i++) {
		emb[i] = embd[i];
	}

    llama_batch_free(batch);

    return embeddings;
}

BertEmbedder::BertEmbedder() {
    // Initialization logic    
    params.embedding = true;
    // For non-causal models, batch size must be equal to ubatch size
    params.n_ubatch = params.n_batch;
    params.model = "C:\\Users\\James\\source\\projects\\models\\classifier.ggml";
    //params.seed = time(NULL);;

    // init LLM    
    llama_backend_init();
    llama_numa_init(params.numa);

    llama_init_result init_result = llama_init_from_gpt_params(params);
    model = init_result.model;
    ctx = init_result.context;
    if (model == NULL) {
        fprintf(stderr, "%s: error: unable to load model\n", __func__);
        exit(1);
    }
    if (ctx == NULL) {
        fprintf(stderr, "%s: error: unable to load context\n", __func__);
        exit(1);
    }
}

BertEmbedder::~BertEmbedder() {
    // Deinitialization logic
    llama_free(ctx);
    llama_free_model(model);
    llama_backend_free();
}

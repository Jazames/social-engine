#include "embed.h"

#include "util.h"
#include <algorithm>



std::array<float, EMBEDDING_SIZE> Embedder::get_embedding(const std::string& prompt) 
{
    std::array<float, EMBEDDING_SIZE> embedding = std::array<float, EMBEDDING_SIZE>();
    llama_kv_cache_tokens_rm(ctx, -1, -1);
    llama_reset_timings(ctx);

    const int n_ctx_train = llama_n_ctx_train(model);
    const int n_ctx = llama_n_ctx(ctx);

    if (n_ctx > n_ctx_train) {
        fprintf(stderr, "%s: warning: model was trained on only %d context tokens (%d specified)\n",
            __func__, n_ctx_train, n_ctx);
    }

    int n_past = 0;

    // tokenize the prompt
    auto embd_inp = ::llama_tokenize(ctx, params.prompt, true);

    if (embd_inp.size() > (size_t)n_ctx) {
        fprintf(stderr, "%s: error: prompt is longer than the context window (%zu tokens, n_ctx = %d)\n",
            __func__, embd_inp.size(), n_ctx);
        return embedding;
    }

    while (!embd_inp.empty()) {
        int n_tokens = std::min(params.n_batch, (int)embd_inp.size());
        if (llama_decode(ctx, llama_batch_get_one(embd_inp.data(), n_tokens, n_past, 0))) {
            fprintf(stderr, "%s : failed to eval\n", __func__);
            return embedding;
        }
        n_past += n_tokens;
        embd_inp.erase(embd_inp.begin(), embd_inp.begin() + n_tokens);
    }

    const int n_embd = llama_n_embd(model);
    float * values = llama_get_embeddings(ctx);
    for (int i = 0; i < n_embd && i < EMBEDDING_SIZE; i++)
    {
        embedding[i] = values[i];
    }
    return embedding;
}

Embedder::Embedder() {
    // Initialization logic
    params.sparams.temp = 0.0f;
    params.embedding = true;
    params.model = "C:\\Users\\James\\source\\repos\\llama.cpp\\models\\llama-2-13b-chat\\ggml-model-q4_0.gguf";
    params.seed = time(NULL);;

    // init LLM
    llama_backend_init(params.numa);


    std::tie(model, ctx) = llama_init_from_gpt_params(params);
    if (model == NULL) {
        fprintf(stderr, "%s: error: unable to load model\n", __func__);
        exit(1);
    }
    if (ctx == NULL) {
        fprintf(stderr, "%s: error: unable to load model\n", __func__);
        exit(1);
    }
}

Embedder::~Embedder() {
    // Deinitialization logic
    llama_free(ctx);
    llama_free_model(model);
    llama_backend_free();
}

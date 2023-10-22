#include "embed.h"

#include "util.h"
#include <algorithm>



std::array<float, EMBEDDING_SIZE> Embedder::get_embedding(const std::string& prompt)
{
    std::array<float, EMBEDDING_SIZE> embedding;
    DeallocatingWrapper<llama_context, llama_free, decltype(llama_new_context_with_model), llama_model*, llama_context_params> context(llama_new_context_with_model, model, ctx_params);
    llama_context* ctx = context.get();

    std::vector<llama_token> tokens_list;
    tokens_list = llama_tokenize(ctx, prompt, true);

    const int max_context_size = llama_n_ctx(ctx);
    const int max_tokens_list_size = max_context_size - 4;

    if ((int)tokens_list.size() > max_tokens_list_size) {
        fprintf(stderr, "%s: error: prompt too long (%d tokens, max %d)\n", __func__, (int)tokens_list.size(), max_tokens_list_size);
        return embedding;
    }

    const int n_ctx_train = llama_n_ctx_train(model);
    if (params.n_ctx > n_ctx_train) {
        fprintf(stderr, "%s: warning: model was trained on only %d context tokens (%d specified)\n",
            __func__, n_ctx_train, params.n_ctx);
    }

    int n_past = 0;

    while (!tokens_list.empty()) {
        int n_tokens = std::min(params.n_batch, (int)tokens_list.size());
        if (llama_decode(ctx, llama_batch_get_one(tokens_list.data(), n_tokens, n_past, 0))) {
            fprintf(stderr, "%s : failed to eval\n", __func__);
            return embedding;
        }
        n_past += n_tokens;
        tokens_list.erase(tokens_list.begin(), tokens_list.begin() + n_tokens);
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
    params.model = "C:\\Users\\James\\source\\repos\\llama.cpp\\models\\llama-2-13b-chat\\ggml-model-q4_0.gguf";
    params.n_threads = 4;
    params.seed = 2358;
    params.n_ctx = 2048;

    // init LLM
    llama_backend_init(params.numa);
    model_params = llama_model_params_from_gpt_params(params);
    ctx_params = llama_context_params_from_gpt_params(params);
    model = llama_load_model_from_file(params.model.c_str(), model_params);
    if (model == NULL) {
        fprintf(stderr, "%s: error: unable to load model\n", __func__);
        exit(1);
    }
}

Embedder::~Embedder() {
    // Deinitialization logic
    llama_free_model(model);
    llama_backend_free();
}

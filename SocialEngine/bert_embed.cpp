#include "bert_embed.h"


std::vector<float> BertEmbedder::get_embedding(const std::string& prompt) 
{
    params.prompt = prompt.c_str();
    //std::vector<float> embedding = std::vector<float>(1);//(EMBEDDING_SIZE);
    
    //ggml_time_init();
    //const int64_t t_main_start_us = ggml_time_us();

    //bert_params params;
    //params.model = "../../models/all-MiniLM-L6-v2/ggml-model-f32.bin";

    //int64_t t_load_us = 0;


    //int64_t t_eval_us  = 0;
    //int64_t t_start_us = ggml_time_us();
    int N = bert_n_max_tokens(bctx);

    // tokenize the prompt
    std::vector<bert_vocab_id> tokens(N);
    int n_tokens;
    bert_tokenize(bctx, params.prompt, tokens.data(), &n_tokens, N);
    tokens.resize(n_tokens);

    std::vector<float> embeddings(bert_n_embd(bctx));
    bert_eval(bctx, params.n_threads, tokens.data(), n_tokens, embeddings.data());
    //t_eval_us += ggml_time_us() - t_start_us;
    


    /*
    float * values = llama_get_embeddings(ctx);
    for (int i = 0; i < n_embd && i < EMBEDDING_SIZE; i++)
    {
        embedding[i] = values[i];
    }
    */
    return embeddings;
}

BertEmbedder::BertEmbedder() {
    // Initialization logic
    //params.embedding = true;
    params.model = "C:\\Users\\James\\source\\repos\\SocialEngine\\bertCpp\\models\\all-MiniLM-L12-v2\\ggml-model-f16.bin";
    //params.seed = time(NULL);;

    // init LLM
    //llama_backend_init(params.numa);
    bctx = bert_load_from_file(params.model);
    if (bctx == NULL) {
        fprintf(stderr, "%s: error: unable to load model\n", __func__);
        exit(1);
    }

    embedding_max_size = bert_n_max_tokens(bctx);
    //Save internal state to get fresh embeddings.
    //size_t state_size = llama_get_state_size(ctx);
    //save_state = std::vector<unsigned char>(state_size);
    //llama_copy_state_data(ctx, save_state.data());
}

BertEmbedder::~BertEmbedder() {
    // Deinitialization logic
    bert_free(bctx);
}

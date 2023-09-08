#include "classifier.h"
#include "util.h"

#include <cmath>
#include <cstdio>
#include <vector>


DialogueType Classifier::get_classification(const std::string& dialogue)
{
    //TODO: Might want to only deallocate and reallocate the context as needed, rather than for each call.
    DeallocatingWrapper<llama_context, llama_free, decltype(llama_new_context_with_model), llama_model*, llama_context_params> context(llama_new_context_with_model, model, ctx_params);
    llama_context* ctx = context.get();

    std::vector<llama_token> tokens_list;
    tokens_list = llama_tokenize(ctx, prompt_instructions + dialogue + ".\n", true);

    const int max_context_size = llama_n_ctx(ctx);
    const int max_tokens_list_size = max_context_size - 4;

    if ((int)tokens_list.size() > max_tokens_list_size) {
        //fprintf(stderr, "%s: error: prompt too long (%d tokens, max %d)\n", __func__, (int)tokens_list.size(), max_tokens_list_size);
        return IncoherentRambling;
    }

    //fprintf(stderr, "\n\n");

    for (auto id : tokens_list) {
        //fprintf(stderr, "%s", llama_token_to_piece(ctx, id).c_str());
    }

    //fflush(stderr);

    // main loop

    // The LLM keeps a contextual cache memory of previous token evaluation.
    // Usually, once this cache is full, it is required to recompute a compressed context based on previous
    // tokens (see "infinite text generation via context swapping" in the main example), but in this minimalist
    // example, we will just stop the loop once this cache is full or once an end of stream is detected.

    const int n_gen = std::min(512, max_context_size);
    std::string response;

    while (llama_get_kv_cache_token_count(ctx) < n_gen) {
        // evaluate the transformer

        if (llama_eval(ctx, tokens_list.data(), int(tokens_list.size()), llama_get_kv_cache_token_count(ctx), params.n_threads))
        {
            return IncoherentRambling;
        }

        tokens_list.clear();

        // sample the next token

        llama_token new_token_id = 0;

        auto logits = llama_get_logits(ctx);
        auto n_vocab = llama_n_vocab(ctx);

        std::vector<llama_token_data> candidates;
        candidates.reserve(n_vocab);

        for (llama_token token_id = 0; token_id < n_vocab; token_id++)
        {
            candidates.emplace_back(llama_token_data{ token_id, logits[token_id], 0.0f });
        }

        llama_token_data_array candidates_p = { candidates.data(), candidates.size(), false };

        new_token_id = llama_sample_token_greedy(ctx, &candidates_p);

        response += llama_token_to_piece(ctx, new_token_id);

        // Check for the specific words in response
        if (response.find("Greeting") != std::string::npos) {
            return Greeting;
        }
        else if (response.find("Insult") != std::string::npos) {
            return Insult;
        }
        else if (response.find("Question") != std::string::npos) {
            return Question;
        }
        else if (response.find("Statement") != std::string::npos) {
            return Statement;
        }
        else if (response.find("Request") != std::string::npos) {
            return Request;
        }

        // is it an end of stream ?
        if (new_token_id == llama_token_eos(ctx)) {
            return IncoherentRambling;
        }

        // push this new token for next evaluation
        tokens_list.push_back(new_token_id);
    }

    // ... (use the provided code for processing the dialogue but replace initialization and deinitialization parts)
    return IncoherentRambling; // This should be replaced with your actual logic
}



Classifier::Classifier() {
    // Initialization logic
    params.model = "C:\\Users\\James\\source\\repos\\llama.cpp\\models\\llama-2-13b-chat\\ggml-model-q4_0.gguf";

    // init LLM
    llama_backend_init(params.numa);
    ctx_params = llama_context_default_params();
    model = llama_load_model_from_file(params.model.c_str(), ctx_params);
    if (model == NULL) {
        fprintf(stderr, "%s: error: unable to load model\n", __func__);
        exit(1);
    }
}

Classifier::~Classifier() {
    // Deinitialization logic
    llama_free_model(model);
    llama_backend_free();
}





/*
DialogueType get_classification(std::string dialogue)
{
    
    gpt_params params;


    params.model = "path to model";
    

    params.prompt = "hey hey you you";
    
    params.temp = 0.4;


    // init LLM

    llama_backend_init(params.numa);

    llama_context_params ctx_params = llama_context_default_params();

    llama_model* model = llama_load_model_from_file(params.model.c_str(), ctx_params);

    if (model == NULL) {
        fprintf(stderr, "%s: error: unable to load model\n", __func__);
        return Incoher;
    }

    llama_context* ctx = llama_new_context_with_model(model, ctx_params);

    // tokenize the prompt

    std::vector<llama_token> tokens_list;
    tokens_list = ::llama_tokenize(ctx, params.prompt, true);

    const int max_context_size = llama_n_ctx(ctx);
    const int max_tokens_list_size = max_context_size - 4;

    if ((int)tokens_list.size() > max_tokens_list_size) {
        fprintf(stderr, "%s: error: prompt too long (%d tokens, max %d)\n", __func__, (int)tokens_list.size(), max_tokens_list_size);
        //Todo: fix.
        return Greeting;
    }

    fprintf(stderr, "\n\n");

    for (auto id : tokens_list) {
        fprintf(stderr, "%s", llama_token_to_piece(ctx, id).c_str());
    }

    fflush(stderr);

    // main loop

    // The LLM keeps a contextual cache memory of previous token evaluation.
    // Usually, once this cache is full, it is required to recompute a compressed context based on previous
    // tokens (see "infinite text generation via context swapping" in the main example), but in this minimalist
    // example, we will just stop the loop once this cache is full or once an end of stream is detected.

    const int n_gen = std::min(32, max_context_size);

    while (llama_get_kv_cache_token_count(ctx) < n_gen) {
        // evaluate the transformer

        if (llama_eval(ctx, tokens_list.data(), int(tokens_list.size()), llama_get_kv_cache_token_count(ctx), params.n_threads)) {
            fprintf(stderr, "%s : failed to eval\n", __func__);
            return ;
        }

        tokens_list.clear();

        // sample the next token

        llama_token new_token_id = 0;

        auto logits = llama_get_logits(ctx);
        auto n_vocab = llama_n_vocab(ctx);

        std::vector<llama_token_data> candidates;
        candidates.reserve(n_vocab);

        for (llama_token token_id = 0; token_id < n_vocab; token_id++) {
            candidates.emplace_back(llama_token_data{ token_id, logits[token_id], 0.0f });
        }

        llama_token_data_array candidates_p = { candidates.data(), candidates.size(), false };

        new_token_id = llama_sample_token_greedy(ctx, &candidates_p);

        // is it an end of stream ?
        if (new_token_id == llama_token_eos(ctx)) {
            fprintf(stderr, " [end of text]\n");
            break;
        }

        // print the new token :
        printf("%s", llama_token_to_piece(ctx, new_token_id).c_str());
        fflush(stdout);

        // push this new token for next evaluation
        tokens_list.push_back(new_token_id);
    }

    llama_free(ctx);
    llama_free_model(model);

    llama_backend_free();
    return DialogueType::Greeting;
}
    */
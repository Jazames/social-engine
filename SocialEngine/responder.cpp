#include "responder.h"
#include "util.h"

#include <cmath>
#include <cstdio>
#include <vector>
#include <sstream>




std::string Responder::get_response(const std::string& dialogue, Age age, Enthusiasm enthusiasm)
{
    //TODO: Might want to only deallocate and reallocate the context as needed, rather than for each call.
    DeallocatingWrapper<llama_context, llama_free, decltype(llama_new_context_with_model), llama_model*, llama_context_params> context(llama_new_context_with_model, model, ctx_params);
    llama_context* ctx = context.get();

    std::vector<llama_token> tokens_list;
    std::string prompt = build_prompt(dialogue, age, enthusiasm);
    tokens_list = llama_tokenize(ctx, prompt, true);

    const int max_context_size = llama_n_ctx(ctx);
    const int max_tokens_list_size = max_context_size - 4;

    if ((int)tokens_list.size() > max_tokens_list_size) {
        //fprintf(stderr, "%s: error: prompt too long (%d tokens, max %d)\n", __func__, (int)tokens_list.size(), max_tokens_list_size);
        return "...";
    }

    //fprintf(stderr, "\n\n");

    //for (auto id : tokens_list) {
    //    fprintf(stderr, "%s", llama_token_to_piece(ctx, id).c_str());
    //}

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
            return "...";
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
        //std::cout << response;

        // Check for the specific words in response

        // is it an end of stream ?
        if (new_token_id == llama_token_eos(ctx)) {
            return response;
        }

        // push this new token for next evaluation
        tokens_list.push_back(new_token_id);
    }

    // ... (use the provided code for processing the dialogue but replace initialization and deinitialization parts)
    return "..."; // This should be replaced with your actual logic
}

void Responder::do_test()
{
    std::array<std::string, 3> dialogues = { "Hello.", "How's it going?", "Good morning." };
    std::array<Age, 5> ages = { Old, Middle, Young, Teen, Child };
    std::array<Enthusiasm, 4> emotions = { Enthusiastic, Mild, Reluctant, Angry };

    for (auto d : dialogues)
    {
        for (auto a : ages)
        {
            for (auto e : emotions)
            {
                std::cout << "Prompt: " << build_prompt(d, a, e) << std::endl;
                std::cout << "Response: " << get_response(d, a, e) << std::endl << std::endl;
            }
        }
    }
}

std::string Responder::build_prompt(const std::string& dialogue, Age age, Enthusiasm enthusiasm)
{
    std::ostringstream string_builder;
    string_builder << prompt_instructions
        << disposition_instructions << emotion[enthusiasm]
        << age_instructions << generation[age]
        << end_prompt << dialogue
        << end_chat;

    std::string result = string_builder.str();
    return result;
}


Responder::Responder() {
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

Responder::~Responder() {
    // Deinitialization logic
    llama_free_model(model);
    llama_backend_free();
}

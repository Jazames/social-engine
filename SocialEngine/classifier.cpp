#include "classifier.h"
#include "util.h"

#include <cmath>
#include <cstdio>
#include <vector>


DialogueType Classifier::get_classification(const std::string& dialogue)
{
    //TODO: Might want to only deallocate and reallocate the context as needed, rather than for each call.
    //DeallocatingWrapper<llama_context, llama_free, decltype(llama_new_context_with_model), llama_model*, llama_context_params> context(llama_new_context_with_model, model, ctx_params);
    //llama_context* ctx = context.get();
    llama_kv_cache_tokens_rm(ctx, -1, -1);
    llama_reset_timings(ctx);

    llama_sampling_params& sparams = params.sparams;

    params.prompt = prompt_instructions + dialogue + ".\n";

    const int n_ctx_train = llama_n_ctx_train(model);
    const int n_ctx = llama_n_ctx(ctx);

    const bool add_bos = llama_vocab_type(model) == LLAMA_VOCAB_TYPE_SPM;

    std::vector<llama_token> embd_inp;
    embd_inp = ::llama_tokenize(ctx, params.prompt, add_bos, true);



    // number of tokens to keep when resetting context
    if (params.n_keep < 0 || params.n_keep >(int) embd_inp.size() || params.instruct) {
        params.n_keep = (int)embd_inp.size();
    }

    bool input_echo = true;
     
    int n_past = 0;
    int n_remain = params.n_predict;
    int n_consumed = 0;
    int n_session_consumed = 0;
    int n_past_guidance = 0;


    //std::vector<llama_token> embd;

    struct llama_sampling_context* ctx_sampling = llama_sampling_init(sparams);
    std::vector<llama_token> embd;


    std::string prompty = "";
    for (auto i : embd_inp)
    {
        prompty += llama_token_to_piece(ctx, i);
    }

    std::string response;
    int max_generated_tokens = 16;
    int generated_tokens = 0;


    while (generated_tokens < max_generated_tokens)
    {
        // predict
        if (!embd.empty()) {
            // Note: n_ctx - 4 here is to match the logic for commandline prompt handling via
            // --prompt or --file which uses the same value.
            int max_embd_size = n_ctx - 4;

            // Ensure the input doesn't exceed the context size by truncating embd_inp if necessary.
            if ((int)embd.size() > max_embd_size) {
                const int skipped_tokens = (int)embd.size() - max_embd_size;
                embd.resize(max_embd_size);

                printf("<<input too long: skipped %d token%s>>", skipped_tokens, skipped_tokens != 1 ? "s" : "");
                fflush(stdout);
            }

            // infinite text generation via context swapping
            // if we run out of context:
            // - take the n_keep first tokens from the original prompt (via n_past)
            // - take half of the last (n_ctx - n_keep) tokens and recompute the logits in batches
            if (n_past + (int)embd.size() > n_ctx) {
                if (params.n_predict == -2) {
                    LOG_TEE("\n\n%s: context full and n_predict == -%d => stopping\n", __func__, params.n_predict);
                    break;
                }

                const int n_left = n_past - params.n_keep - 1;
                const int n_discard = n_left / 2;

                LOG("context full, swapping: n_past = %d, n_left = %d, n_ctx = %d, n_keep = %d, n_discard = %d\n",
                    n_past, n_left, n_ctx, params.n_keep, n_discard);

                llama_kv_cache_seq_rm(ctx, 0, params.n_keep + 1, params.n_keep + n_discard + 1);
                llama_kv_cache_seq_shift(ctx, 0, params.n_keep + 1 + n_discard, n_past, -n_discard);

                n_past -= n_discard;

                LOG("after swap: n_past = %d, n_past_guidance = %d\n", n_past, n_past_guidance);

                LOG("embd: %s\n", LOG_TOKENS_TOSTR_PRETTY(ctx, embd).c_str());

            }

            for (int i = 0; i < (int)embd.size(); i += params.n_batch) {
                int n_eval = (int)embd.size() - i;
                if (n_eval > params.n_batch) {
                    n_eval = params.n_batch;
                }

                LOG("eval: %s\n", LOG_TOKENS_TOSTR_PRETTY(ctx, embd).c_str());

                if (llama_decode(ctx, llama_batch_get_one(&embd[i], n_eval, n_past, 0))) {
                    LOG_TEE("%s : failed to eval\n", __func__);
                    return IncoherentRambling;
                }

                n_past += n_eval;

                LOG("n_past = %d\n", n_past);
            }
        }

        embd.clear();




        if ((int)embd_inp.size() <= n_consumed) {

            const llama_token id = llama_sampling_sample(ctx_sampling, ctx, NULL);

            llama_sampling_accept(ctx_sampling, ctx, id, true);

            LOG("last: %s\n", LOG_TOKENS_TOSTR_PRETTY(ctx, ctx_sampling->prev).c_str());

            embd.push_back(id);
            const std::string token_str = llama_token_to_piece(ctx, id);
            response += token_str;

            // echo this to console
            input_echo = true;

            // decrement remaining sampling budget
            --n_remain;

            LOG("n_remain: %d\n", n_remain);
        }
        else {
            // some user input remains from prompt or interaction, forward it to processing
            LOG("embd_inp.size(): %d, n_consumed: %d\n", (int)embd_inp.size(), n_consumed);
            while ((int)embd_inp.size() > n_consumed) {
                embd.push_back(embd_inp[n_consumed]);

                // push the prompt in the sampling context in order to apply repetition penalties later
                // for the prompt, we don't apply grammar rules
                llama_sampling_accept(ctx_sampling, ctx, embd_inp[n_consumed], false);

                ++n_consumed;
                if ((int)embd.size() >= params.n_batch) {
                    break;
                }
            }
        }








        //const llama_token id = llama_sampling_sample(ctx_sampling, ctx, NULL);
        //embd.push_back(id);
        //llama_sampling_accept(ctx_sampling, ctx, id, true);
        //const std::string token_str = llama_token_to_piece(ctx, id);
        //response += token_str;

        // Check for the specific words in response
        if (response.find("Greeting") != std::string::npos)
        {
            return Greeting;
        }
        else if (response.find("Compliment") != std::string::npos)
        {
            return Compliment;
        }
        else if (response.find("Insult") != std::string::npos)
        {
            return InsultNoun;
        }
        else if (response.find("Question") != std::string::npos)
        {
            return Question;
        }
        else if (response.find("Statement") != std::string::npos)
        {
            return Statement;
        }
        else if (response.find("Request") != std::string::npos)
        {
            return Request;
        }

        // end of text token
        if (!embd.empty() && embd.back() == llama_token_eos(ctx)) {
            LOG_TEE(" [end of text]\n");
            break;
        }
    }

    // ... (use the provided code for processing the dialogue but replace initialization and deinitialization parts)
    return IncoherentRambling; // This should be replaced with your actual logic
}

Classifier::Classifier() {
    // Initialization logic
    //params.use_mmap = true;
    params.sparams.temp = 0.0f;
    params.model = "C:\\Users\\James\\source\\repos\\llama.cpp\\models\\llama-2-13b-chat\\ggml-model-q4_0.gguf";
    params.seed = time(NULL);;
    //params.n_ctx = 2048;

    // init LLM
    llama_backend_init(params.numa);


    std::tie(model, ctx) = llama_init_from_gpt_params(params);
    
    /*
    model_params = llama_model_params_from_gpt_params(params);
    model = llama_load_model_from_file(params.model.c_str(), model_params);
    ctx_params = llama_context_params_from_gpt_params(params);
    if (model == NULL) {
        fprintf(stderr, "%s: error: unable to load model\n", __func__);
        exit(1);
    }
    ctx = llama_new_context_with_model(model, ctx_params);
    if (ctx == NULL) {
        fprintf(stderr, "%s: error: unable to load model\n", __func__);
        exit(1);
    }

    std::vector<llama_token> tmp = { llama_token_bos(ctx), llama_token_eos(ctx), };
    llama_decode(ctx, llama_batch_get_one(tmp.data(), std::min(tmp.size(), (size_t)params.n_batch), 0, 0));
    llama_kv_cache_tokens_rm(ctx, -1, -1);
    llama_reset_timings(ctx);
    //*/
}

Classifier::~Classifier() {
    // Deinitialization logic
    llama_free(ctx);
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
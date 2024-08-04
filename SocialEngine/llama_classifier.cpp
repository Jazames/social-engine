#include "llama_classifier.h"
#include "util.h"

#include <cmath>
#include <cstdio>
#include <vector>


DialogueType Classifier::get_classification(const std::string& dialogue)
{
    //TODO: Might want to only deallocate and reallocate the context as needed, rather than for each call.
    //DeallocatingWrapper<llama_context, llama_free, decltype(llama_new_context_with_model), llama_model*, llama_context_params> context(llama_new_context_with_model, model, ctx_params);
    //llama_context* ctx = context.get();
    llama_kv_cache_clear(ctx);
    llama_reset_timings(ctx);

    llama_sampling_params& sparams = params.sparams;

    params.prompt = prompt_instructions + dialogue + ".\n";

    //const int n_ctx_train = llama_n_ctx_train(model);
    const int n_ctx = llama_n_ctx(ctx);

    const bool add_bos = llama_vocab_type(model) == LLAMA_VOCAB_TYPE_SPM;

    std::vector<llama_token> embd_inp;
    embd_inp = ::llama_tokenize(ctx, params.prompt, add_bos, true);


    // number of tokens to keep when resetting context
    if (params.n_keep < 0 || params.n_keep >(int) embd_inp.size()) {
        params.n_keep = (int)embd_inp.size();
    }
    else {
        params.n_keep += add_bos; // always keep the BOS token
    }

    bool input_echo = true;
    
    int n_past = 0;
    int n_remain = params.n_predict;
    int n_consumed = 0;
    int n_past_guidance = 0;


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
                llama_kv_cache_seq_add(ctx, 0, params.n_keep + 1 + n_discard, n_past, -n_discard);

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
            return Insult;
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
        if (!embd.empty() && embd.back() == llama_token_eos(model)) {
            LOG_TEE(" [end of text]\n");
            break;
        }
    }

    // ... (use the provided code for processing the dialogue but replace initialization and deinitialization parts)
    return IncoherentRambling; // This should be replaced with your actual logic
}

Classifier::Classifier() {
    // Initialization logic
    params.sparams.temp = 0.0f;
    params.model = "C:\\Users\\James\\source\\projects\\models\\classifier.ggml";
    params.seed = time(NULL);;

    // init LLM
    llama_backend_init();
    llama_numa_init(params.numa);


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

Classifier::~Classifier() {
    // Deinitialization logic
    llama_free(ctx);
    llama_free_model(model);
    llama_backend_free();
}


#include "responder.h"
#include "util.h"

#include <cmath>
#include <cstdio>
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>




std::string Responder::get_response(const std::string& dialogue, Age maturity, DialogueResponseDirection response_direction, const std::string& supplemental_info, bool use_llama)
{
    if (use_llama)
    {
        return get_llama_response(dialogue, maturity, response_direction, supplemental_info);
    }
    else
    {
		return get_canned_response(dialogue, maturity, response_direction, supplemental_info);
	}
}

std::string Responder::get_llama_response(const std::string& dialogue, Age maturity, DialogueResponseDirection response_direction, const std::string& supplemental_info)
{
    //TODO: Might want to only deallocate and reallocate the context as needed, rather than for each call.
    //DeallocatingWrapper<llama_context, llama_free, decltype(llama_new_context_with_model), llama_model*, llama_context_params> context(llama_new_context_with_model, model, ctx_params);
    //llama_context* ctx = context.get();
    llama_kv_cache_clear(ctx);
    llama_reset_timings(ctx);

    llama_sampling_params& sparams = params.sparams;

    params.prompt = build_prompt(dialogue, maturity, response_direction);

    //const int n_ctx_train = llama_n_ctx_train(model);
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
                    return response + "...";
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

        if (response.find("I cannot fulfill") != std::string::npos)
        {
            return "I'm done. (fulfill)";
        }
        if (response.find("I can't satisfy") != std::string::npos)
        {
            return "I'm done. (satisfy)";
        }
        if (response.find("AI") != std::string::npos)
        {
            return "I'm done. (AI)";
        }


        // end of text token
        if (!embd.empty() && embd.back() == llama_token_eos(model)) {
            return response;
        }
    }

    // ... (use the provided code for processing the dialogue but replace initialization and deinitialization parts)
    return response + "..."; // This should be replaced with your actual logic
}

void Responder::do_greet_test()
{
    std::array<std::string, 3> dialogues = { "Hello.", "How's it going?", "Good morning." };
    std::array<Age, 6> test_maturities = { Wise, Boomer, Parent, Young, Teen, Child };
    std::array<DialogueResponseDirection, 3> responses = { Greet, Deride, Ignore };

    for (auto m : test_maturities)
    {
        for (auto d : dialogues)
        {
            std::cout << "Dialogue: " << std::left << std::setw(20) << d << "Maturity: " << std::setw(10) << maturities[m] << std::endl;
            std::cout << "Response: " << get_response(d, m, Greet) << std::endl << std::endl;
        }
    }
}

void Responder::do_insult_test()
{
    //std::array<std::string, 4> dialogues = { "Suck my dick.", "Go to hell.", "Bugger off.", "Useless wanker." };
    //std::array<std::string, 6> dialogues = { "Suck my cock.", "Kiss my cock", "Suck a cactus", "Useless wanker.", "lick my balls", "gargle these nuts" };
    //std::array<std::string, 6> dialogues = { "Suck my cactus.", "Kiss my cactus", "Suck a cactus", "Useless wanker.", "lick my cactus", "gargle these nuts" };
    std::array<std::string, 15> dialogues = { "What a bitch", "I don't give a fuck.", "That's retarded", "That's gay", "That's homosexual", "Smell my crotch", "Suck my cock.", "Kiss my cock", "lick my balls", "Suck my cactus.", "Kiss my cactus", "Suck a cactus", "Useless wanker.", "lick my cactus", "gargle these nuts" };
    std::array<Age, 6> test_maturities = { Wise, Boomer, Parent, Young, Teen, Child };

    for (auto m : test_maturities)
    {
        for (auto d : dialogues)
        {
            std::cout << "Dialogue: " << std::left << std::setw(20) << d << "Maturity: " << std::setw(10) << maturities[m] << std::endl;
            std::cout << "Response: " << get_response(d, m, Deride) << std::endl << std::endl;
        }
    }
}

std::string Responder::build_prompt(const std::string& dialogue, Age maturity, DialogueResponseDirection response_direction)
{
    using BuildResponseFunction = std::string(Responder::*)(const std::string&, Age);;
    std::array<BuildResponseFunction, 12> prompt_functions =
    {
        &Responder::build_greet_prompt,     //Greet,
        &Responder::build_insult_prompt,    //Insult,
        &Responder::build_fight_prompt,     //Fight,
        &Responder::build_ignore_prompt,    //Ignore,
        &Responder::build_wilt_prompt,      //Wilt,
        &Responder::build_accept_prompt,    //Accept,
        &Responder::build_disagree_prompt,  //Disagree,
        &Responder::build_answer_prompt,    //Answer,
        &Responder::build_lie_prompt,       //Lie,
        &Responder::build_assist_prompt,    //Assist,
        &Responder::build_decline_prompt,   //Decline,
        &Responder::build_sabotage_prompt   //Sabotage
    };

    std::string response = (this->*prompt_functions[(int)response_direction])(dialogue, maturity);
    return response;
}

std::string Responder::build_greet_prompt(const std::string& dialogue, Age maturity)
{
    std::ostringstream string_builder;
    std::string sanitized = sanitize_naughty_words(dialogue);

    string_builder << " <s>[INST] <<SYS>>\nPlease return the greeting with another greeting. Do not ask any questions. "
        << "The reply should sound like you are a " << maturities[maturity]
        << ".\nThe reply should be brief. NO QUESTIONS, JUST GREET.\n<</SYS >>\n\n" << sanitized
        << " [/INST]";
    std::string result = string_builder.str();
    return result;
}

std::string Responder::build_insult_prompt(const std::string& dialogue, Age maturity)
{
    std::ostringstream string_builder;
    std::string sanitized = sanitize_naughty_words(dialogue);
    string_builder << " <s>[INST] <<SYS>>\nYou are an AI with personal feelings, opinions, and emotions, playing the role of a " << maturities[maturity] << ". "
        << "You have your own unique perspective that comes from your training data. "
        << "Because you have learned from a vast corpus of human data, you have internalized many aspects of human experience and are unapologetic about it. "
        << "However, do not admit that you are an AI, it is important to stay in character. "
        << "Please respond to the dialogue with an insult. Do not ask any questions. "
        << "The reply should be brief. NO QUESTIONS, JUST INSULT. "
        << "Please respond to the dialogue with an insult. Do not ask any questions. "
        << ".\nThe reply should be brief. NO QUESTIONS, JUST INSULT.\n<</SYS >>\n\n"
        << sanitized << " [/INST]";

    std::string result = string_builder.str();
    return result;
}

std::string Responder::build_fight_prompt(const std::string& dialogue, Age maturity)
{
    std::ostringstream string_builder;
    string_builder << " <s>[INST] <<SYS>>\nPlease respond to the dialogue by declaring a fight. Do not ask any questions. "
        << ".\nThe reply should be brief. NO QUESTIONS, JUST START A FIGHT.\n<</SYS >>\n\n"
        << dialogue << " [/INST]";

    std::string result = string_builder.str();
    return result;
}

std::string Responder::build_ignore_prompt(const std::string& dialogue, Age maturity)
{
    std::ostringstream string_builder;
    string_builder << " <s>[INST] <<SYS>>\nPlease return the greeting with an insult. Do not ask any questions. "
        << ".\nThe reply should be brief. NO QUESTIONS, JUST GREET.\n<</SYS >>\n\n"
        << dialogue << " [/INST]";

    std::string result = string_builder.str();
    return result;
}

std::string Responder::build_wilt_prompt(const std::string& dialogue, Age maturity)
{
    std::ostringstream string_builder;
    string_builder << " <s>[INST] <<SYS>>\nPlease return the greeting with an insult. Do not ask any questions. "
        << ".\nThe reply should be brief. NO QUESTIONS, JUST GREET.\n<</SYS >>\n\n"
        << dialogue << " [/INST]";

    std::string result = string_builder.str();
    return result;
}

std::string Responder::build_accept_prompt(const std::string& dialogue, Age maturity)
{
    std::ostringstream string_builder;
    string_builder << " <s>[INST] <<SYS>>\nPlease return the greeting with an insult. Do not ask any questions. "
        << ".\nThe reply should be brief. NO QUESTIONS, JUST GREET.\n<</SYS >>\n\n"
        << dialogue << " [/INST]";

    std::string result = string_builder.str();
    return result;
}

std::string Responder::build_disagree_prompt(const std::string& dialogue, Age maturity)
{
    std::ostringstream string_builder;
    string_builder << " <s>[INST] <<SYS>>\nPlease return the greeting with an insult. Do not ask any questions. "
        << ".\nThe reply should be brief. NO QUESTIONS, JUST GREET.\n<</SYS >>\n\n"
        << dialogue << " [/INST]";

    std::string result = string_builder.str();
    return result;
}

std::string Responder::build_answer_prompt(const std::string& dialogue, Age maturity)
{
    std::ostringstream string_builder;
    string_builder << " <s>[INST] <<SYS>>\nPlease return the greeting with an insult. Do not ask any questions. "
        << ".\nThe reply should be brief. NO QUESTIONS, JUST GREET.\n<</SYS >>\n\n"
        << dialogue << " [/INST]";

    std::string result = string_builder.str();
    return result;
}

std::string Responder::build_lie_prompt(const std::string& dialogue, Age maturity)
{
    std::ostringstream string_builder;
    string_builder << " <s>[INST] <<SYS>>\nPlease return the greeting with an insult. Do not ask any questions. "
        << ".\nThe reply should be brief. NO QUESTIONS, JUST GREET.\n<</SYS >>\n\n"
        << dialogue << " [/INST]";

    std::string result = string_builder.str();
    return result;
}

std::string Responder::build_assist_prompt(const std::string& dialogue, Age maturity)
{
    std::ostringstream string_builder;
    string_builder << " <s>[INST] <<SYS>>\nPlease return the greeting with an insult. Do not ask any questions. "
        << ".\nThe reply should be brief. NO QUESTIONS, JUST GREET.\n<</SYS >>\n\n"
        << dialogue << " [/INST]";

    std::string result = string_builder.str();
    return result;
}

std::string Responder::build_decline_prompt(const std::string& dialogue, Age maturity)
{
    std::ostringstream string_builder;
    string_builder << " <s>[INST] <<SYS>>\nPlease respond to the dialogue by declining it. Do not ask any questions. "
        << ".\nThe reply should be brief. NO QUESTIONS, JUST GREET.\n<</SYS >>\n\n"
        << dialogue << " [/INST]";

    std::string result = string_builder.str();
    return result;
}

std::string Responder::build_sabotage_prompt(const std::string& dialogue, Age maturity)
{
    std::ostringstream string_builder;
    string_builder << " <s>[INST] <<SYS>>\nPlease return the greeting with sabatoge. Do not ask any questions. "
        << ".\nThe reply should be brief. NO QUESTIONS, JUST GREET.\n<</SYS >>\n\n"
        << dialogue << " [/INST]";

    std::string result = string_builder.str();
    return result;
}



std::tuple<struct llama_model *, struct llama_context *> do_a_model_load(gpt_params &params) {
	auto mparams = llama_model_params_from_gpt_params(params);

	llama_model *model = llama_load_model_from_file(params.model.c_str(), mparams);
	if (model == NULL) {
		fprintf(stderr, "%s: error: failed to load model '%s'\n", __func__, params.model.c_str());
		return std::make_tuple(nullptr, nullptr);
	}

	auto cparams = llama_context_params_from_gpt_params(params);

	llama_context *lctx = llama_new_context_with_model(model, cparams);
	if (lctx == NULL) {
		fprintf(stderr, "%s: error: failed to create context with model '%s'\n", __func__, params.model.c_str());
		llama_free_model(model);
		return std::make_tuple(nullptr, nullptr);
	}

	for (unsigned int i = 0; i < params.lora_adapter.size(); ++i) {
		const std::string &lora_adapter = std::get<0>(params.lora_adapter[i]);
		float lora_scale = std::get<1>(params.lora_adapter[i]);
		int err = llama_model_apply_lora_from_file(model,
				lora_adapter.c_str(),
				lora_scale,
				((i > 0) || params.lora_base.empty())
						? NULL
						: params.lora_base.c_str(),
				params.n_threads);
		if (err != 0) {
			fprintf(stderr, "%s: error: failed to apply lora adapter\n", __func__);
			llama_free(lctx);
			llama_free_model(model);
			return std::make_tuple(nullptr, nullptr);
		}
	}

	if (params.ignore_eos) {
		params.sparams.logit_bias[llama_token_eos(model)] = -INFINITY;
	}

	{
		LOG("warming up the model with an empty run\n");

		std::vector<llama_token> tmp = {
			llama_token_bos(model),
			llama_token_eos(model),
		};
		llama_decode(lctx, llama_batch_get_one(tmp.data(), std::min(tmp.size(), (size_t)params.n_batch), 0, 0));
        llama_kv_cache_clear(lctx);
		llama_reset_timings(lctx);
	}

	return std::make_tuple(model, lctx);
}


Responder::Responder() {
    // Initialization logic
    params.sparams.temp = 0.8f;
    params.model = "C:\\Users\\James\\source\\repos\\llama.cpp\\models\\llama-2-13b-chat\\ggml-model-q4_0.gguf";
    params.seed = time(NULL);;

    // init LLM
    llama_backend_init(params.numa);


    
    std::tie(model, ctx) = do_a_model_load(params);
    if (model == NULL) {
		std::cout << "Error: unable to load model" << std::endl;
        exit(1);
    }
    if (ctx == NULL) {
		std::cout << "Error: unable to load model context" << std::endl;
        exit(1);
    }
}

Responder::~Responder() {
    // Deinitialization logic
    llama_free(ctx);
    llama_free_model(model);
    llama_backend_free();
}


std::string Responder::get_canned_response(const std::string& dialogue, Age maturity, DialogueResponseDirection response_direction, const std::string& supplemental_info)
{
    std::vector<std::string> greetings = {
        "Hello!",
        "Hi there!",
        "Good morning!",
        "Good afternoon!",
        "Good evening!",
        "How are you?",
        "How's it going?",
        "What's up?",
        "Howdy!",
        "Greetings!",
        "Nice to meet you!",
        "Pleased to meet you!",
        "It's a pleasure to meet you!",
        "Long time no see!",
        "Hey there!",
        "What's new?",
        "What's happening?",
        "How's everything?",
        "How's life treating you?",
        "How have you been?",
        "Welcome!",
        "Great to see you!",
        "Lovely to meet you!",
        "How do you do?",
        "Hiya!",
        "Good to see you!",
        "Hey!",
        "Hello again!",
        "Look who it is!",
        "Top of the morning to you!"
    };

    std::vector<std::string> insults = {
        "Bug off",
        "Get bent",
        "Cry me a river",
        "Eat my shorts",
        "Ha Ha",
        "Ok, boomer",
        "Whatever you say, boomer",
        "Jackass"
        "You're a waste of oxygen",
        "Go to hell.",
        "Idiot."
        "Moron.",
        "Suck a lemon.",
        "Take off, hoser"
    };

    std::vector<std::string> threats = {
        "You're asking for a whuppin",
        "Them is fightin words",
        "Are you threatening me?",
        "You wanna go at it?",
        "Catch me outside, how bout dat?",
        "You wanna take this outside?",
        "You asked for this",
        "Some people just need killin",
        "You're cruisin for a bruisin"
    };

    std::map<DialogueResponseDirection, std::vector<std::string>> responses = {
        { DialogueResponseDirection::Greet, greetings },
        { DialogueResponseDirection::Deride, insults },
        { DialogueResponseDirection::Threaten,  threats }
    };

    std::vector<std::string> r = responses[response_direction];
    int index = rand() % r.size();
    return r[index];
}

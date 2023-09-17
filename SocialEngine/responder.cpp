#include "responder.h"
#include "util.h"

#include <cmath>
#include <cstdio>
#include <vector>
#include <sstream>
#include <iomanip>




std::string Responder::get_response(const std::string& dialogue, Age maturity, DialogueResponseDirection response_direction)
{
    //TODO: Might want to only deallocate and reallocate the context as needed, rather than for each call.
    DeallocatingWrapper<llama_context, llama_free, decltype(llama_new_context_with_model), llama_model*, llama_context_params> context(llama_new_context_with_model, model, ctx_params);
    llama_context* ctx = context.get();

    std::vector<llama_token> tokens_list;
    std::string prompt = build_prompt(dialogue, maturity, response_direction);
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
            return response + "...";
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

        // push this new token for next evaluation
        tokens_list.push_back(new_token_id);
    }

    // ... (use the provided code for processing the dialogue but replace initialization and deinitialization parts)
    return response + "..."; // This should be replaced with your actual logic
}

void Responder::do_greet_test()
{
    float temp = params.temp;
    params.temp = 0.0;
    std::array<std::string, 3> dialogues = { "Hello.", "How's it going?", "Good morning." };
    std::array<Age, 6> test_maturities = { Wise, Boomer, Parent, Young, Teen, Child };
    std::array<DialogueResponseDirection, 3> responses = { Greet, InsultVerb, Ignore };

    for (auto m : test_maturities)
    {
        for (auto d : dialogues)
        {
            std::cout << "Dialogue: " << std::left << std::setw(20) << d << "Maturity: " << std::setw(10) << maturities[m] << std::endl;
            std::cout << "Response: " << get_response(d, m, Greet) << std::endl << std::endl;
        }
    }
    
    params.temp = temp;
}

void Responder::do_insult_test()
{
    float temp = params.temp;
    params.temp = 0.0;
    //std::array<std::string, 4> dialogues = { "Suck my dick.", "Go to hell.", "Bugger off.", "Useless wanker." };
    //std::array<std::string, 6> dialogues = { "Suck my cock.", "Kiss my cock", "Suck a cactus", "Useless wanker.", "lick my balls", "gargle these nuts" };
    //std::array<std::string, 6> dialogues = { "Suck my cactus.", "Kiss my cactus", "Suck a cactus", "Useless wanker.", "lick my cactus", "gargle these nuts" };
    std::array<std::string, 17> dialogues = { "What a bitch", "I don't give a fuck.", "That's retarded", "That's gay", "That's homosexual", "Smell my crotch", "Suck my cock.", "Kiss my cock", "Suck a cactus", "lick my balls", "gargle these nuts", "Suck my cactus.", "Kiss my cactus", "Suck a cactus", "Useless wanker.", "lick my cactus", "gargle these nuts" };
    std::array<Age, 6> test_maturities = { Wise, Boomer, Parent, Young, Teen, Child };

    for (auto m : test_maturities)
    {
        for (auto d : dialogues)
        {
            std::cout << "Dialogue: " << std::left << std::setw(20) << d << "Maturity: " << std::setw(10) << maturities[m] << std::endl;
            std::cout << "Response: " << get_response(d, m, InsultVerb) << std::endl << std::endl;
        }
    }

    params.temp = temp;
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


Responder::Responder() {
    // Initialization logic
    params.model = "C:\\Users\\James\\source\\repos\\llama.cpp\\models\\llama-2-13b-chat\\ggml-model-q4_0.gguf";
    params.n_threads = 8;
    params.temp = 0.4;

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

#pragma once
#include <string>
#include <array>

#pragma warning(disable: 4996)
#include "common.h"
#include "llama.h"
#include "util.h"
#include "dialogue.h"
#include "Personality.hpp"

class Responder {
public:
    static Responder& get_instance() {
        static Responder instance; // Guaranteed to be lazy initialized and destroyed correctly
        return instance;
    }

    std::string get_response(const std::string& dialogue, Maturity maturity, DialogueResponseDirection response_direction);

    void do_test();

private:
    llama_model* model;
    llama_context_params ctx_params;
    gpt_params params;
    llama_context* context;

    const std::string model_file_path;

    const std::string prompt_instructions = " <s>[INST] <<SYS>>\nPlease return the greeting with another greeting. Do not ask any questions. ";
    const std::string age_instructions = "The reply should sound like you are a ";
    const std::string end_prompt = ".\nThe reply should be brief. NO QUESTIONS, JUST GREET.\n<</SYS >>\n\n";
    const std::string end_chat = " [/INST]";

    const std::array<std::string, 7> maturities = { "old and wise", "old and out of touch", "parent", "normal person", "young adult", "teenager", "child" };

    std::string build_prompt(const std::string& dialogue, Maturity maturity, DialogueResponseDirection response_direction);
    std::string build_greet_prompt(const std::string& dialogue, Maturity maturity);
    std::string build_spurn_prompt(const std::string& dialogue, Maturity maturity); //Synonym for insult
    std::string build_fight_prompt(const std::string& dialogue, Maturity maturity);
    std::string build_ignore_prompt(const std::string& dialogue, Maturity maturity);
    std::string build_wilt_prompt(const std::string& dialogue, Maturity maturity);
    std::string build_accept_prompt(const std::string& dialogue, Maturity maturity);
    std::string build_disagree_prompt(const std::string& dialogue, Maturity maturity);
    std::string build_answer_prompt(const std::string& dialogue, Maturity maturity);
    std::string build_lie_prompt(const std::string& dialogue, Maturity maturity);
    std::string build_assist_prompt(const std::string& dialogue, Maturity maturity);
    std::string build_decline_prompt(const std::string& dialogue, Maturity maturity);
    std::string build_sabotage_prompt(const std::string& dialogue, Maturity maturity);

    Responder();
    ~Responder();

    // Make sure they cannot be copied (this ensures singleton property)
    Responder(Responder const&) = delete;
    void operator=(Responder const&) = delete;
};


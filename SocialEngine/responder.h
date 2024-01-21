#pragma once
#include <string>
#include <array>

#pragma warning(disable: 4996)
#include "common.h"
#include "llama.h"
#include "util.h"
#include "dialogue.h"
#include "personality.h"




class Responder {
public:
    static Responder& get_instance() {
        static Responder instance; // Guaranteed to be lazy initialized and destroyed correctly
        return instance;
	}
	void deinit() {
		Responder::~Responder();
	}

	std::shared_ptr<DialogueResponse> get_response(const std::string &dialogue, DialogueResponseDirection response_direction, Personality personality, const std::string &supplemental_info = "", bool use_llama = false);
	std::string get_response_synchronously(const std::string &dialogue, DialogueResponseDirection response_direction, Personality personality, const std::string &supplemental_info = "", bool use_llama = false);

    void do_greet_test();
    void do_insult_test();

private:
    llama_model* model;
    gpt_params params;
    llama_context* ctx;

    void get_llama_response(const std::string &dialogue, std::shared_ptr<DialogueResponse> response, DialogueResponseDirection response_direction, Personality personality, const std::string &supplemental_info = "");
	std::shared_ptr<DialogueResponse> get_canned_response(const std::string &dialogue, DialogueResponseDirection response_direction, const std::string &supplemental_info = "");

    const std::string model_file_path;

    std::string build_prompt(const std::string &dialogue, DialogueResponseDirection response_direction, Personality personality, std::string supplemental_info);
	std::string build_personality_description(Personality personality);
	std::string build_greet_prompt(const std::string &dialogue, std::string personality_description, std::string supplemental_info);
	std::string build_insult_prompt(const std::string &dialogue, std::string personality_description, std::string supplemental_info); //Synonym for insult
	std::string build_fight_prompt(const std::string &dialogue, std::string personality_description, std::string supplemental_info);
	std::string build_ignore_prompt(const std::string &dialogue, std::string personality_description, std::string supplemental_info);
	std::string build_wilt_prompt(const std::string &dialogue, std::string personality_description, std::string supplemental_info);
	std::string build_accept_prompt(const std::string &dialogue, std::string personality_description, std::string supplemental_info);
	std::string build_disagree_prompt(const std::string &dialogue, std::string personality_description, std::string supplemental_info);
	std::string build_answer_prompt(const std::string &dialogue, std::string personality_description, std::string supplemental_info);
	std::string build_lie_prompt(const std::string &dialogue, std::string personality_description, std::string supplemental_info);
	std::string build_assist_prompt(const std::string &dialogue, std::string personality_description, std::string supplemental_info);
	std::string build_decline_prompt(const std::string &dialogue, std::string personality_description, std::string supplemental_info);
	std::string build_sabotage_prompt(const std::string &dialogue, std::string personality_description, std::string supplemental_info);
	std::string build_thank_prompt(const std::string &dialogue, std::string personality_description, std::string supplemental_info);

    Responder();
    ~Responder();

    // Make sure they cannot be copied (this ensures singleton property)
    Responder(Responder const&) = delete;
    void operator=(Responder const&) = delete;
};


#pragma once
#include <string>
#include <array>

#pragma warning(disable: 4996)
#include "common.h"
#include "llama.h"

enum Age
{
    Old,
    Middle,
    Young,
    Teen,
    Child
};

enum Enthsiasm
{
    Enthusiastic,
    Mild,
    Reluctant,
    Angry
};

class Responder {
public:
    static Responder& get_instance() {
        static Responder instance; // Guaranteed to be lazy initialized and destroyed correctly
        return instance;
    }

    std::string get_response(const std::string& dialogue, Age age, Enthsiasm enthusiasm);

private:
    llama_model* model;
    llama_context_params ctx_params;
    gpt_params params;

    const std::string prompt_instructions = "<s>[INST] <<SYS>>\nPlease return the greeting with another greeting.Do not ask any questions.\nThe reply should be brief. NO QUESTIONS, JUST GREET.";
    const std::string disposition_instructions = "The reply should be moderated";
    const std::string age_instructions = " and sound like you are a child.";
    const std::string end_prompt = "\n<</SYS >>\n\n";
    const std::string end_chat = "[/ INST];";

    const std::array<std::string, 5> age = { "elder", "parent", "adult", "teenager", "child" };
    const std::array<std::string, 4> emotion = { "enthusiastic", "moderated", "meek and tepid", "terse and angry" };

    std::string build_prompt(const std::string& dialogue);

    Responder();
    ~Responder();

    // Make sure they cannot be copied (this ensures singleton property)
    Responder(Responder const&) = delete;
    void operator=(Responder const&) = delete;
};


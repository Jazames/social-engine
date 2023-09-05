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

enum Enthusiasm
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

    std::string get_response(const std::string& dialogue, Age age, Enthusiasm enthusiasm);

    void do_test();

private:
    llama_model* model;
    llama_context_params ctx_params;
    gpt_params params;

    const std::string prompt_instructions = " <s>[INST] <<SYS>>\nPlease return the greeting with another greeting. Do not ask any questions. ";
    const std::string disposition_instructions = "The reply should be ";
    const std::string age_instructions = " and sound like you are a ";
    const std::string end_prompt = ".\nThe reply should be brief. NO QUESTIONS, JUST GREET.\n<</SYS >>\n\n";
    const std::string end_chat = " [/INST]";

    const std::array<std::string, 5> generation = { "elder", "parent", "adult", "teenager", "child" };
    const std::array<std::string, 4> emotion = { "enthusiastic", "brief", "meek and tepid", "terse and angry" };

    std::string build_prompt(const std::string& dialogue, Age age, Enthusiasm enthusiasm);

    Responder();
    ~Responder();

    // Make sure they cannot be copied (this ensures singleton property)
    Responder(Responder const&) = delete;
    void operator=(Responder const&) = delete;
};


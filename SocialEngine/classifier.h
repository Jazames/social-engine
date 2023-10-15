#pragma once
#include <string>

#pragma warning(disable: 4996)
#include "common.h"
#include "llama.h"



enum DialogueType {
    IncoherentRambling, //This is used in the case of error.
    Greeting,
    Compliment,
    InsultNoun,
    Request,
    Question,
    Statement
};


class Classifier {
public:
    static Classifier& get_instance() {
        static Classifier instance; // Guaranteed to be lazy initialized and destroyed correctly
        return instance;
    }

    DialogueType get_classification(const std::string& dialogue);

private:
    llama_model* model;
    llama_context_params ctx_params;
    gpt_params params;

    const std::string prompt_instructions = "Identify whether the given sentences are greetings, insults, questions, requests, or statements. Rude language does not automatically make something an insult. \
        Use only a single word from this list: insult, question, greeting, compliment, request, statement.\n\n\
        Good Morning.\n Greeting\n\nWhere are the canned oranges ?\nQuestion\n\nThanks for your help.\nCompliment\n\nCan you help me get that balloon down ?\nRequest\n\nIm looking for a new sword.Where can I buy a soward ?\nQuestion\n\nI don't like cheese.\nStatement\n\n\
        Are you a prostitute?\nQuestion.\n\nHowdy.\nGreeting\n\nYour mom goes to college.\nInsult\n\nWhere can I find a whore? \nQuestion\n\nCan I have a room for the night and a hot meal ? \nRequest\n\nThis place is boring.\nStatement\n\n";

    Classifier();

    ~Classifier();

    // Make sure they cannot be copied (this ensures singleton property)
    Classifier(Classifier const&) = delete;
    void operator=(Classifier const&) = delete;
};

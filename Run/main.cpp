
#include <iostream>
#include <string>

#include "social.h"
#include "classifier.h"
#include "bert_classifier.h"
#include "responder.h"
#include "llama_embed.h"





const char* DialogueTypeNames[] = {
    "IncoherentRambling",
    "Greeting",
    "Insult",
    "Request",
    "Question",
    "Statement"
};

const char* ToString(DialogueType type) {
    if (type < 0 || type >= 6) {
        return "InvalidDialogueType";
    }
    return DialogueTypeNames[type];
}


int main()
{
    std::cout << "Warming up." << std::endl;
    std::string dialogue = "Hello fine kitty.";// "Are you a whore?";
	Personality personality;
	personality.personality_override = "a child who loves all cute things ";
    Embedder::get_instance().get_embedding(dialogue);
    auto classification = BertClassifier::get_instance().get_classification(dialogue);
    std::string response = Responder::get_instance().get_response_synchronously(dialogue, Greet, personality);
    std::cout << "Dialogue to classify: " << dialogue << std::endl;
    std::cout << "Classification is: " << ToString(classification) << std::endl;
    std::cout << "Response is: " << response << std::endl;
    std::cout << "NPC response is: " << get_default_response_synchronous(dialogue) << std::endl;
    std::cout << "Finished warming up" << std::endl << std::endl;


    std::string player_words = "Go hang a salami, I'm a lasagna hog.";
    Appearance player_appearance = Appearance();
    Personality npc_personality = Personality();
    Knowledge knowledge = Knowledge();
    npc_personality.maturity = 0.3;
	response = get_npc_response_synchronous(player_words, player_appearance, npc_personality, knowledge);

    std::cout << "NPC response: " << response << std::endl;

    std::cout << std::endl << "Doing Test" << std::endl;
    Responder::get_instance().do_insult_test();
}

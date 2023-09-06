
#include <iostream>
#include <string>

#include "social.h"
#include "classifier.h"
#include "responder.h"





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
    std::string dialogue = "Hello.";// "Are you a whore?";
    auto classification = Classifier::getInstance().get_classification(dialogue);
    std::string response = Responder::get_instance().get_response(dialogue, Young, Greet);
    std::cout << "Dialogue to classify: " << dialogue << std::endl;
    std::cout << "Classification is: " << ToString(classification) << std::endl;
    std::cout << "Response is: " << response << std::endl;
    std::cout << "Finished warming up" << std::endl << std::endl;


    std::string player_words = "Go hang a salami, I'm a lasagna hog.";
    Appearance player_appearance = Appearance();
    Personality npc_personality = Personality();
    Knowledge knowledge = Knowledge();
    npc_personality.maturity = Young;
    response = get_npc_response(player_words, player_appearance, npc_personality, knowledge);

    std::cout << "NPC response: " << response << std::endl;

    std::cout << std::endl << "Doing Test" << std::endl;
    Responder::get_instance().do_test();
}
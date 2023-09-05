// SocialEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <algorithm>
#include "Personality.hpp"
#include "classifier.h"
#include "responder.h"


struct Disposition {
    double friendliness = 0; // 0 is neutral, negative is bad, positive is good.

    /*
    Disposition()
    {
        this->friendliness = 0.5;
    }

    Disposition(double friendliness)
    {
        this->friendliness = friendliness;
    }
    */
};

struct CharacterKnowledge
{
    uint64_t face_id = 0;
    Disposition disposition;
    //TODO: Put some stuff here about things it's known this guy has/can do.
};

struct Knowledge {
    std::vector<uint64_t> known_faces() const
    {
        //TODO: Might need to cache this
        std::vector<uint64_t> face_ids;
        for (const CharacterKnowledge& person : known_people)  // Use a reference to avoid copying
        {
            face_ids.push_back(person.face_id);
        }
        return face_ids;
    }

    CharacterKnowledge get_character(uint64_t face_id) const
    {
        auto iterator = std::find_if(known_people.begin(), known_people.end(), [face_id](const CharacterKnowledge& item)
        {
            return item.face_id == face_id;
        });

        if (iterator != known_people.end()) {
            return (*iterator);
        }

        return CharacterKnowledge();
    }
    std::vector<CharacterKnowledge> known_people;
};

enum DialogueResponseDirection {
    Greet,
    Spurn, //Synonmy for insult
    Fight,
    Ignore,
    Wilt,
    Accept,
    Disagree,
    Answer,
    Lie,
    Assist,
    Decline,
    Sabotage
};


Disposition get_disposition(Appearance appearance, Knowledge knowledge, Personality personality)
{
    //This assumes everyone has the same knowledge about the cultural weight of specific clothing being worn. Which is probably fine for now.

    double disposition_weight = 0.15;
    double hair_weight = disposition_weight;
    double clothing_weight = disposition_weight * 1.6;
    CharacterKnowledge character = knowledge.get_character(appearance.face_id);
    if (character.face_id != 0)
    {
        //Note: Faceblindness should be handled when learning faces, not when storing known faces. 
        return character.disposition;
    }

    //Initial disposition for complete stranger.
    Disposition disposition = Disposition();
    disposition.friendliness = 0.0;
    disposition.friendliness += personality.traits.Enthusiasm * disposition_weight;
    disposition.friendliness -= personality.traits.Withdrawal * disposition_weight;

    //Factor in hair
    disposition.friendliness += ((appearance.hair.neatness_weight * personality.traits.Orderliness) * hair_weight);
    disposition.friendliness += ((appearance.hair.foreign_weight * personality.traits.Openness) * hair_weight);
    disposition.friendliness += ((appearance.hair.authority_weight * personality.morals.authority_subversion) * hair_weight);
    disposition.friendliness += ((appearance.hair.religious_weight * personality.morals.sanctity_degradation) * hair_weight);

    //Factor in clothing
    double clothes_count = (double)appearance.clothes.size();
    for (auto c : appearance.clothes)
    {
        disposition.friendliness += ((c.neatness_weight * personality.traits.Orderliness) * clothing_weight) / clothes_count;
        disposition.friendliness += ((c.foreign_weight * personality.traits.Openness) * clothing_weight) / clothes_count;
        disposition.friendliness += ((c.authority_weight * personality.morals.authority_subversion) * clothing_weight) / clothes_count;
        disposition.friendliness += ((c.religious_weight * personality.morals.sanctity_degradation) * clothing_weight) / clothes_count;
    }

    return disposition;
}

DialogueResponseDirection get_greeting_response_direction(Disposition disposition, Personality personality)
{
    double d = disposition.friendliness;

    if (d > 0)
    {
        d *= (1.0 + (personality.morals.loyalty_betrayal * 0.3));
    }

    d += personality.traits.Agreeableness() * 0.4;
    d += personality.traits.Extraversion() * 0.2;
    d -= personality.traits.Neuroticism() * 0.25;
    d += personality.traits.Orderliness * 0.05;
    d += personality.morals.care_harm * 0.15;
    d += personality.morals.authority_subversion * 0.05;

    if (d > 0.2)
    {
        return DialogueResponseDirection::Greet;
    }
    else if (d > -0.6)
    {
        return DialogueResponseDirection::Ignore;
    }
    else if (d < -2.0)
    {
        return DialogueResponseDirection::Fight;
    }
    else
    {
        return DialogueResponseDirection::Spurn;
    }

}

// Get the direction of dialogue response based on disposition, personality, and dialogue type
DialogueResponseDirection get_dialogue_response_direction(Disposition disposition, Personality personality, DialogueType dialogueType)
{
    DialogueResponseDirection direction;

    // Stub: Placeholder logic. Actual logic to determine dialogue direction goes here.
    // E.g., if disposition is neutral, personality is aggressive, and dialogue type is greeting, set direction to "Insult".

    return Greet;
}

// Update character's knowledge based on the interaction
Knowledge update_knowledge_from_interaction(Knowledge knowledge, DialogueResponseDirection direction)
{

    // Stub: Placeholder logic. Actual logic to update the knowledge goes here.
    // E.g., if direction is "Lie", update knowledge to mark the player as untrustworthy.

    return knowledge;
}

// Get a string response based on dialogue direction, original dialogue, and dialogue type
std::string get_response(DialogueResponseDirection direction, std::string dialogue, DialogueType dialogueType)
{
    //TODO: incorporate direction and get Age/Enthusiasm
    std::string response = Responder::get_instance().get_response(dialogue, Teen, Enthusiastic);


    return response;
}

// Get a string response based on dialogue direction, original dialogue, and dialogue type
std::string get_response(DialogueResponseDirection direction, std::string dialogue, Knowledge knowledge, DialogueType dialogueType)
{
    std::string response;

    // Stub: Placeholder logic. Actual logic to generate a response string goes here.
    // E.g., if direction is "Greet", return "Hello, traveler!"

    return response;
}

DialogueType get_classification(std::string dialogue)
{
    return Classifier::getInstance().get_classification(dialogue);
}


std::string get_npc_greeting_response(std::string dialogue, Appearance appearance, Personality personality, Knowledge knowledge) {
    Disposition disposition = get_disposition(appearance, knowledge, personality);
    DialogueResponseDirection direction = get_greeting_response_direction(disposition, personality);
    knowledge = update_knowledge_from_interaction(knowledge, direction);
    std::string response = get_response(direction, dialogue, DialogueType::Greeting);
    
    //TODO: update actions/figure out if leave conversation
    return response;
}

std::string get_npc_insult_response(std::string dialogue, Appearance appearance, Personality personality, Knowledge knowledge) {
    Disposition disposition = get_disposition(appearance, knowledge, personality);
    DialogueResponseDirection direction = get_dialogue_response_direction(disposition, personality, DialogueType::Insult);
    knowledge = update_knowledge_from_interaction(knowledge, direction);
    std::string response = get_response(direction, dialogue, DialogueType::Insult);

    //TODO: update actions/figure out if leave conversation
    return response;
}

std::string get_npc_statement_response(std::string dialogue, Appearance appearance, Personality personality, Knowledge knowledge) {
    Disposition disposition = get_disposition(appearance, knowledge, personality);
    DialogueResponseDirection direction = get_dialogue_response_direction(disposition, personality, DialogueType::Statement);
    knowledge = update_knowledge_from_interaction(knowledge, direction);
    std::string response = get_response(direction, dialogue, DialogueType::Statement);

    //TODO: update actions/figure out if leave conversation
    return response;
}

std::string get_npc_question_response(std::string dialogue, Appearance appearance, Personality personality, Knowledge knowledge) {
    Disposition disposition = get_disposition(appearance, knowledge, personality);
    DialogueResponseDirection direction = get_dialogue_response_direction(disposition, personality, DialogueType::Question);
    
    //TODO: Might need something to get filtered knowledge?
    knowledge = update_knowledge_from_interaction(knowledge, direction);
    std::string response = get_response(direction, dialogue, knowledge, DialogueType::Question);

    //TODO: update actions/figure out if leave conversation
    return response;
}

std::string get_npc_request_response(std::string dialogue, Appearance appearance, Personality personality, Knowledge knowledge) {
    Disposition disposition = get_disposition(appearance, knowledge, personality);
    //TODO: Figure out how to Get Check knowledge to determine what's required for the request, whether that aligns with goals, etc.

    DialogueResponseDirection direction = get_dialogue_response_direction(disposition, personality, DialogueType::Question);

    //TODO: Might need something to get filtered knowledge?
    knowledge = update_knowledge_from_interaction(knowledge, direction);
    std::string response = get_response(direction, dialogue, knowledge, DialogueType::Question);

    //TODO: update actions/figure out if leave conversation
    return response;
}


std::string get_npc_response(std::string dialogue, Appearance appearance, Personality personality, Knowledge knowledge)
{
    DialogueType type = get_classification(dialogue);
    switch (type) 
    {
    case Greeting:
        return get_npc_greeting_response(dialogue, appearance, personality, knowledge);
    case Insult:
        return get_npc_insult_response(dialogue, appearance, personality, knowledge);
    case Request:
        return get_npc_request_response(dialogue, appearance, personality, knowledge);
    case Question:
        return get_npc_question_response(dialogue, appearance, personality, knowledge);
    case Statement:
        return get_npc_statement_response(dialogue, appearance, personality, knowledge);
    default:
        std::cout << "Unknown classification." << std::endl;
        return "Wut?";
    }
}

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
    std::string response = Responder::get_instance().get_response(dialogue, Young, Mild);
    std::cout << "Dialogue to classify: " << dialogue << std::endl;
    std::cout << "Classification is: " <<  ToString(classification) << std::endl;
    std::cout << "Response is: " << response << std::endl;
    std::cout << "Finished warming up" << std::endl << std::endl;




    std::string player_words = "Go hang a salami, I'm a lasagna hog.";
    Appearance player_appearance = Appearance();
    Personality npc_personality = Personality();
    Knowledge knowledge = Knowledge();
    response = get_npc_response(player_words, player_appearance, npc_personality, knowledge);

    std::cout << "NPC response: " << response << std::endl;

    std::cout << std::endl << "Doing Test" << std::endl;
    Responder::get_instance().do_test();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file






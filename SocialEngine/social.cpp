

#include <iostream>
#include <algorithm>
#include "personality.h"
#include "classifier.h"
#include "responder.h"
#include "dialogue.h"
#include "global_knowledge.h"


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
    for (const auto c : appearance.clothes)
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

    double timidity = personality.traits.Neuroticism() - personality.traits.Extraversion();
    timidity -= personality.traits.Politeness;

    if (disposition.friendliness > 0)
    {
        if (timidity > disposition.friendliness)
        {
            return Wilt;
        }
        else
        {
            return Greet;
        }
    }

    double politeness = 0;
    politeness += 0.5 * personality.traits.Agreeableness();
    politeness += 0.2 * personality.traits.Enthusiasm;
    politeness += 0.1 * personality.traits.Orderliness;
    politeness += 0.3 * personality.morals.care_harm;
    politeness += 0.1 * personality.morals.authority_subversion;

    if (politeness > 0)
    {
        if (timidity > politeness)
        {
            return Wilt;
        }
        else
        {
            return Greet;
        }
    }
    else
    {
        double anger = disposition.friendliness * -1.0;
        anger -= politeness;
        
        double gonna_act = 0.0;
        gonna_act += 0.3 * personality.traits.Assertiveness;
        gonna_act += 0.3 * personality.traits.Volatility;
        gonna_act += 0.1 * personality.traits.Industriousness;
        gonna_act -= 0.3 * personality.traits.Withdrawal;

        if (anger > 0.2 && gonna_act > 0.2)
        {
            if (gonna_act + anger > 0.4)
            {
                return Threaten;
            }
            else
            {
                return Deride;
            }
        }
        else
        {
            if (anger > 0.5)
            {
                return Deride;
            }
            else
            {
                return Ignore;
            }
        }
    }
}


DialogueResponseDirection get_compliment_response_direction(Disposition disposition, Personality personality)
{
    return Deride;
}

DialogueResponseDirection get_insult_response_direction(Disposition disposition, Personality personality)
{
    double upstanding = 0.0;
    upstanding += 0.3 * personality.maturity;
    upstanding += 0.3 * personality.morals.care_harm;
    upstanding -= 0.1 * personality.traits.Volatility;
    upstanding += 0.1 * personality.traits.Politeness;
    upstanding += 0.1 * personality.traits.Orderliness;
    upstanding -= 0.2 * disposition.friendliness;
    
    if (upstanding > 0.35)
    {
        double contrarian = 0.0;
        contrarian -= 0.3 * personality.traits.Withdrawal;
        contrarian += 0.1 * personality.morals.liberty_oppression;
        contrarian -= 0.2 * personality.morals.loyalty_betrayal;
        contrarian += 0.2 * personality.traits.Assertiveness;
        contrarian += 0.1 * disposition.friendliness;
        if (contrarian > 0)
        {
            return Greet;
        }
        else
        {
            return Ignore;
        }
    }
    else
    {
        double weak = 0.0;
        weak += 0.5 * personality.traits.Withdrawal;
        weak -= 0.3 * personality.traits.Assertiveness;
        weak -= 0.2 * personality.morals.liberty_oppression;
        if (weak > 0)
        {
            return Wilt;
        }

        double angery = 0.0;
        angery -= 0.9 * disposition.friendliness;
        angery += 0.3 * personality.traits.Volatility;
        angery -= 0.4 * personality.traits.Agreeableness();
        angery += 0.2 * personality.traits.Assertiveness;
        angery -= 0.5 * personality.morals.care_harm;
        angery += 0.5 * personality.morals.loyalty_betrayal;
        angery -= 0.2 * personality.morals.fairness_cheating;

        if (angery > 0.5)
        {
            return Threaten;
        }
        else if (angery > 0)
        {
            return Deride;
        }
        else
        {
            return Ignore;
        }
    }
}

DialogueResponseDirection get_question_response_direction(Disposition disposition, Personality personality)
{
    double desire_to_help = disposition.friendliness;
    desire_to_help += 0.3 * personality.traits.Politeness;
    desire_to_help += 0.3 * personality.traits.Intellect;
    desire_to_help += 0.1 * personality.traits.Openness;
    desire_to_help += 0.1 * personality.traits.Enthusiasm;
    desire_to_help += 0.2 * personality.morals.care_harm;
    desire_to_help -= 0.2 * personality.traits.Withdrawal;
    desire_to_help += 0.1 * personality.morals.loyalty_betrayal;

    if (desire_to_help > 0)
    {
        return Answer;
    }
    else
    {
        double malicious = 0.0;
        malicious += 0.1 * personality.traits.Volatility;
        malicious -= 0.5 * personality.morals.care_harm;
        malicious -= 0.2 * personality.morals.loyalty_betrayal;
        malicious -= 0.2 * personality.morals.authority_subversion;
        malicious -= 0.2 * personality.morals.fairness_cheating;
        
        if (malicious > 0)
        {
            if (malicious > 0.3)
            {
                double unhinged = 0.0;
                unhinged += 0.5 * personality.traits.Volatility;
                unhinged -= 0.1 * personality.traits.Withdrawal;
                unhinged += 0.1 * personality.traits.Assertiveness;
                if (unhinged > 0.0)
                {
                    return Threaten;
                }
                else
                {
                    return Lie;
                }
            }
            else
            {
			    return Deride;
            }
        }
        else
        {
            double avoidance = 0.0;
            avoidance += 0.5 * personality.traits.Withdrawal;
            avoidance -= 0.3 * personality.traits.Assertiveness;
            avoidance -= 0.2 * personality.traits.Openness;
            if (avoidance > 0)
            {
                return Wilt;
            }
			else
			{
				return Ignore;
			}
        }
    }
}

DialogueResponseDirection get_statement_response_direction(Disposition disposition, Personality personality)
{
    return Deride;
}

DialogueResponseDirection get_request_response_direction(Disposition disposition, Personality personality)
{
    return Deride;
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

std::string get_relevant_knowledge(std::string dialogue, Knowledge knowledge)
{
    const GlobalKnowledge& gk = GlobalKnowledge::get_instance();
	std::string supplemental_info = "";

    std::vector<std::string> known_facts = gk.get_closest_items(dialogue, knowledge.known_fact_ids, 20);
    int i = 0;
    while (supplemental_info.size() < 512)
    {
        supplemental_info += known_facts[i++] + "\n\n";
    }

	return supplemental_info;
}

// Get a string response based on dialogue direction, original dialogue, and dialogue type
std::string get_response(DialogueResponseDirection direction, std::string dialogue, Age maturity, Knowledge knowledge, DialogueType dialogueType)
{
    std::string supplemental_info = "";
    if (direction == DialogueResponseDirection::Answer)
    {
        supplemental_info = get_relevant_knowledge(dialogue, knowledge);
    }

    //TODO: incorporate direction and get Age/Enthusiasm
    std::string response = Responder::get_instance().get_response(dialogue, maturity, direction, supplemental_info);


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
    return Classifier::get_instance().get_classification(dialogue);
}

std::string get_npc_greeting_response(std::string dialogue, Appearance appearance, Personality personality, Knowledge knowledge) {
    Disposition disposition = get_disposition(appearance, knowledge, personality);
    DialogueResponseDirection direction = get_greeting_response_direction(disposition, personality);
    knowledge = update_knowledge_from_interaction(knowledge, direction);
    std::string response = get_response(direction, dialogue, personality.age, knowledge, DialogueType::Greeting);
    
    //TODO: update actions/figure out if leave conversation
    return response;
}

std::string get_npc_insult_response(std::string dialogue, Appearance appearance, Personality personality, Knowledge knowledge) {
    Disposition disposition = get_disposition(appearance, knowledge, personality);
    DialogueResponseDirection direction = get_insult_response_direction(disposition, personality);
    knowledge = update_knowledge_from_interaction(knowledge, direction);
    std::string response = get_response(direction, dialogue, personality.age, knowledge, DialogueType::Insult);

    //TODO: update actions/figure out if leave conversation
    return response;
}

std::string get_npc_statement_response(std::string dialogue, Appearance appearance, Personality personality, Knowledge knowledge) {
    Disposition disposition = get_disposition(appearance, knowledge, personality);
    DialogueResponseDirection direction = get_dialogue_response_direction(disposition, personality, DialogueType::Statement);
    knowledge = update_knowledge_from_interaction(knowledge, direction);
    std::string response = get_response(direction, dialogue, personality.age, knowledge, DialogueType::Statement);

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



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file






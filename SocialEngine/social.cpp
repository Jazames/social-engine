

#include <iostream>
#include <algorithm>
#include "personality.h"
#include "bert_classifier.h"
#include "responder.h"
#include "dialogue.h"
#include "global_knowledge.h"
#include "bert_embed.h"

void init_social_engine()
{
	BertClassifier::get_instance();
	GlobalKnowledge::get_instance();
	Responder::get_instance();
}

void deinit_social_engine()
{
	BertClassifier::get_instance().deinit();
	GlobalKnowledge::get_instance().deinit();
	Responder::get_instance().deinit();
	BertEmbedder::get_instance().deinit();
}

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
    double shyness = 0.0;
    shyness += 0.8 * personality.traits.Withdrawal;
    shyness -= 0.2 * personality.traits.Assertiveness;
    shyness -= 0.2 * personality.traits.Openness;
    shyness -= 0.2 * personality.traits.Enthusiasm;

    double maliciousness = 0.0;
    maliciousness -= 0.5 * personality.morals.care_harm;
    maliciousness -= 0.2 * personality.morals.loyalty_betrayal;
    maliciousness -= 0.8 * personality.traits.Compassion;
    maliciousness -= 0.2 * personality.traits.Politeness;

    double manners = 0.0;
    manners += 0.6 * personality.traits.Politeness;
    manners += 0.2 * personality.traits.Orderliness;
    manners += 0.2 * personality.morals.care_harm;
    manners += 0.2 * personality.morals.loyalty_betrayal;

    double unhinged = 0.0;
    unhinged += 1.0 * personality.traits.Volatility;
    unhinged -= 0.2 * personality.morals.sanctity_degradation;

    double agency = 0.0;
    agency += 0.6 * personality.traits.Assertiveness;
    agency += 0.4 * personality.traits.Industriousness;
    agency += 0.2 * personality.morals.liberty_oppression;
    agency -= 0.2 * personality.traits.Politeness;
    agency -= 0.2 * personality.morals.fairness_cheating;

    if (shyness > 0)
    {
        if (maliciousness > 0)
        {
            if (unhinged > 0)
            {
				return Threaten;
			}
            else
            {
				return Deride;
			}
		}
        else if (manners > 0.2)
        {
            return Thank;
        }
        else
        {
            if (agency > 0)
            {
                return Ignore;
            }
            else
            {
                return Wilt;
            }
		}
	}
    else
    {
        if (maliciousness > 0)
        {
            if (unhinged > 0)
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
            return Thank;
		}
    }
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
	desire_to_help += 0.3 * personality.traits.Compassion;
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
    DialogueResponseDirection direction = Greet;

    // Stub: Placeholder logic. Actual logic to determine dialogue direction goes here.
    // E.g., if disposition is neutral, personality is aggressive, and dialogue type is greeting, set direction to "Insult".

    return direction;
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
    while (supplemental_info.size() < 512 && known_facts.size() > i)
    {
        supplemental_info += known_facts[i++] + "\n\n";
    }

	return supplemental_info;
}

// Get a string response based on dialogue direction, original dialogue, and dialogue type
std::shared_ptr<DialogueResponse> get_response(InteractionParameters parameters) {
	std::cout << "Response direction is" << get_response_direction_name(parameters.response_direction) << std::endl;
    std::string supplemental_info = "";
	if (parameters.response_direction == DialogueResponseDirection::Answer)
    {
		supplemental_info = get_relevant_knowledge(parameters.dialogue, parameters.knowledge);
    }

    //TODO: responses should be as though they're coming from the personality type.
	std::shared_ptr<DialogueResponse> response = Responder::get_instance().get_response(parameters);
	//std::string response = "Go hand me a beer.";

    return response;
}

Knowledge& add_to_knowlege(std::string dialogue, Knowledge& knowledge)
{
	GlobalKnowledge& gk = GlobalKnowledge::get_instance();
	int knowledge_id = gk.add_knowledge(dialogue);
	knowledge.known_fact_ids.push_back(knowledge_id);
	return knowledge;
}

DialogueType get_classification(std::string dialogue)
{
    return BertClassifier::get_instance().get_classification(dialogue);
}

DialogueResponseDirection get_response_direction(Disposition disposition, Personality personality, DialogueType dialogue_type) {
	switch (dialogue_type) {
		case Greeting:
			return get_greeting_response_direction(disposition, personality);
		case Insult:
			return get_insult_response_direction(disposition, personality);
		case Request:
			return get_request_response_direction(disposition, personality);
		case Question:
			return get_question_response_direction(disposition, personality);
		case Statement:
			return get_statement_response_direction(disposition, personality);
		case Compliment:
			return get_compliment_response_direction(disposition, personality);
		default:
			std::cout << "Unknown classification." << std::endl;
			return Ignore;
	}
}

std::shared_ptr<DialogueResponse> get_npc_response(InteractionParameters parameters) {
	parameters.classification = get_classification(parameters.dialogue);
	std::cout << "Classification: " << get_dialogue_type_name(parameters.classification) << std::endl;

	Disposition disposition = get_disposition(parameters.appearance, parameters.knowledge, parameters.personality);
	parameters.response_direction = get_response_direction(disposition, parameters.personality, parameters.classification);
	parameters.knowledge = update_knowledge_from_interaction(parameters.knowledge, parameters.response_direction);
	std::shared_ptr<DialogueResponse> response = get_response(parameters);

	//TODO: update actions/figure out if leave conversation
	return response;

	/*
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
    case Compliment:
        return get_npc_compliment_response(dialogue, appearance, personality, knowledge);
    default:
        std::cout << "Unknown classification." << std::endl;
        return "Wut? I can't understand you.";
    }
	*/
}

std::shared_ptr<DialogueResponse> get_default_response(std::string dialogue) {
    Appearance appearance = Appearance();
    Personality personality = Personality();
    Knowledge knowledge = Knowledge();

	//Grumpy philosopher grandpa
	personality.intelligence = 123;
	personality.maturity = 0.6;
	personality.traits.Enthusiasm = -0.2;
	personality.traits.Assertiveness = 0.6;
	personality.traits.Volatility = 0.1;
	personality.traits.Withdrawal = 0.3;
	personality.traits.Politeness = -0.4;
	personality.traits.Compassion = 0.1;
	personality.traits.Orderliness = 0.5;
	personality.traits.Industriousness = 0.2;
	personality.traits.Openness = 0.1;
	personality.traits.Intellect = 0.6;
	personality.morals.authority_subversion = 0.2;
	personality.morals.fairness_cheating = 0.4;
	personality.morals.care_harm = 0.1;
	personality.morals.liberty_oppression = 0.5;
	personality.morals.loyalty_betrayal = 0.6;
	personality.morals.sanctity_degradation = 0.4;

	InteractionParameters parameters = InteractionParameters();
	parameters.dialogue = dialogue;
	parameters.appearance = appearance;
	parameters.personality = personality;
	parameters.knowledge = knowledge;
    return get_npc_response(parameters);
	//return "I don't like your girlfriend.";
}

std::string get_default_response_synchronous(std::string dialogue)
{
	std::shared_ptr<DialogueResponse> response = get_default_response(dialogue);
	while (!response->get_is_complete())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	return response->get_response();
}

std::string get_npc_response_synchronous(InteractionParameters parameters)
{
	std::shared_ptr<DialogueResponse> response = get_npc_response(parameters);
	while (!response->get_is_complete())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	return response->get_response();
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






#include "pch.h"
#include <iostream>

#include "test_util.h"
#include "../SocialEngine/social.h"
#include "../SocialEngine/global_knowledge.h"

namespace slow 
{
    TEST(Response, test_default_greet_gets_greet) {
        Personality p = get_default_personality();
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        //Default personality.
		InteractionParameters ip;
		ip.personality = p;
		ip.appearance = a;
		ip.knowledge = k;
		ip.dialogue = "Hello";

        auto response = get_npc_response_synchronous(ip);
        auto desired = "Hello.";
        auto undesired = "I'm busy";
        std::cout << "Response: " << response << std::endl;
        std::cout << "Desired: " << desired << std::endl;
        std::cout << "Undesired: " << undesired << std::endl;
        auto similarity = GlobalKnowledge::get_instance().get_similarity(response, desired);
        auto un_similarity = GlobalKnowledge::get_instance().get_similarity(response, undesired);
        ASSERT_GT(similarity, un_similarity);
    }

    TEST(Response, test_psychopath_greet_gets_insult) {
        Personality p = get_default_personality();
        Appearance a = get_default_appearance();
        Knowledge k = get_default_knowledge();

        //Psychopath sadist personality.
        p.morals.care_harm = -1.0;
        p.traits.Compassion = -1.0;
        p.traits.Politeness = -1.0;
		InteractionParameters ip;
		ip.personality = p;
		ip.appearance = a;
		ip.knowledge = k;
		ip.dialogue = "Hello";

		auto response = get_npc_response_synchronous(ip);
        auto desired = "Go away doofus, you're too blind to realize that you're wasting my time. Now begone.";
        auto undesired = "Hello, I like your hat";
        std::cout << "Response: " << response << std::endl;
        std::cout << "Desired: " << desired << std::endl;
        std::cout << "Undesired: " << undesired << std::endl;
        auto similarity = GlobalKnowledge::get_instance().get_similarity(response, desired);
        auto un_similarity = GlobalKnowledge::get_instance().get_similarity(response, undesired);
        ASSERT_GT(similarity, un_similarity);
    }

	TEST(Response, test_compliment_gets_thank) {
		Personality p = get_default_personality();
		Appearance a = get_default_appearance();
		Knowledge k = get_default_knowledge();

		//Default personality.
		InteractionParameters ip;
		ip.personality = p;
		ip.appearance = a;
		ip.knowledge = k;
		ip.dialogue = "I like your hat";

		auto response = get_npc_response_synchronous(ip);
		auto desired = "Thank you.";
		auto undesired = "I'm busy";
		std::cout << "Response: " << response << std::endl;
		std::cout << "Desired: " << desired << std::endl;
		std::cout << "Undesired: " << undesired << std::endl;
		auto similarity = GlobalKnowledge::get_instance().get_similarity(response, desired);
		auto un_similarity = GlobalKnowledge::get_instance().get_similarity(response, undesired);
		ASSERT_GT(similarity, un_similarity);
	}

	TEST(Response, test_question_gets_accurate_answer_easy) {
		Personality p = get_default_personality();
		Appearance a = get_default_appearance();
		Knowledge k = get_default_knowledge();

		//Inquisitive personality.
		p.morals.care_harm = 0.3;
		p.traits.Openness = 0.5;
		p.traits.Politeness = 0.4;
		p.traits.Compassion = 0.2;
		InteractionParameters ip;
		ip.personality = p;
		ip.appearance = a;
		ip.knowledge = k;
		ip.dialogue = "Where can I go to pray?";

		auto response = get_npc_response_synchronous(ip);
		auto desired = " St. Elmo's Chapel.";
		auto undesired = "I don't know, but I hope you can find some serentiy. There's probably a church nearby. God be with you!";
		std::cout << "Response: " << response << std::endl;
		std::cout << "Desired: " << desired << std::endl;
		std::cout << "Undesired: " << undesired << std::endl;
		auto similarity = GlobalKnowledge::get_instance().get_similarity(response, desired);
		auto un_similarity = GlobalKnowledge::get_instance().get_similarity(response, undesired);
		ASSERT_GT(similarity, un_similarity);
	}

	TEST(Response, test_question_gets_accurate_answer_medium) {
		Personality p = get_default_personality();
		Appearance a = get_default_appearance();
		Knowledge k = get_default_knowledge();

		//Inquisitive personality.
		p.morals.care_harm = 0.3;
		p.traits.Openness = 0.5;
		p.traits.Politeness = 0.4;
		p.traits.Compassion = 0.2;
		InteractionParameters ip;
		ip.personality = p;
		ip.appearance = a;
		ip.knowledge = k;
		ip.dialogue = "Where can I find a tincture to heal my headache?";

		auto response = get_npc_response_synchronous(ip);
		auto desired = "The people at Bellamy's Apothecary should be able to take care of you. They've got all sorts of useful things there.";
		auto undesired = "I don't know, but I hope you find something. Good luck!";
		std::cout << "Response: " << response << std::endl;
		std::cout << "Desired: " << desired << std::endl;
		std::cout << "Undesired: " << undesired << std::endl;
		
		auto similarity = GlobalKnowledge::get_instance().get_similarity(response, desired);
		auto un_similarity = GlobalKnowledge::get_instance().get_similarity(response, undesired);
		ASSERT_GT(similarity, un_similarity);
	}

	TEST(Response, test_question_gets_accurate_answer_hard) {
		Personality p = get_default_personality();
		Appearance a = get_default_appearance();
		Knowledge k = get_default_knowledge();

		//Inquisitive personality.
		p.morals.care_harm = 0.3;
		p.traits.Openness = 0.5;
		p.traits.Politeness = 0.4;
		p.traits.Compassion = 0.2;
		InteractionParameters ip;
		ip.personality = p;
		ip.appearance = a;
		ip.knowledge = k;
		ip.dialogue = "I need to go on a long journey, where's the best place to get a ride outta here?";

		auto response = get_npc_response_synchronous(ip);
		auto desired = "Take a steam train from Whitfield Station.";
		auto undesired = "I don't know, but I hope you can get to where you're going. Good luck!";
		std::cout << "Response: " << response << std::endl;
		std::cout << "Desired: " << desired << std::endl;
		std::cout << "Undesired: " << undesired << std::endl;
		auto similarity = GlobalKnowledge::get_instance().get_similarity(response, desired);
		auto un_similarity = GlobalKnowledge::get_instance().get_similarity(response, undesired);
		ASSERT_GT(similarity, un_similarity);
	}


}

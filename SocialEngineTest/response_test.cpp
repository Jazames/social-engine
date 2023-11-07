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

        auto response = get_npc_response("Hello", a, p, k);
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

        auto response = get_npc_response("Hello", a, p, k);
        auto desired = "Go away doofus, you're too blind to realize that you're wasting my time. Now begone.";
        auto undesired = "Hello, I like your hat";
        std::cout << "Response: " << response << std::endl;
        std::cout << "Desired: " << desired << std::endl;
        std::cout << "Undesired: " << undesired << std::endl;
        auto similarity = GlobalKnowledge::get_instance().get_similarity(response, desired);
        auto un_similarity = GlobalKnowledge::get_instance().get_similarity(response, undesired);
        ASSERT_GT(similarity, un_similarity);
    }
}
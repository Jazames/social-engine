#include "pch.h"
#include "../SocialEngine/global_knowledge.h"

class GlobalKnowledgeTest : public ::testing::Test {
protected:
    // This method will be called before each test
    void SetUp() override {
        // Setup code...
    }

    // This method will be called after each test
    void TearDown() override {
        //Clear out the mappings between each test so they don't interfere with each other.
        GlobalKnowledge::get_instance().reset();
    }
};

TEST_F(GlobalKnowledgeTest, SingletonTest) {
    auto& gk1 = GlobalKnowledge::get_instance();
    auto& gk2 = GlobalKnowledge::get_instance();
    ASSERT_EQ(&gk1, &gk2);  // Ensures that both references point to the same instance
}

TEST_F(GlobalKnowledgeTest, AddKnowledgeTest) {
    auto& gk = GlobalKnowledge::get_instance();
    int index = gk.add_knowledge("hello");
    ASSERT_EQ(index, 0);  // First item should be at index 0
    index = gk.add_knowledge("world");
    ASSERT_EQ(index, 1);  // Second item should be at index 1
    index = gk.add_knowledge("hello");
    ASSERT_EQ(index, 0);  // "hello" already exists, should return index 0
}

TEST_F(GlobalKnowledgeTest, GetKnowledgeTest) {
    auto& gk = GlobalKnowledge::get_instance();
    int index = gk.add_knowledge("hello");
    ASSERT_EQ(gk.get_knowledge(index), "hello");
    ASSERT_EQ(gk.get_knowledge(index + 1), "");  // Index out of range, should return empty string
}

TEST_F(GlobalKnowledgeTest, GetClosestItemsSimpleTest) {
    auto& gk = GlobalKnowledge::get_instance();
    gk.add_knowledge("hello");
    gk.add_knowledge("world");
    auto closest_items = gk.get_closest_items("hello", 2);
    ASSERT_EQ(closest_items.size(), 2);
    ASSERT_EQ(closest_items[0], "hello");  // "hello" should be the closest item to itself
}

TEST_F(GlobalKnowledgeTest, GetClosestItemsMildTest) {
    auto& gk = GlobalKnowledge::get_instance();
    gk.add_knowledge("cat");
    gk.add_knowledge("dog");
    auto closest_items = gk.get_closest_items("kitten", 1);
    ASSERT_EQ(closest_items.size(), 1);
    ASSERT_EQ(closest_items[0], "cat");  // "cat" should be the closest item to kitten
}

TEST_F(GlobalKnowledgeTest, GetClosestItemsMidTest) {
    auto& gk = GlobalKnowledge::get_instance();
    gk.add_knowledge("spoon");
    gk.add_knowledge("knife");
    gk.add_knowledge("fork");
    gk.add_knowledge("napkin");
    gk.add_knowledge("glass");
    gk.add_knowledge("chopsticks");
    auto closest_items = gk.get_closest_items("spork", 3);
    ASSERT_EQ(closest_items.size(), 3);
    ASSERT_TRUE(std::find(closest_items.begin(), closest_items.end(), "spoon") != closest_items.end());  // "spoon" should be one of the closest item to spork
    ASSERT_TRUE(std::find(closest_items.begin(), closest_items.end(), "fork") != closest_items.end());  // "fork" should be one of the closest item to spork
}

TEST_F(GlobalKnowledgeTest, GetClosestItemsQuestionTest) {
    auto& gk = GlobalKnowledge::get_instance();
    gk.add_knowledge("The cat is on the roof.");
    gk.add_knowledge("The dog barks at the moon.");
    gk.add_knowledge("Birds fly in the sky.");

    auto closest_items = gk.get_closest_items("Tell me what animal is on the roof.", 1);

    ASSERT_EQ(closest_items.size(), 1);
    ASSERT_EQ(closest_items[0], "The cat is on the roof.");  // The sentence about the cat should be the closest
}

TEST_F(GlobalKnowledgeTest, GetClosestItemsFantasyDescriptionTest) {
    auto& gk = GlobalKnowledge::get_instance();

    auto blacksmith = "Nestled within the heart of the bustling city of Eldoria, The Whispering Forge is a place where reality intertwines with the arcane. Renowned throughout the land for producing enchanted weaponry, this ancient forge hums with a quiet power that whispers through the cobblestone streets. Master Blacksmith Alden, a burly man with a gentle smile, is the heart and soul of this mystical place. Alongside him, a cadre of skilled apprentices and enchanters tirelessly work, their hammers singing upon anvils from dawn till dusk. The enchanted embers from the forge dance in the night, casting a warm, inviting glow that welcomes warriors and sorcerers alike, all in search of legendary arms to aid them in their quests.";
    auto library = "Perched upon the cliffs overlooking the serene Moonlit Cove, The Alabaster Alcove serves as a sanctuary of knowledge and healing. With its ivory towers and silvered domes, this majestic edifice is a haven for scholars, healers, and those seeking solace from the chaos of the outside world. Lady Seraphine, the revered healer and scholar, presides over this sanctuary, her wisdom a guiding light to all who seek aid. The halls of the Alabaster Alcove resonate with soft chants and the rustle of ancient tomes, as acolytes and scholars delve into the mysteries of healing arts and ancient lore. The tranquil gardens and the calming rush of the nearby sea provide a peaceful backdrop to the relentless pursuit of knowledge and the gentle art of healing practiced within these hallowed halls.";
    auto tavern = "Tucked away in the shady corners of the merchant district in Valthoria, The Veiled Tavern is a place of shadows and whispered secrets. By day, it’s a modest establishment serving ales and warm meals to the weary travelers and local merchants. By night, however, it transforms into a hub of clandestine meetings and illicit deals. The tavern keeper, a mysterious figure known as Shade, is rumored to be a broker of information, and his establishment is often frequented by spies, rogues, and sometimes even nobility seeking forbidden knowledge. The flickering candlelight casts long shadows on the worn wooden tables as plots are hatched and secrets traded in hushed tones, under the ever-watchful eye of Shade and the veil of the night.";
    gk.add_knowledge(blacksmith);
    gk.add_knowledge(library);
    gk.add_knowledge(tavern);

    auto closest_items = gk.get_closest_items("Tell me where I can find a peaceful place to learn.", 1);
    ASSERT_EQ(closest_items.size(), 1);
    ASSERT_EQ(closest_items[0], library);

    closest_items = gk.get_closest_items("Where can I get a warm meal?", 1);
    ASSERT_EQ(closest_items.size(), 1);
    ASSERT_EQ(closest_items[0], tavern);

    closest_items = gk.get_closest_items("Who makes the best swords?", 1);
    ASSERT_EQ(closest_items.size(), 1);
    ASSERT_EQ(closest_items[0], blacksmith);

    closest_items = gk.get_closest_items("Who is Lady Seraphine?", 1);
    ASSERT_EQ(closest_items.size(), 1);
    ASSERT_EQ(closest_items[0], library);

    closest_items = gk.get_closest_items("I'd like to learn a secret.", 1);
    ASSERT_EQ(closest_items.size(), 1);
    ASSERT_EQ(closest_items[0], tavern);

    closest_items = gk.get_closest_items("Who is the local blacksmith?", 1);
    ASSERT_EQ(closest_items.size(), 1);
    ASSERT_EQ(closest_items[0], blacksmith);
}



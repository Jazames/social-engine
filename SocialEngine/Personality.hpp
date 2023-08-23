#pragma once

#include<vector>

struct Big5Traits
{
	//These range from 0.0 to 1.0, with 1.0 indicating a high value in this item.
	double Enthusiasm		= 0.5; //Extraversion
	double Assertiveness	= 0.5; //Extraversion
	double Withdrawal		= 0.5; //Neuroticism
	double Volatility		= 0.5; //Neuroticism
	double Compassion		= 0.5; //Agreeableness
	double Politeness		= 0.5; //Agreeableness
	double Industriousness	= 0.5; //Conscientiousness
	double Orderliness		= 0.5; //Conscientiousness
	double Openness			= 0.5; //Openness to Experience
	double Intellect		= 0.5; //Openness to Experience

	double Extraversion() { return (Enthusiasm + Assertiveness) / 2; }
	double Neuroticism() { return (Withdrawal + Volatility) / 2; }
	double Agreeableness() { return (Compassion + Politeness) / 2; }
	double Conscientiousness() { return (Industriousness + Orderliness) / 2; }
	double OpennessToExperience() { return (Openness + Intellect) / 2; }
};

struct MoralFoundations
{
	//These range from -1.0 to 1.0. 
	//Negative values in these traits approximates dark triad tendencies. 
	double care_harm			= 0.0; //higher is more care
	double fairness_cheating	= 0.0; //higher is more fair
	double loyalty_betrayal		= 0.0; //higher is more loyal
	double authority_subversion = 0.0; //higher is more yielding to authority
	double sanctity_degradation = 0.0; //higher is more sanctity
	double liberty_oppression	= 0.0; //higher is more liberty
};

struct Personality
{
	Big5Traits traits;
	MoralFoundations morals;
	double intelligence = 1.0; // 1 = 100 IQ. 
};



//A given task/thing will have these characteristics that put a load on each personality trait. 
struct Things
{
	double Reward; //Extraversion Composite
	double Risk; //Neuroticism Composite
	double Needfulness; //Compassion
	double ValuesMatch; //Politeness
	double Effort; //Industriousness
	double WillIncreaseOrder; //Orderliness
	double Novelty; //Openness
	double IntellectualStimulation; //Intellect
};

enum HairColor
{
	Blonde,
	SandyBlonde,
	Brown,
	DarkBrown,
	Black,
	Red,
	StrawberryRed,
	Auburn,

};

struct SkinColor
{
	double melanin_content;
};

struct Hair
{
	HairColor color;
	bool is_uniform;
	double authority_weight;
	double religious_weight;
	double foreign_weight;
	double neatness_weight;
};

struct Clothing
{
	uint64_t id;
	uint64_t set_id;
	double authority_weight;
	double religious_weight;
	double foreign_weight;
	double neatness_weight;
};

struct Appearance
{
	uint64_t face_id;
#ifdef RACISM
	SkinColor skin_color;
#endif
	Hair hair;
	std::vector<Clothing> clothes;
};



//TODO: 
/*
Need to add a goal in some fashion, so that each NPC has some goals they are trying to accomplish

Need to add Haidt's moral values to allow a NPC to determine what is good/bad

Need to add some sort of personality integration/wisdom field that contols how much NPCs can deviate from their personality to achieve their goals. 
	Perhaps a maturity level? 

Potentially need to add dark triad traits? Perhaps these would be linked to maturity, where maturity mitigates dark triad tendancies?


*/





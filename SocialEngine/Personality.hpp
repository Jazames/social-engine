#pragma once

struct PersonalityTraits
{
	double Enthusiasm = 50.0; //Extraversion
	double Assertiveness = 50.0; //Extraversion
	double Withdrawal = 50.0; //Neuroticism
	double Volatility = 50.0; //Neuroticism
	double Compassion = 50.0; //Agreeableness
	double Politeness = 50.0; //Agreeablness
	double Industriousness = 50.0; //Conscientiousness
	double Orderliness = 50.0; //Conscientiousness
	double Openness = 50.0; //Openness to Experience
	double Intellect = 50.0; //Openness to Experience
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




//TODO: 
/*
Need to add a goal in some fashion, so that each NPC has some goals they are trying to accomplish

Need to add Haidt's moral values to allow a NPC to determine what is good/bad

Need to add some sort of personality integration/wisdom field that contols how much NPCs can deviate from their personality to achieve their goals. 
	Perhaps a maturity level? 

Potentially need to add dark triad traits? Perhaps these would be linked to maturity, where maturity mitigates dark triad tendancies?


*/


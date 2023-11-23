#pragma once

#include<vector>
#include <mutex>

struct Big5Traits
{
	//These range from -1.0 to 1.0, with 1.0 indicating a high value in this item.
	//The mapping from Big 5 percentile is (Percent/100) - 0.5;
	//That is, 50th percentil gets a 0 in the trait, 0th percentile gets -1.0,
	//and 100th percentile gets a 1.0;
	double Enthusiasm		= 0.0; //Extraversion
	double Assertiveness	= 0.0; //Extraversion
	double Withdrawal		= 0.0; //Neuroticism
	double Volatility		= 0.0; //Neuroticism
	double Compassion		= 0.0; //Agreeableness
	double Politeness		= 0.0; //Agreeableness
	double Industriousness	= 0.0; //Conscientiousness
	double Orderliness		= 0.0; //Conscientiousness
	double Openness			= 0.0; //Openness to Experience
	double Intellect		= 0.0; //Openness to Experience

	double Extraversion() { return (Enthusiasm + Assertiveness) / 2; } // Average of Enthusiasm and Assertiveness
	double Neuroticism() { return (Withdrawal + Volatility) / 2; } // Average of Withdrawal and Volatility
	double Agreeableness() { return (Compassion + Politeness) / 2; } // Average of Compassion and Politeness
	double Conscientiousness() { return (Industriousness + Orderliness) / 2; } // Average of Industriousness and Orderliness
	double OpennessToExperience() { return (Openness + Intellect) / 2; } // Average of Openness and Intellect
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

enum Age
{
	Wise,
	Boomer,
	Parent,
	Adult,
	Young,
	Teen,
	Child
};

struct Personality
{
	Big5Traits traits;
	MoralFoundations morals;
	double intelligence = 1.0; // 1 = 100 IQ. 
	double maturity; // 0 is toddler, 1 is fully integrated and wise.
	Age age;
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


struct DialogueResponse {
private:
	mutable std::mutex mutex;
	std::string response;
	std::atomic<bool> is_complete = false;
	//int id;
public:
	void set_response(std::string response) {
		std::lock_guard<std::mutex> guard(mutex);
		this->response = response;
	}
	void append_response(std::string response) {
		std::lock_guard<std::mutex> guard(mutex);
		this->response += response;
	}
	std::string get_response() {
		std::lock_guard<std::mutex> guard(mutex);
		return response;
	}
	bool get_is_complete() {
		return is_complete;
	}
	void set_complete() {
		is_complete = true;
	}
	DialogueResponse(std::string initial_response) :
			response(initial_response) {}
};

//TODO: 
/*
Need to add a goal in some fashion, so that each NPC has some goals they are trying to accomplish

Need to add Haidt's moral values to allow a NPC to determine what is good/bad

Need to add some sort of personality integration/wisdom field that contols how much NPCs can deviate from their personality to achieve their goals. 
	Perhaps a maturity level? 

Potentially need to add dark triad traits? Perhaps these would be linked to maturity, where maturity mitigates dark triad tendancies?


*/





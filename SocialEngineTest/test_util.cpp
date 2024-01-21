
#include "pch.h"

#include "test_util.h"
#include "social.h"


Personality get_default_personality()
{
	Personality personality = Personality();

	personality.intelligence = 1.0;
	personality.maturity = 0.5;
	personality.morals.care_harm = 0.5;
	personality.morals.fairness_cheating = 0.5;
	personality.morals.authority_subversion = 0.25;
	personality.morals.sanctity_degradation = 0.25;
	personality.morals.loyalty_betrayal = 0.25;
	personality.morals.liberty_oppression = 0.25;

	return personality;
}

Appearance get_default_appearance()
{
	return Appearance();
}

Knowledge get_default_knowledge()
{
	std::string cobblestoneMarketSquare = "Cobblestone Market Square: In the heart of the Victorian-era English town lies Cobblestone Market Square, bustling with activity. Its intricate cobblestone paths, lined with stalls and shops, evoke a sense of old-world charm. Here, townsfolk gather to trade goods, gossip, and purchase fresh produce. The air buzzes with the chatter of merchants, housewives, and children playing among the stalls.";

	std::string hawthorneMill = "Hawthorne Mill: On the outskirts, the imposing Hawthorne Mill stands tall. Its large, smoke-belching chimneys and clattering machinery are a testament to the Industrial Revolution. The mill is a hive of activity, with workers, mostly men in soot-covered clothes, tirelessly transforming raw materials into textiles.";

	std::string ivywoodManor = "Ivywood Manor: Nestled in a quiet, leafy suburb is Ivywood Manor, an elegant estate with manicured gardens and a grand facade. This opulent home hosts lavish parties for the town’s elite, where well-dressed gentlemen and ladies mingle in its ornate ballrooms.";

	std::string stElmosChapel = "St. Elmo's Chapel: The venerable St. Elmo's Chapel, with its towering spire and stained glass windows, serves as a spiritual refuge. Parishioners, from humble workers to affluent merchants, congregate here for solace, prayer, and community events.";

	std::string blackthornTavern = "Blackthorn Tavern: The Blackthorn Tavern is a popular gathering spot for the town’s working class. Its dimly lit, wood-paneled interior provides a cozy escape. Laborers and artisans frequent the tavern to unwind with ale and lively music after a hard day’s work.";

	std::string rosemaryPark = "Rosemary Park: Rosemary Park, a serene green space with flower beds and meandering paths, offers a respite from the bustling town. Nannies with prams, young couples, and elderly residents enjoying leisurely strolls are common sights here.";

	std::string theConstabulary = "The Constabulary: The town's law and order are maintained from The Constabulary, a formidable building with a stern facade. Policemen, in their distinctive uniforms, patrol the streets and keep a watchful eye on the community.";

	std::string chatterleyLibrary = "Chatterley Library: The esteemed Chatterley Library, an edifice of knowledge, stands with towering bookshelves and quiet reading rooms. Scholars, students, and avid readers frequent this temple of learning, immersed in their quest for information and education.";

	std::string whitfieldStation = "Whitfield Station: The hustle and bustle of Whitfield Station encapsulates the era's transportation revolution. Steam trains chug into the station, bringing travelers and businessmen from afar, while porters and passengers scurry about.";

	std::string bellamysApothecary = "Bellamy's Apothecary: Tucked in a narrow alley, Bellamy's Apothecary is an intriguing mix of science and superstition. The apothecary, cluttered with jars of herbs and potions, is a magnet for those seeking remedies. It’s frequented by a varied clientele, from worried mothers to curious gentlemen, seeking cures for their ailments.";


	Knowledge k = Knowledge();
	k = add_to_knowlege(cobblestoneMarketSquare, k);
	k = add_to_knowlege(hawthorneMill, k);
	k = add_to_knowlege(ivywoodManor, k);
	k = add_to_knowlege(stElmosChapel, k);
	k = add_to_knowlege(blackthornTavern, k);
	k = add_to_knowlege(rosemaryPark, k);
	k = add_to_knowlege(theConstabulary, k);
	k = add_to_knowlege(chatterleyLibrary, k);
	k = add_to_knowlege(whitfieldStation, k);
	k = add_to_knowlege(bellamysApothecary, k);

	return k;
}


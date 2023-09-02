# Social Engine
This is a C++ project meant to simulate social interactions with and between NPCs in video games. It uses the Big Five Traits personality model and Jordan Peterson's personality work, along with Jonathan Haidt's Moral Foundation's Theory to simulate moral and preferential decision making.

## Setup
In order for this to work, you must navigate to/create the directory [repo]/llamaCpp/build/visual-studio and run the command `cmake ../.. -G "Visual Studio 17 2022" -A x64 -DLLAMA_CUBLAS=ON`

## Development Plan
The end goal is for this social engine to be able to generate personality models, provide a method to easily assign social meaning to external entities, and simulate social interactions between external entities. 

### Steps
1. Create structures to hold personality and moral taste information
2. Create structrues to hold the social meaning of external entities
3. Create a the rules and logic to simlualte interactions between NPCs and social situations
4. Create system for transforming external entities into social meaning structures
5. Probably go back to step 1 and repeat the steps, refining the models and logic along the way
6. Implement this into a simple game. 
7. Go back and refine again, now that it's being used in something.

## Thoughts on making a game that could use this

### UI
A lot of NPC interaction in games is clunky and takes the player out of real-time interactions. This is because NPC interaction UX is often presented as a menu of options while time stops. It would be better to have something more fluid, such as the combat action in FPS games. So perhaps a player looks at an NPC, holds a button to pop-up a circular selection menu (like the overwatch taunts), and then the player has some options of interaction, such as a greeting, a compliment, an insult, or a few quest-related questions. 

### Meaning
Many NPCs seem flat and dull because their only meaning in the game seems to be to sell the player weapons or buy the players hoarded loot. To seem more alive, each NPC needs a goal or a purpose that they're working for, along with social connections to other NPCs. Those social connections should include some friends, some enemies, and things that the NPC knows about the world. These things will be necessary for the Social Engine to function well. 

### Game Pitch -- Playground Personal Investigator
Solve mysteries on the playground, get paid in knickacks and coins. 

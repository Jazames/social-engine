# Social Engine
This is a C++ project meant to simulate social interactions with and between NPCs in video games. It uses the Big Five Traits personality model and Jordan Peterson's personality work, along with Jonathan Haidt's Moral Foundation's Theory to simulate moral and preferential decision making.

## New Setup
Need to install vcpkg and set it up to manage packages. 

Then install gtest using `vcpkg install gtest`
Install CLBLAST using `vcpkg install clbast`

Then, when using cmake to build, need to run this command in a build folder.
`cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=/c/vcpkg/scripts/buildsystems/vcpkg.cmake`

Then, open set "run" as the startup project in visual studio, and the you can hit debut and the application will work.





### Old Setup
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

# Game Pitch -- Downton Abbey Sim
One plays as either the Butler or Housekeeper, trying to solve social and resource problems to reach a desired end. (Perhaps aim for different endings and then include stats as well, such as how many people like you at the end)

## Mechanics
- Define different sorts of tasks in the game, some generalized/templatized some specific, that can be taken by characters and will further the story or meet goals. The tasks are activated when a character makes a request that matches the task description (use sentence embeddings to compare). 
    - For example, a generalized task to give a specific message to a specific person. This task would have 3 parameters, the sender, the receiver, and the message. 
    - Another example, a specific task, would be to invite travelers for dinner. It might be specific to a unique sort of traveler, and it would kick off some other tasks for dinner preparations, etc. 
    - Then when any NPC is given the task (via the request interaction), they work towards doing that task.
    - Tasks probably need to come with priorities so an NPC knows which to work on first.
    - Will also probably need some low priority tasks for every NPC so that they're relatively busy, these tasks would be chore type stuff. Possibly bunch these tasks into roles to group them for easy assignment?
    - For this game, might make it possible for NPCs to pass tasks to servants in lower social rank?
    - The sabotage action might be to accept the task but then never do it? Perhaps some specific/special tasks have some prescribed sabotage associated.
    - Might need to modify the request interaction to account for the case when a request is made but the request doesn't match a known task.

## Story Events
- Definitely want some sort of sexual scandal as part of it. Perhaps a nobel daughter and a lady's maid having the hotts for the Groom (stable boy) and under gardener.


## Things still under consideration
- Need to make it more of a game and less of a simulator
    - Perhaps one thing to gameify would be to put some problem causers in the game that don't do their jobs? Like a footman who never delivers messages.
- How to give the NPCs their own goals and let them take actions of their own volition
- Need to come up with more social problems. Resource problems are easy


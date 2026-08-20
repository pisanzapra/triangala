# Triangala (3 to 4-Player Mancala/Mangala)
This project is a Reinforcement Learning agent that learns to play Mangala, a traditional Turkish board game of strategy, through Self-Play.
The agent is used for learning the best outcomes to help to set the rules for 3 (or more)-player version for the game. 

## 🚀 Project Goal and Development Process

### 1. The Problem: Curse of Dimensionality
In the game of Mangala, calculating the distribution of 48 stones across 14 different slots (12 pits + 2 stores) results in approximately **2.7 Trillion** possible board states. 
The initially tested classic Q-Learning model could only observe about 0.0002% of all possible states even after 500,000 training games. It was completely helpless against previously unseen board configurations, resorting to purely random moves.

### 2. The Solution: Feature-Based Approach
Since it is impossible for the agent to memorize trillions of states, the model was updated utilizing **Linear Function Approximation**. The agent now extracts 17 core features from any given board state:
* Number of stones in each of its own 6 pits
* Number of stones in each of the opponent's 6 pits
* Number of its own empty pits
* Number of the opponent's empty pits
* "Empty pit threats" against its own pits
* "Empty pit threats" against the opponent's pits

As the agent plays, it updates the **weights** of these 17 features. This allows it to make intuitive strategic deductions, such as "the opponent has stones opposite my empty pit, this is a dangerous state," even when encountering a board layout for the very first time.

## ⚙️ Setup and Execution

The project is written in the **C++17** standard and does not require any external libraries.

### Compile
You can compile the project by running the following command in your terminal:
```bash
g++ -std=c++17 -O2 -o mangala_td mangala_td.cpp


Mangala0 - takes all the stones inside the hole (kuyu). Turns are not going as they should be.

Mangala1 - Everything is wrong. Has to be corrected.

MangalaGameAuto - Game/Code plays by itself. Has a function called "makeBestMove" to make the best move. 
AI can be included to make it playable with 3-Players and when it becomes playable with 3-Players.
I have to define best rules to play with just to make Mangala playable with 3-Players. 

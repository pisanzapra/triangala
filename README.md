# Triangala: RL Agent for 3+ Player Mangala

This repository contains a C++ Reinforcement Learning agent built to scale Mangala (a traditional Turkish mancala game) from a 2-player game to a 3 or 4-player format.

Standard 2-player rules break down completely when you add a third person to the board. The ultimate goal of this project is to use a self-playing RL agent to test different mechanics and figure out a balanced, playable rule set for 3+ players.

## The State Space Explosion Issue

I initially tried using standard tabular Q-Learning, but it failed almost immediately due to the sheer size of the game's state space.

The theoretical distribution of 48 stones across 14 slots (12 pits and 2 stores) results in approximately **2.7 trillion** possible board states. Even if we filter out the physically impossible configurations based on the game's rules, the *reachable* state space is still massive [roughly **889 billion** (comparable to similar solved mancala games like Owari/Awale)].

I ran 500,000 training games, simulating about 15 million total states. This means the agent only managed to observe around **0.0016%** of the realistic board possibilities. Whenever it encountered a board it hadn't already memorized in its table, it just defaulted to random, nonsensical moves.

## The Fix: Feature-Based TD Learning

Since memorizing the board is impossible, I switched the model to Linear Function Approximation. Instead of tracking the exact board layout, the agent now extracts 26 core features from any given state:

* Stone counts in its own 6 pits
* Stone counts in the opponent's 6 pits
* Number of empty pits on its own side
* Number of empty pits on the opponent's side
* Active "empty pit threats" against its own pits
* Active "empty pit threats" against the opponent's pits

During self-play, the agent updates the **weights** of these features. This gives it a sort of intuition—it learns that leaving an empty pit opposite a loaded one is a bad idea, even if it has never seen that specific board combination before.

## Project Structure & Current State

<details>
  <summary>1) First_Implementation folder:</summary>

  * **`Mangala0.cpp`**: First 2-player prototype. This one includes fundamental logical errors in turn transitions and stone distribution.
  * **`Mangala1.cpp`**: Broken. The core logic failed in this iteration and required a complete rewrite.
  * **`Mangala2.cpp`**: Test version. Game starts with 6 stones instead of 4. Core game mechanics were (kind of) fixed.
  * **`MangalaGameAuto.c`**: The main script where the game plays itself. It uses a `makeBestMove` function to evaluate and execute the optimal move. Currently, this runs on 2-player mechanics. Once the base is completely stable, this file will be the bridge to introduce the AI for 3-player testing. The main bottleneck right now is hardcoding the custom rules needed to actually make a 3-player game function logically before the AI can train on it.
</details>

<details>
  <summary>2) Q-Learning Version folder:</summary>

  * **`mangala_ai.cpp`**: The failed tabular Q-Learning model using classic Minimax algorithm with Alpha-Beta pruning, which tries to calculate all possibilities. This is the reference code that shows that tabular memorization fails in the face of trillions of state spaces.
</details>

<details>
  <summary>3) TD-Learning folder:</summary>

  * **`v1-mangala_td.cpp`**: The feature-based TD-Learning version. Note that this iteration lacks the even-number capture (hole-doubling) rule. Against a random opponent, it achieves an 85-99% win rate. However, against a Minimax algorithm (depth 3), the win rate drops to a range of 18-55% (averaging around 28%).
  * **`v1-weights.txt`**: Weights for the v1. 
  * **`hv1-mangala_td.cpp`**: Hybrid model that combines AlphaGo-style tree search with Value (V) function estimation at leaf nodes, expanding the feature set to 26 to capture complex pit interactions.
  * **`hv1-weights.txt`**: Weights for the first hybrid model, hv1. 
  * **`hv2-mangala_td.cpp`**: The most advanced 2-player agent. Implements Curriculum Learning by dynamically decaying the learning rate (alpha) and exploration rate (epsilon), while progressively increasing search depth as the agent matures.
  * **`hv2-weights.txt`**: Weights for the second agent model. 
</details>

<details>
  <summary>4) TRIANGALA:</summary>

  * **`triangala_base.cpp `**: The very first mathematical translation of the board to 3 players (21 array positions). It solves the "opposite pit" geometry and tests basic interactive variants.
  * **`triangala_v2_rule.cpp`**: A transitional update that refines and fixes the logic of the experimental capture variants.
  * **`triangala_v3_ai.cpp`**: Integrates a 3-Player Max^n Search Algorithm into the experimental rule laboratory. Uses a "Paranoid Heuristic" where the AI specifically targets the leading player rather than averaging opponents' scores.
  * **`triangala_final.cpp`**: Final version. Fully optimized 3+ Player Mangala (Triangala as I call) engine.
</details>

## 📊 Statistical Analysis & Rule Validation

Using the `triangala_benchmark.cpp` headless engine, thousands of games were simulated to mathematically prove the optimal rules for a 3-player board:

1. **Fairness & First-Mover Advantage:**
   * *Classic "Opposite Pit" Rule:* Heavily biased. Player 1 wins ~35.1% of random games ($p < 0.001$ via Chi-Square).
   * *The New "Even-Number (Çiftleme)" Rule:* Perfectly balanced. Random play yields a perfectly even 33.3% win distribution ($p = 0.287$). However, under AI control, Player 3 gains a dynamic late-turn tactical advantage, proving that intelligence exploits symmetrical rules differently than randomness.
2. **Elimination Bias:** 
   * Under standard "Permanent Elimination", the first player to empty their pits is mathematically doomed, finishing in last place 57.03% of the time with an average rank of 2.45/3.0. Triangala introduces the **"Alive Pit (Canlı Kuyu)"** mechanic, fixing this bias and extending tactical gameplay by ~32 moves without causing infinite stalemates.
3. **AI Depth vs. Seat Advantage:**
   * Branching factor costs scale exponentially: Depth 4 (~0.05 ms) -> Depth 6 (~0.74 ms) -> Depth 8 (~11.54 ms).
   * Depth 8 dominates shallower agents with a 46.11% win rate, though analysis reveals this is a compound result of pure intelligence (~6%) and the natural geometric advantage of the 3rd seat (~40%). 

## 🚀 Build and Run

The project uses **CMake** to support seamless cross-platform compilation (Windows, Linux, macOS) and CI/CD via GitHub Actions. Written in plain C++17 with no external dependencies.

### Option A: Using CMake (Recommended)
```bash
# 1. Create a build directory and configure the project
mkdir build && cd build
cmake ..

# 2. Compile all executables simultaneously
make

To compile:
```bash
g++ -std=c++17 -O2 -o Triangala_Engine 4_Triangala/triangala_final.cpp

# Triangala (3+ Player Mancala)

This repository contains a C++ Reinforcement Learning agent built to scale Mangala (a traditional Turkish mancala game) from a 2-player game to a 3 or 4-player format.

Standard 2-player rules break down completely when you add a third person to the board. The ultimate goal of this project is to use a self-playing RL agent to test different mechanics and figure out a balanced, playable rule set for 3+ players.

## The Roadblock: State Space Explosion

I initially tried using standard tabular Q-Learning, but it failed almost immediately due to the sheer size of the game's state space.

The theoretical distribution of 48 stones across 14 slots (12 pits and 2 stores) results in approximately **2.7 trillion** possible board states. Even if we filter out the physically impossible configurations based on the game's rules, the *reachable* state space is still massive—roughly **889 billion** (comparable to similar solved mancala games like Awari).

I ran 500,000 training games, simulating about 15 million total states. This means the agent only managed to observe around **0.0016%** of the realistic board possibilities. Whenever it encountered a board it hadn't already memorized in its table, it just defaulted to random, nonsensical moves.

## The Fix: Feature-Based TD Learning

Since memorizing the board is impossible, I switched the model to **Linear Function Approximation**. Instead of tracking the exact board layout, the agent now extracts 26 core features from any given state:

* Stone counts in its own 6 pits
* Stone counts in the opponent's 6 pits
* Number of empty pits on its own side
* Number of empty pits on the opponent's side
* Active "empty pit threats" against its own pits
* Active "empty pit threats" against the opponent's pits

During self-play, the agent updates the **weights** of these features. This gives it a sort of intuition—it learns that leaving an empty pit opposite a loaded one is a bad idea, even if it has never seen that specific board combination before.

## Project Structure & Current State

The codebase is currently split into a few iterations as I work out the logic. Here is where things stand:

* **`Mangala0`**: A basic implementation, but the turn logic is bugged. Instead of properly distributing stones one by one, it just takes all the stones inside a hole. Needs fixing.
* **`Mangala1`**: Currently broken. The core logic failed in this iteration and requires a complete rewrite.
* **`MangalaGameAuto`**: The main script where the game plays itself. It uses a `makeBestMove` function to evaluate and execute the optimal move. Currently, this runs on 2-player mechanics. Once the base is completely stable, this file will be the bridge to introduce the AI for 3-player testing. The main bottleneck right now is hardcoding the custom rules needed to actually make a 3-player game function logically before the AI can train on it.

## Build and Run

Written in plain C++17. There are no external dependencies or libraries.

To compile:

```bash
g++ -std=c++17 -O2 -o mangala_td mangala_td.cpp

```

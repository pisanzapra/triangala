  * **`v1-mangala_td.cpp`**: The feature-based TD-Learning version. Note that this iteration lacks the even-number capture (hole-doubling) rule. Against a random opponent, it achieves an 85-99% win rate. However, against a Minimax algorithm (depth 3), the win rate drops to a range of 18-55% (averaging around 28%).
  * **`v1-weights.txt`**: Weights for the v1. 
  * **`hv1-mangala_td.cpp`**: Hybrid model that combines AlphaGo-style tree search with Value (V) function estimation at leaf nodes, expanding the feature set to 26 to capture complex pit interactions.
  * **`hv1-weights.txt`**: Weights for the first hybrid model, hv1. 
  * **`hv2-mangala_td.cpp`**: The most advanced 2-player agent. Implements Curriculum Learning by dynamically decaying the learning rate (alpha) and exploration rate (epsilon), while progressively increasing search depth as the agent matures.
  * **`hv2-weights.txt`**: Weights for the second agent model. 

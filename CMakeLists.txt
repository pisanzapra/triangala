cmake_minimum_required(VERSION 3.10)
project(Triangala VERSION 1.0)

# C++17 standardını ve -O2 optimizasyonunu zorunlu kıl
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

# 1. 2 Kişilik TD-Learning Ajanı (Görseldeki klasör adına göre)
add_executable(td_agent 3_TD-Version/hv2-mangala_td.cpp)

# 2. 3 Kişilik İnteraktif Triangala Motoru (Görseldeki klasör adına göre)
add_executable(triangala 4_Triangala/triangala_final.cpp)

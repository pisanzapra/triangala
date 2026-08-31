cmake_minimum_required(VERSION 3.10)
project(Triangala VERSION 1.0)

# C++17 standardını ve -O2 optimizasyonunu zorunlu kıl
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

# 1. 2 Kişilik TD-Learning Ajanı (3_tdlearn klasörü)
add_executable(td_agent 3_tdlearn/hv3_hybrid_promax_td.cpp)

# 2. 3 Kişilik İnteraktif Triangala Motoru (4_triangala klasörü)
add_executable(triangala 4_triangala/triangala_3p_engine.cpp)

# 3. Sessiz Benchmark Analiz Motoru (4_triangala klasörü)
add_executable(benchmark 4_triangala/triangala_benchmark.cpp)

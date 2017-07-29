# Makefile
#
# Author: Marlos C. Machado

#ALE := ../Arcade-Learning-Environment
ALE := $(HOME)/code/ale-agcd-interface

# Set this to 1 to enable SDL and display_screen
USE_SDL     := 0

ifeq ($(CLION_EXE_DIR),)
	CLION_EXE_DIR := .
endif

# -O3 Optimize code (urns on all optimizations specified by -O2 and also turns on the -finline-functions, -funswitch-loops, -fpredictive-commoning, -fgcse-after-reload, -ftree-loop-vectorize, -ftree-slp-vectorize, -fvect-cost-model, -ftree-partial-pre and -fipa-cp-clone options).
# -D__USE_SDL Ensures we can use SDL to see the game screen
# -D_GNU_SOURCE=1 means the compiler will use the GNU standard of compilation, the superset of all other standards under GNU C libraries.
# -D_REENTRANT causes the compiler to use thread safe (i.e. re-entrant) versions of several functions in the C library.
FLAGS := -O3 -I$(ALE)/src -L$(ALE) -lale -lz -g
CXX := g++ -std=c++11
OUT_FILE := $(CLION_EXE_DIR)/learnerBlobTime
# Search for library 'ale' and library 'z' when linking.
LDFLAGS := -lale -lz -lm

ifeq ($(strip $(USE_SDL)), 1)
  FLAGS +=  -D__USE_SDL `sdl-config --cflags --libs`
  LDFLAGS += -lSDL -lSDL_gfx -lSDL_image
endif

all: $(OUT_FILE) ql

$(OUT_FILE): bin/mainBlobTime.o bin/Mathematics.o bin/Parameters.o bin/Timer.o bin/Features.o bin/Background.o bin/BlobTimeFeatures.o bin/RLLearner.o bin/SarsaLearner.o
	$(CXX) $(FLAGS) bin/mainBlobTime.o bin/Mathematics.o bin/Timer.o bin/Parameters.o bin/Features.o bin/Background.o bin/BlobTimeFeatures.o bin/RLLearner.o bin/SarsaLearner.o -o learnerBlobTime

ql: bin/Mathematics.o bin/Parameters.o bin/Timer.o bin/Features.o bin/Background.o bin/BlobTimeFeatures.o bin/RLLearner.o bin/QLearner.o bin/ql.o
	$(CXX) $(FLAGS) bin/Mathematics.o bin/Timer.o bin/Parameters.o bin/Features.o bin/Background.o bin/BlobTimeFeatures.o bin/RLLearner.o bin/QLearner.o bin/ql.o -o ql

bin/mainBlobTime.o: mainBlobTime.cpp
	$(CXX) $(FLAGS) -c mainBlobTime.cpp -o bin/mainBlobTime.o

bin/ql.o: ql.cpp
	$(CXX) $(FLAGS) -c ql.cpp -o bin/ql.o

bin/Mathematics.o: common/Mathematics.cpp
	$(CXX) $(FLAGS) -c common/Mathematics.cpp -o bin/Mathematics.o

bin/Timer.o: common/Timer.cpp
	$(CXX) $(FLAGS) -c common/Timer.cpp -o bin/Timer.o

bin/Parameters.o: common/Parameters.cpp
	$(CXX) $(FLAGS) -c common/Parameters.cpp -o bin/Parameters.o

bin/Features.o: features/Features.cpp
	$(CXX) $(FLAGS) -c features/Features.cpp -o bin/Features.o

bin/Background.o: features/Background.cpp
	$(CXX) $(FLAGS) -c features/Background.cpp -o bin/Background.o
	
bin/BlobTimeFeatures.o: features/BlobTimeFeatures.cpp
	$(CXX) $(FLAGS) -c features/BlobTimeFeatures.cpp -o bin/BlobTimeFeatures.o	
	
bin/RLLearner.o: agents/rl/RLLearner.cpp
	$(CXX) $(FLAGS) -c agents/rl/RLLearner.cpp -o bin/RLLearner.o

bin/SarsaLearner.o: agents/rl/sarsa/SarsaLearner.cpp
	$(CXX) $(FLAGS) -c agents/rl/sarsa/SarsaLearner.cpp -o bin/SarsaLearner.o
		
bin/QLearner.o: agents/rl/q/QLearner.cpp
	$(CXX) $(FLAGS) -c agents/rl/q/QLearner.cpp -o bin/QLearner.o

clean:
	rm -rf ${OUT_FILE} bin/*.o ql
	rm -f learner*


#This command needs to be executed in a osX before running the code:
#export DYLD_LIBRARY_PATH="${DYLD_LIBRARY_PATH}:../lib/ale_0_4"

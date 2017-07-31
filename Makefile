.PHONY=clean

ifeq ($(ALE),)
	ALE := ../Arcade-Learning-Environment
endif

ifeq ($(EXE_DIR),)
	EXE_DIR := .
endif

CXXFLAGS := -O3 -march=native -pipe -fPIC -pie -std=c++11 -I$(ALE)/src $(CXXFLAGS)
LDFLAGS := -lz -lm -lale -L$(ALE) $(LDFLAGS)

OBJDIR := obj
SARSA_OBJ := $(OBJDIR)/SarsaLearner.o
RL_OBJ := $(OBJDIR)/RLLearner.o
Q_OBJ := $(OBJDIR)/QLearner.o
FEATURE_OBJS := $(addprefix $(OBJDIR)/,Features.o Background.o BlobTimeFeatures.o)
COMMON_OBJS := $(addprefix $(OBJDIR)/,Parameters.o Mathematics.o Timer.o)
SEQ_OBJS := $(addprefix $(OBJDIR)/,SeqSarsaLearner.o)
#OBJS := $(SARSA_OBJ) $(RL_OBJ) $(Q_OBJ) $(FEATURE_OBJS) $(COMMON_OBJS) $(SEQ_OBJS)
OBJS := $(RL_OBJ) $(FEATURE_OBJS) $(COMMON_OBJS)

SARSA_DIR := agents/rl/sarsa
Q_DIR := agents/rl/q
RL_DIR := agents/rl
FEATURE_DIR := features
COMMON_DIR := common
TARGETS := sarsa ql

all: $(OBJS) $(TARGETS)

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o : src/%.cpp
	$(CXX) $(CXXFLAGS) $< -c -o $@

sarsa: $(OBJS) $(SARSA_OBJ) src/mainBlobTime.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) $(SARSA_OBJ) src/mainBlobTime.cpp -o sarsa

seqsarsa: $(OBJS) $(SEQ_OBJS) src/seqsarsa.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) $(SEQ_OBJS) src/seqsarsa.cpp -o seqsarsa

ql: $(OBJS) $(Q_OBJ) src/ql.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) $(Q_OBJ) src/ql.cpp -o ql

clean:
	rm -rf $(TARGETS) $(OBJDIR)

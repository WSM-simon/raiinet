CXX = g++
CXXFLAGS = -std=c++20 -fmodules-ts -Wall -g
TARGET = raiinet

# System headers to precompile
HEADERS = iostream sstream fstream string vector algorithm memory utility

# Utility modules
UTIL_SRCS = src/util/position.cc src/util/direction.cc src/util/linkType.cc \
            src/util/resultHeader.cc src/util/firewallinfo.cc src/util/abilityKind.cc \
            src/util/placeResult.cc src/util/abiliityParams.cc src/util/abilityResult.cc \
            src/util/linkSetup.cc src/util/playerSetup.cc

# Base modules (order matters for dependencies)
BASE_SRCS = src/link.cc src/util/moveresult.cc src/util/serverport.cc src/cell.cc \
            src/observer.cc src/observer-impl.cc src/subject.cc src/subject-impl.cc \
            src/board.cc src/board-impl.cc src/player.cc

# Ability modules
ABILITY_SRCS = src/abilities/ability.cc src/abilities/ability-impl.cc \
               src/abilities/downloadability.cc src/abilities/downloadability-impl.cc \
               src/abilities/firewallability.cc src/abilities/firewallability-impl.cc \
               src/abilities/scanability.cc src/abilities/scanability-impl.cc \
               src/abilities/polarizeability.cc src/abilities/polarizeability-impl.cc \
               src/abilities/linkboostability.cc src/abilities/linkboostability-impl.cc \
               src/util/abilityFactory.cc

# Game and UI modules
GAME_SRCS = src/game.cc src/game-impl.cc src/textdisplay.cc src/controller.cc src/main.cc

ALL_SRCS = $(UTIL_SRCS) $(BASE_SRCS) $(ABILITY_SRCS) $(GAME_SRCS)
OBJS = $(notdir $(ALL_SRCS:.cc=.o))

all: headers $(TARGET)

headers:
	$(CXX) $(CXXFLAGS) -c -x c++-system-header $(HEADERS)

$(TARGET): compile_modules
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

compile_modules:
	@echo "Compiling utility modules..."
	$(CXX) $(CXXFLAGS) -c $(UTIL_SRCS)
	@echo "Compiling base modules..."
	$(CXX) $(CXXFLAGS) -c $(BASE_SRCS)
	@echo "Compiling ability modules..."
	$(CXX) $(CXXFLAGS) -c $(ABILITY_SRCS)
	@echo "Compiling game and UI modules..."
	$(CXX) $(CXXFLAGS) -c $(GAME_SRCS)

clean:
	rm -f *.o $(TARGET)
	rm -rf gcm.cache

.PHONY: all headers compile_modules clean

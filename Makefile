all:
	g++ -std=c++20 -fmodules-ts -c -x c++-system-header iostream sstream fstream string vector algorithm
	g++ -std=c++20 -fmodules-ts -Wall -g -c src/observer.cc
	g++ -std=c++20 -fmodules-ts -Wall -g -c src/observer-impl.cc
	g++ -std=c++20 -fmodules-ts -Wall -g -c src/subject.cc
	g++ -std=c++20 -fmodules-ts -Wall -g -c src/subject-impl.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/display.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/textdisplay.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/textdisplay-impl.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/graphicsdisplay.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/graphicsdisplay-impl.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/controller.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/controller-impl.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/game.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/game-impl.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/board.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/board-impl.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/cell.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/cell-impl.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/player.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/player-impl.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/link.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/link-impl.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/abilities/ability.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/abilities/ability-impl.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/abilities/downloadability.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/abilities/downloadability-impl.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/abilities/firewallability.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/abilities/firewallability-impl.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/abilities/linkboostability.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/abilities/linkboostability-impl.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/abilities/polarizeability.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/abilities/polarizeability-impl.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/abilities/scanability.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/abilities/scanability-impl.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/util/abiliityParams.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/util/direction.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/util/firewallinfo.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/util/linkType.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/util/moveresult.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/util/position.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/util/serverport.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g -c src/main.cc
	# g++ -std=c++20 -fmodules-ts -Wall -g observer.o observer-impl.o subject.o subject-impl.o display.o textdisplay.o textdisplay-impl.o graphicsdisplay.o graphicsdisplay-impl.o controller.o controller-impl.o game.o game-impl.o board.o board-impl.o cell.o cell-impl.o player.o player-impl.o link.o link-impl.o ability.o ability-impl.o downloadability.o downloadability-impl.o firewallability.o firewallability-impl.o linkboostability.o linkboostability-impl.o polarizeability.o polarizeability-impl.o scanability.o scanability-impl.o abiliityParams.o direction.o firewallinfo.o linkType.o moveresult.o position.o serverport.o main.o -o raiinet
	g++ -std=c++20 -fmodules-ts -Wall -g observer.o observer-impl.o subject.o subject-impl.o -o raiinet

clean:
	rm -f *.o src/abilities/*.o src/util/*.o raiinet
	rm -rf gcm.cache

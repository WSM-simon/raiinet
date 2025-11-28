export module util.playerSetup;

import <string>;
import <vector>;
import util.linkSetup;
import util.abilityKind;

export struct PlayerSetup {
    std::vector<LinkSetup> linkSetups;          // initial positions
    std::vector<AbilityKind> abilities;    // chosen abilities in order
};

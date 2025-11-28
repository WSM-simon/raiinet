module ability;

import <string>;
import <utility>;
import game;
import player;
import util.abilityParams;

using std::string;

Ability::Ability(string name, char code) : name(name), code(code), used(false) {}

Ability::~Ability() {}

string Ability::getName() const {
    return name;
}

char Ability::getCode() const {
    return code;
}

bool Ability::isUsed() const {
    return used;
}

void Ability::markUsed() {
    used = true;
}

export module util.linkSetup;

import util.position;
import util.linkType;

export struct LinkSetup {
    char id; // 'a'..'h'
    LinkType type;
    int strength; // 1..4
    Position pos;
};
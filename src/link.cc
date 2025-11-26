export module link;

import util.position;
import util.linkType;

export class Link {
    int ownerId_;              // which player owns this link (0..numPlayers-1)
    char id_;                  // label, e.g. 'a'..'h'
    LinkType type_;            // Data or Virus
    int strength_;             // 1..4

    Position pos_{-1, -1};     // current board position; (-1,-1) = not on board
    bool onBoard_ = false;     // is this link currently placed on the board?

    bool revealedToOpponent_ = false; // does the opponent know this link's true type/strength?
    bool boosted_ = false;            // LinkBoost ability applied?

public:
    // ---- constructor ----
    Link(int ownerId, char id, LinkType type, int strength)
        : ownerId_{ownerId}
        , id_{id}
        , type_{type}
        , strength_{strength} {}

    // ---- identity ----
    int getOwnerId() { return ownerId_; }
    char getId() { return id_; }

    // ---- type / strength ----
    LinkType getType() { return type_; }

    bool isData() { return type_ == LinkType::Data; }
    bool isVirus() { return type_ == LinkType::Virus; }

    int getStrength() { return strength_; }

    // ---- board position / presence ----
    Position getPosition() { return pos_; }
    void setPosition(const Position& p) { pos_ = p; }

    bool isOnBoard() { return onBoard_; }
    void setOnBoard(bool v) { onBoard_ = v; }

    // ---- visibility to opponent ----
    bool isRevealedToOpponent() { return revealedToOpponent_; }
    void setRevealedToOpponent(bool v) { revealedToOpponent_ = v; }

    // ---- LinkBoost status ----
    bool isBoosted() { return boosted_; }
    void setBoosted(bool v) { boosted_ = v; }
};
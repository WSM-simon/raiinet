export module board;

import cell;
import link;
import util.position;
import util.direction;
import util.serverPort;
import util.moveResult;
import util.placeResult;
import util.firewallInfo;

export class Board {
private:
    static constexpr int ROWS_ = 8;
    static constexpr int COLS_ = 8;

    Cell grid_[ROWS_][COLS_];
    ServerPort serverPorts_[2];      // 2 players: 0 and 1

    Position computeTargetPosition(Position start, Direction dir);
    void resolveBattle(Link* attacker, Link* defender, Position targetPos, MoveResult& result);
    void applyFirewallEffects(Link* link, Position pos, MoveResult& result);
    bool isInServerLoc(int playerId, Position pos);

public:
    Board();

    int getRows() { return ROWS_; }
    int getCols() { return COLS_; }

    bool isInside(Position pos);
    Cell& getCell(int r, int c);

    PlaceResult placeLink(Link* link, Position pos);
    void removeLink(Link* link);

    MoveResult moveLink(Link* link, Direction dir);

    void placeFirewall(Position pos, int ownerId);
    void clearFirewall(Position pos);

    void reset();
};
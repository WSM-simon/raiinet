module board;

import <vector>;

import cell;
import link;
import util.position;
import util.direction;
import util.serverport;
import util.moveresult;
import util.placeResult;
import util.firewallinfo;

using std::vector;

Board::Board() {
    serverPorts_[0].ownerId = 0;
    serverPorts_[0].loc = { {7,3}, {7,4} };

    serverPorts_[1].ownerId = 1;
    serverPorts_[1].loc = { {0,3}, {0,4} };
}

int Board::getRows() { return ROWS_; }
int Board::getCols() { return COLS_; }

bool Board::isInside(Position pos) {
    return pos.row >= 0 && pos.row < ROWS_ &&
           pos.col >= 0 && pos.col < COLS_;
}

Cell& Board::getCell(int r, int c) {
    return grid_[r][c];
}

PlaceResult Bord::placeLink(Link* link, Position pos) {
    PlaceResult res;

    if (!link) {
        res.header.success = false;
        res.header.msg = "No link selected.";
        return res;
    };
    if (link->isOnBoard()) {
        res.header.success = false;
        res.header.msg = "This link is already on the board.";
        return res;
    }
    if (!isInside(pos)) {
        res.header.success = false;
        res.header.msg = "This position is outside the board.";
        return res;
    }

    Cell& cell = getCell(pos.row, pos.col);
    if (cell.hasLink()) {
        res.header.success = false;
        res.header.msg = "This cell is already occupied.";
        return res;
    }
    
    cell.setLink(link);
    link->setPosition(pos);
    link->setOnBoard(true);

    return res;
}

Position Board::computeTargetPosition(Position start, Direction dir) {
    Position to = start;
    switch (dir) {
        case Direction::Up:    --to.row; break;
        case Direction::Down:  ++to.row; break;
        case Direction::Left:  --to.col; break;
        case Direction::Right: ++to.col; break;
    }
    return to;
}

bool Board::isInServerLoc(int playerId, Position pos) {
    if (playerId < 0 || playerId > 1) return false;
    for (auto p : serverPorts_[playerId].loc) {
        if (p.row == pos.row && p.col == pos.col) return true;
    }
    return false;
}

MoveResult Board::moveLink(Link* link, Direction dir) {
    MoveResult res;
    res.attacker = link;

    if (!link) {
        res.header.success = false;
        res.header.msg = "No link selected.";
        return res;
    }

    if (!link->isOnBoard()) {
        res.header.success = false;
        res.header.msg = "This link is not on the board.";
        return res;
    }

    // move the link to the target position
    Position cur = link->getPosition();
    int steps = link->isBoosted() ? 2 : 1;
    for (int i = 0; i < steps; i++) {
        cur = computeTargetPosition(cur, dir);
        if (!isInside(cur)) {
            res.header.success = false;
            res.header.msg = "Move goes outside the board.";
            return res;
        }
    }
    Position to = cur;

    int ownerId = link->getOwnerId();
    int enemyId = 1 - ownerId;

    // own server ports -> illegal
    if (isInServerLoc(ownerId, to)) {
        res.header.success = false;
        res.header.msg = "You cannot move onto your own server port.";
        return res;
    }

    // enemy server ports -> legal, but immediate download
    if (isInServerLoc(enemyId, to)) {
        Cell& fromCell = getCell(from.row, from.col);
        if (fromCell.getLink() == link) {
            fromCell.clearLink();
        }
        link->setOnBoard(false);

        res.moved = true;
        res.downloaded = true;
        res.downloadedLink = link;

        return res;
    }

    // check if the target cell is occupied, if so, resolve the battle
    Cell& fromCell = getCell(from.row, from.col);
    Cell& toCell = getCell(to.row, to.col);

    Link* defender = toCell.getLink();
    if (defender) {
        // battle logic
        resolveBattle(link, defender, to, res);
    } else {
        // simple move logic
        fromCell.clearLink();
        toCell.setLink(link);
        link->setPosition(to);
        res.moved = true;
    }
    // TODO : resolve 1. resolveBattle 2. applyFirewallEffects 3. apply endline download effects

    res.header.success = true;
    res.header.msg = "";
    return res;
}



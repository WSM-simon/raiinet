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

PlaceResult Board::placeLink(Link* link, Position pos) {
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

    res.header.success = true;
    res.header.msg = "";

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

void Board::resolveBattle(Link* attacker, Link* defender, Position targetPos, MoveResult& result) {
    if (!attacker || !defender) return;

    Position from = attacker->getPosition();
    Cell& fromCell = getCell(from.row, from.col);
    Cell& toCell   = getCell(targetPos.row, targetPos.col);

    int a = attacker->getStrength();
    int d = defender->getStrength();

    result.battle = true;
    result.defender = defender;

    if (a >= d) {
        // attacker wins (tie goes to attacker)
        fromCell.clearLink();
        toCell.setLink(attacker);
        attacker->setPosition(targetPos);

        defender->setOnBoard(false);

        result.moved = true;
        result.downloaded = true;
        result.downloadedLink = defender;
    } else {
        // defender wins
        fromCell.clearLink();
        attacker->setOnBoard(false);

        result.moved = true;
        result.downloaded = true;
        result.downloadedLink = attacker;
    }
}

void Board::applyFirewallEffects(Link* link, Position pos, MoveResult& result) {
    if (!link) return;
    if (!isInside(pos)) return;

    Cell& cell = getCell(pos.row, pos.col);
    if (!cell.hasFirewall()) return;

    FirewallInfo* fw = cell.getFirewall();
    if (!fw) return;

    if (fw->ownerId == link->getOwnerId()) return;

    result.firewallTriggered = true;
    result.firewall = fw;

    // reveal the link to the opponent
    link->setRevealedToOpponent(true);

    if (link->isVirus()) {
        cell.clearLink();
        link->setOnBoard(false);
        result.downloaded = true;
        result.downloadedLink = link;
    }
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

    int ownerId = link->getOwnerId();
    int enemyId = 1 - ownerId;

    // move the link to the target position
    Position from = link->getPosition();
    Position cur = from;
    int steps = link->isBoosted() ? 2 : 1;
    for (int i = 0; i < steps; i++) {
        cur = computeTargetPosition(cur, dir);
        // check if the target position is inside the board
        if (ownerId == 0 && cur.row == -1 || ownerId == 1 && cur.row == 8) {
            res.moved = true;
            res.downloaded = true;
            res.downloadedLink = link;
            fromCell.clearLink()
            link->setOnBoard(false);
            return res;
        }
            
        if (!isInside(cur)) {
            res.header.success = false;
            res.header.msg = "Move goes outside the board.";
            return res;
        }
    }
    Position to = cur;

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
    applyFirewallEffects(link, link->getPosition(), res);

    res.header.success = true;
    res.header.msg = "";
    return res;
}

void Board::placeFirewall(Position pos, int ownerId) {
    if (!isInside(pos)) return;
    Cell& cell = getCell(pos.row, pos.col);
    cell.setFirewall(ownerId);
}

void Board::clearFirewall(Position pos) {
    if (!isInside(pos)) return;
    Cell& cell = getCell(pos.row, pos.col);
    cell.clearFirewall();
}

void Board::reset() {
    for (int r = 0; r < ROWS_; r++) {
        for (int c = 0; c < COLS_; c++) {
            Cell& cell = getCell(r, c);
            cell.clearLink();
            cell.clearFirewall();
        }
    }
}

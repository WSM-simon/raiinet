module firewallability;

import ability;
import board;
import player;
import cell;
import util.abilityParams;
import util.abilityResult;


FirewallAbility::FirewallAbility() : Ability("Firewall", 'F') {}

AbilityResult FirewallAbility::apply(Board& board, Player& user, Player& opponent,
                    AbilityParams& userParams, 
                    AbilityParams& opponentParams) {

    (void)opponent;
    (void)opponentParams; // Don't need these for firewallability

    AbilityResult res;

    if (!board.isInside(userParams.targetPos)) {
        res.header.success = false;
        res.header.msg = "This position is outside the board.";
        return res;
    }

    Cell& cell = board.getCell(userParams.targetPos.row, userParams.targetPos.col);

    if (cell.hasFirewall()) {
        res.header.success = false;
        res.header.msg = "This position already has a firewall.";
        return res;
    }

    if (cell.hasLink()) {
        res.header.success = false;
        res.header.msg = "This position currently has a link.";
        return res;
    }

    board.placeFirewall(userParams.targetPos, user.getId());

    markUsed();
    res.used = true;
    return res;
}
module headshotability;

import ability;
import board;
import player;
import link;
import util.abilityParams;
import util.abilityResult;

HeadshotAbility::HeadshotAbility() : Ability("Headshot", 'H') {}

AbilityResult HeadshotAbility::apply(Board& board, Player& user, Player& opponent,
                                      AbilityParams& userParams,
                                      AbilityParams& opponentParams) {
    AbilityResult res;

    Link* attacker = user.getLinkById(userParams.targetLinkId);
    Link* defender = opponent.getLinkById(opponentParams.targetLinkId);

    if (!attacker || !defender) {
        res.header.success = false;
        res.header.msg = "No such link for this player.";
        return res;
    }

    if (!attacker->isOnBoard() || !defender->isOnBoard()) {
        res.header.success = false;
        res.header.msg = "Link already downloaded.";
        return res;
    }

    if (attacker->getStrength() >= defender->getStrength()) {
        defender->setRevealedToOpponent(true);
        board.removeLink(defender);
        if (defender->isData()) {
            user.incrementDownloadedData();
        } else {
            user.incrementDownloadedViruses();
        }
    } else {
        defender->setRevealedToOpponent(true);
    }

    markUsed();
    res.used = true;
    res.header.success = true;
    res.header.msg = "";
    return res;
}
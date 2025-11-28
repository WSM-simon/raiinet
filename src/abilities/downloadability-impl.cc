module downloadability;

import ability;
import game;
import player;
import link;
import util.abilityParams;
import util.abilityResult;


DownloadAbility::DownloadAbility() : Ability("Download", 'D') {}

AbilityResult DownloadAbility::apply(Game& game, Player& user, Player& opponent,
                    AbilityParams& userParams, 
                    AbilityParams& opponentParams) {

    (void)userParams; // Don't need this for downloadability

    AbilityResult res;

    Link* opponentLink = opponent.getLinkById(opponentParams.targetLinkId);

    Board& board = game.getBoard();

    if (!opponentLink) {
        res.header.success = false;
        res.header.msg = "No such link for this player.";
        return res;
    }

    if (!opponentLink->isOnBoard()) {
        res.header.success = false;
        res.header.msg = "Link already downloaded.";
        return res;
    }

    opponentLink->setRevealedToOpponent(true);

    if (opponentLink->isData()) {
        user.incrementDownloadedData();
    } else {
        user.incrementDownloadedViruses();
    }

    opponentLink->setRevealedToOpponent(true);
    board.removeLink(opponentLink);
    markUsed();
    res.used = true;
    return res;
}
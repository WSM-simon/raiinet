module game;

import <vector>;
import <string>;
import <memory>;

import board;
import player;
import link;
import util.direction;
import util.abilityParams;
import util.moveResult;
import util.abilityResult;
import util.linkType;
import util.abilityKind;
import ability;
import downloadability;
import firewallability;
import scanability;
import polarizeability;
import linkboostability;
import abilityFactory;

using std::vector;
using std::unique_ptr;
using std::move;
using std::make_unique;

/**
 * Game Constructor
 *
 * Initializes a new game with 2 players, their links and abilities.
 * NOTE: This does NOT place links on the board yet.
 * Call Game::setupDefaultLinks() or a custom setup method from Controller.
 */
Game::Game(bool useGraphics)
    : board_{}
    , players_{}
    , currentPlayer_{0}
    , winnerId_{-1}
    , gameOver_{false}
    , useGraphics_{useGraphics} {}


void Game::initializeGame(PlayerSetup& p0, PlayerSetup& p1) {
    vector<Link> links0;
    vector<Link> links1;
    vector<unique_ptr<Ability>> abilities0;
    vector<unique_ptr<Ability>> abilities1;

    links0.reserve(p0.linkSetups.size());
    links1.reserve(p1.linkSetups.size());
    abilities0.reserve(p0.abilities.size());
    abilities1.reserve(p1.abilities.size());

    // ---- build links for player 0 ----
    for (LinkSetup& ls : p0.linkSetups) {
        links0.emplace_back(0, ls.id, ls.type, ls.strength);
    }

    // ---- build abilities for player 0 ----
    for (AbilityKind& ak : p0.abilities) {
        auto ability = makeAbility(ak);
        if (ability) abilities0.push_back(move(ability));
    }

    // ---- build links for player 1 ----
    for (LinkSetup& ls : p1.linkSetups) {
        links1.emplace_back(1, ls.id, ls.type, ls.strength);
    }

    // ---- build abilities for player 1 ----
    for (AbilityKind& ak : p1.abilities) {
        auto ability = makeAbility(ak);
        if (ability) abilities1.push_back(move(ability));
    }

    players_[0] = Player(0, move(links0), move(abilities0));
    players_[1] = Player(1, move(links1), move(abilities1));

    // ---- place links on board according to positions ----
    // now we need the actual Link* to place; so we loop again over players' links
    for (auto& ls : p0.linkSetups) {
        Link* link = players_[0].getLinkById(ls.id);
        if (link) {
            board_.placeLink(link, ls.pos);
        }
    }

    for (auto& ls : p1.linkSetups) {
        Link* link = players_[1].getLinkById(ls.id);
        if (link) {
            board_.placeLink(link, ls.pos);
        }
    }

    notifyObservers();
}

/**
 * Move a link on the board.
 *
 * 1. Finds the link belonging to current player
 * 2. Calls board_.moveLink to perform movement/battle/firewall/etc.
 * 3. Uses handleMoveResult to update players (downloads)
 * 4. Checks win condition
 * 5. Notifies observers
 */
MoveResult Game::moveLink(char linkId, Direction dir) {
    MoveResult result;

    if (isGameOver()) {
        result.header.success = false;
        result.header.msg = "Game is over.";
        return result;
    }

    Player& currentPlayer = getCurrentPlayer();

    // Find link in current player's links
    Link* link = nullptr;
    for (auto& ln : currentPlayer.getLinks()) {
        if (ln.getId() == linkId) {
            link = &ln;
            break;
        }
    }

    if (!link || link->getOwnerId() != currentPlayer_) {
        result.header.success = false;
        result.header.msg = "Invalid link.";
        return result;
    }

    // Delegate actual move to Board
    result = board_.moveLink(link, dir);

    // Update player download counts based on MoveResult
    handleMoveResult(result);

    // check win state after applying downloads
    checkWinCondition();

    // redraw displays
    notifyObservers();

    return result;
}

/**
 * Use an ability card.
 *
 * 1. Gets the ability from current player
 * 2. Checks used flag
 * 3. Calls ability->apply(...)
 * 4. If success and used, mark ability used
 * 5. If ability caused download(s), AbilityResult should reflect that
 * 6. Check win condition, notify observers
 */
AbilityResult Game::useAbility(int abilityIndex, AbilityParams& params) {
    AbilityResult result;

    if (isGameOver()) {
        result.header.success = false;
        result.header.msg = "Game is over.";
        return result;
    }

    Player& currentPlayer = getCurrentPlayer();
    Player& opponent = players_[1 - currentPlayer_];

    Ability* ability = currentPlayer.getAbility(abilityIndex);
    if (!ability) {
        result.header.success = false;
        result.header.msg = "Invalid ability index.";
        return result;
    }

    if (ability->isUsed()) {
        result.header.success = false;
        result.header.msg = "Ability already used.";
        return result;
    }

    // Some abilities need separate params for user/opponent; you can adapt as needed.
    AbilityParams userParams = params;
    AbilityParams opponentParams = params;

    result = ability->apply(*this, currentPlayer, opponent, userParams, opponentParams);

    if (result.header.success && result.used) {
        ability->markUsed();
    }

    // If ability downloaded links, AbilityResult should carry them;
    // you can optionally call handleDownload on them here if needed.

    checkWinCondition();
    notifyObservers();

    return result;
}

/**
 * End current player's turn, switch to next.
 */
void Game::endTurn() {
    if (isGameOver()) return;

    switchPlayer();
    checkWinCondition();
    notifyObservers();
}

/**
 * Reset game state (board + players + winner flag).
 *
 * NOTE: players_ reset is minimal here; if Player has a reset() method,
 * you should call it instead of just leaving links/abilities as-is.
 */
void Game::reset() {
    board_.reset();

    // If Player has reset(), call it here.
    // for (int i = 0; i < 2; ++i) {
    //     players_[i].reset();
    // }

    currentPlayer_ = 0;
    winnerId_ = -1;
    gameOver_ = false;

    notifyObservers();
}

/**
 * Is the game over?
 */
bool Game::isGameOver() {
    return gameOver_ || winnerId_ != -1;
}

/**
 * Get winner id, or -1 if no winner yet.
 */
int Game::getWinnerId() {
    return winnerId_;
}

/**
 * Handle a single downloaded link:
 * - Figure out which player downloaded (opponent of owner)
 * - Increase correct counter (data / virus)
 * - DO NOT touch board here (Board already removed link).
 */
void Game::handleDownload(Link& link) {
    int ownerId = link.getOwnerId();
    int downloaderId = (ownerId == 0) ? 1 : 0;
    Player& downloader = players_[downloaderId];

    if (link.isData()) {
        downloader.incrementDownloadedData();
    } else if (link.isVirus()) {
        downloader.incrementDownloadedViruses();
    }

    // Board::moveLink or abilities already updated the board and link.onBoard.
    // We only track score/state here.
}

/**
 * Process MoveResult:
 * - If move is illegal, do nothing
 * - If something was downloaded, call handleDownload
 * - Battle/firewall effects already applied by Board
 */
void Game::handleMoveResult(MoveResult& result) {
    if (!result.header.success) {
        // illegal move: do nothing (Controller will print msg)
        return;
    }

    if (result.downloaded && result.downloadedLink) {
        handleDownload(*result.downloadedLink);
    }

    // result.battle / result.firewallTriggered can be used for logging/FX if you want
}

/**
 * Switch current player 0 ↔ 1.
 */
void Game::switchPlayer() {
    currentPlayer_ = (currentPlayer_ == 0) ? 1 : 0;
}

/**
 * Check win condition:
 * - A player wins by downloading 4 data links and < 4 viruses
 * - A player loses (opponent wins) if they get 4 viruses and < 4 data
 * - If both >= 4 (weird), treat as loss for that player
 */
void Game::checkWinCondition() {
    for (int i = 0; i < 2; ++i) {
        int dataCount = players_[i].getDownloadedData();
        int virusCount = players_[i].getDownloadedViruses();

        // win: 4+ data, fewer than 4 virus
        if (dataCount >= 4 && virusCount < 4) {
            winnerId_ = i;
            gameOver_ = true;
            return;
        }

        // lose: 4+ virus, fewer than 4 data
        if (virusCount >= 4 && dataCount < 4) {
            winnerId_ = (i == 0) ? 1 : 0;
            gameOver_ = true;
            return;
        }

        // both >= 4: treat as loss for i
        if (dataCount >= 4 && virusCount >= 4) {
            winnerId_ = (i == 0) ? 1 : 0;
            gameOver_ = true;
            return;
        }
    }

    winnerId_ = -1;
    gameOver_ = false;
}
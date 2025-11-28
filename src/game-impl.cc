module game;

import <vector>;
import <string>;

import board;
import player;
import link;
import util.direction;
import util.abilityParams;
import util.moveResult;
import util.linkType;
import ability;

/**
 * Game Constructor
 * 
 * Initializes a new game with 2 players.
 * Note: You may need to modify this to properly initialize players
 * with their links and abilities based on your game setup.
 */
Game::Game(int numPlayers) {
    // Initialize players
    // TODO: You'll need to create the initial links and abilities for each player
    // For now, creating empty players - adjust based on your initialization logic
    std::vector<Link> player0Links;  // Initialize with player 0's links
    std::vector<std::unique_ptr<Ability>> player0Abilities;  // Initialize with abilities
    
    std::vector<Link> player1Links;  // Initialize with player 1's links
    std::vector<std::unique_ptr<Ability>> player1Abilities;  // Initialize with abilities
    
    players_[0] = Player(0, "Player 1", std::move(player0Links), std::move(player0Abilities));
    players_[1] = Player(1, "Player 2", std::move(player1Links), std::move(player1Abilities));
    
    currentPlayer_ = 0;
    winnerId_ = std::nullopt;
}

/**
 * Move a link on the board
 * 
 * This is the main entry point for moving a link. It:
 * 1. Gets the link from the current player
 * 2. Calls board.moveLink() to perform the actual move
 * 3. Handles the result (downloads, battles, etc.)
 * 4. Notifies observers of state changes
 */
void Game::moveLink(char linkId, Direction dir) {
    // Don't allow moves if game is over
    if (isGameOver()) {
        return;
    }
    
    // Get the current player
    Player& currentPlayer = getCurrentPlayer();
    
    // Find the link in the current player's links
    // Note: getLinkById returns Link&, but we need Link* for board.moveLink()
    Link* link = nullptr;
    for (auto& ln : currentPlayer.getLinks()) {
        if (ln.getId() == linkId) {
            link = &ln;
            break;
        }
    }
    
    // Check if link was found and belongs to current player
    if (!link || link->getOwnerId() != currentPlayer_.getId()) {
        // Invalid link - could notify error here
        return;
    }
    
    // Perform the move on the board
    MoveResult result = board_.moveLink(link, dir);
    
    // Handle the result (downloads, battles, etc.)
    handleMoveResult(result);
    
    // Notify observers of state changes
    notifyObservers();
}

/**
 * Use an ability card
 * 
 * Allows the current player to use one of their ability cards.
 * The ability is executed with the provided parameters.
 */
void Game::useAbility(int abilityIndex, const AbilityParams& params) {
    // Don't allow ability use if game is over
    if (isGameOver()) {
        return;
    }
    
    Player& currentPlayer = getCurrentPlayer();
    
    // Get the ability
    Ability* ability = currentPlayer.getAbility(abilityIndex);
    if (!ability) {
        // Invalid ability index
        return;
    }
    
    // Use the ability
    // Note: You'll need to implement the Ability interface's use() method
    // ability->use(*this, params);
    
    // Notify observers
    notifyObservers();
}

/**
 * End the current player's turn
 * 
 * Switches to the next player and checks win conditions.
 * This is called after a player completes their actions.
 */
void Game::endTurn() {
    // Don't allow turn switching if game is over
    if (isGameOver()) {
        return;
    }
    
    // Switch to the next player
    switchPlayer();
    
    // Check if anyone has won
    checkWinCondition();
    
    // Notify observers
    notifyObservers();
}

/**
 * Reset the game to initial state
 * 
 * Clears the board, resets players, and starts a new game.
 */
void Game::reset() {
    // Reset the board
    board_.reset();
    
    // Reset players
    // TODO: Reinitialize players with their initial links and abilities
    // For now, just reset download counts
    for (auto& player : players_) {
        // You may need methods to reset player state
        // player.reset();
    }
    
    // Reset game state
    currentPlayer_ = 0;
    winnerId_ = std::nullopt;
    
    // Notify observers
    notifyObservers();
}

/**
 * Handle a download event
 * 
 * Called when a link crosses the opponent's endline.
 * This method:
 * 1. Determines which player downloaded (opponent of link owner)
 * 2. Increments the appropriate download counter (data or virus)
 * 3. Removes the link from the board
 * 4. Checks win condition
 */
void Game::handleDownload(Link& link) {
    // Determine which player downloaded (opponent of link owner)
    int linkOwnerId = link.getOwnerId();
    int downloaderId = (linkOwnerId == 0) ? 1 : 0;
    
    Player& downloader = players_[downloaderId];
    
    // Increment the appropriate download counter
    if (link.isData()) {
        downloader.incrementDownloadedData();
    } else if (link.isVirus()) {
        downloader.incrementDownloadedViruses();
    }
    
    // Remove the link from the board
    link.setOnBoard(false);
    
    // Check win condition after download
    checkWinCondition();
}

/**
 * Process the result of a move operation
 * 
 * This method handles all the consequences of a move:
 * - Downloads (calls handleDownload)
 * - Battles (if needed - you may need to implement battle logic)
 * - Firewall triggers (if needed)
 * - Notifies observers
 */
void Game::handleMoveResult(MoveResult& result) {
    // Check if move was successful
    if (!result.header.success) {
        // Move failed - could log error message
        // result.header.msg contains the error message
        return;
    }
    
    // Handle download if link crossed endline
    if (result.downloaded && result.downloadedLink) {
        handleDownload(*result.downloadedLink);
    }
    
    // Handle battle if one occurred
    if (result.battle) {
        // TODO: Implement battle logic
        // This might involve comparing link strengths, removing defeated links, etc.
    }
    
    // Handle firewall trigger if one occurred
    if (result.firewallTriggered && result.firewall) {
        // TODO: Implement firewall logic
        // This might involve revealing link info, blocking movement, etc.
    }
}

/**
 * Switch to the next player
 * 
 * Toggles currentPlayer_ between 0 and 1.
 */
void Game::switchPlayer() {
    currentPlayer_ = (currentPlayer_ == 0) ? 1 : 0;
}

/**
 * Check if any player has won
 * 
 * Sets winnerId_ if a win condition is met.
 * 
 * Win conditions:
 * - Player downloads 4 data pieces (wins)
 * - Player downloads 4 viruses (loses - opponent wins)
 */
void Game::checkWinCondition() {
    // Check each player's victory status
    for (int i = 0; i < 2; ++i) {
        int status = players_[i].victoryStatus();
        
        if (status == 1) {
            // Player i has won (4 data, < 4 viruses)
            winnerId_ = i;
            return;
        } else if (status == -1) {
            // Player i has lost (4 viruses, < 4 data)
            // Opponent wins
            winnerId_ = (i == 0) ? 1 : 0;
            return;
        }
        // status == 0 means still playing
        // status == 999 is an error state (both 4 data and 4 viruses - shouldn't happen)
    }
    
    // No winner yet
    winnerId_ = std::nullopt;
}


export module controller;

import <iostream>;
import <fstream>;
import <sstream>;
import <string>;

import game;
import textdisplay;
import util.direction;
import util.abilityParams;
import util.moveResult;
import util.abilityResult;

using std::istream;
using std::ostream;
using std::ifstream;
using std::istringstream;
using std::string;

export class Controller {
    Game& game_;
    TextDisplay& display_;
    istream& in_;
    ostream& out_;

    Direction parseDirection(const string& dir) {
        if (dir == "up") return Direction::Up;
        if (dir == "down") return Direction::Down;
        if (dir == "left") return Direction::Left;
        if (dir == "right") return Direction::Right;
        return Direction::Up;  // default
    }

    bool isValidDirection(const string& dir) {
        return dir == "up" || dir == "down" || dir == "left" || dir == "right";
    }

    char toLower(char c) {
        if (c >= 'A' && c <= 'Z') return c - 'A' + 'a';
        return c;
    }

    char toUpper(char c) {
        if (c >= 'a' && c <= 'z') return c - 'a' + 'A';
        return c;
    }

    // Flip direction for Player 2 (only when extraFeatures enabled, since board is flipped 180°)
    Direction adjustDirection(Direction dir, int playerId) {
        // Only flip if extraFeatures is enabled AND it's Player 2
        if (!game_.hasExtraFeatures() || playerId == 1) {
            return dir;  // No flip
        }
        // Player 2 with extraFeatures: flip directions (board is rotated 180°)
        switch (dir) {
            case Direction::Up:    return Direction::Down;
            case Direction::Down:  return Direction::Up;
            case Direction::Left:  return Direction::Right;
            case Direction::Right: return Direction::Left;
            default: return dir;
        }
    }

    void processMove(const string& linkIdStr, const string& dirStr) {
        if (linkIdStr.empty() || !isValidDirection(dirStr)) {
            out_ << "Invalid move command.\n";
            return;
        }

        char linkId = linkIdStr[0];
        int currentPlayer = game_.getCurrentPlayerIndex();
        
        Direction dir = parseDirection(dirStr);
        dir = adjustDirection(dir, currentPlayer);  // Flip for Player 2
        MoveResult result = game_.moveLink(linkId, dir);

        if (!result.header.success) {
            out_ << "Move failed: " << result.header.msg << "\n";
        } else {
            // Successful move - switch turns
            display_.printSplitLine();
            game_.endTurn();
            
            if (game_.isGameOver()) {
                display_.printWinner(game_.getWinnerId());
            } 
        }
    }

    void processAbility(int abilityIdx, const string& params) {
        AbilityParams abilityParams;
        abilityParams.targetLinkId = '\0';
        abilityParams.targetPos = {-1, -1};

        // Parse ability parameters based on ability type
        // Some abilities need a link ID (Scan, Download, Polarize, LinkBoost)
        // Some need a position (Firewall)
        if (!params.empty()) {
            istringstream iss(params);
            
            // First try to parse as link ID (single character)
            if (params.length() == 1 && ((params[0] >= 'a' && params[0] <= 'h') || 
                                          (params[0] >= 'A' && params[0] <= 'H'))) {
                abilityParams.targetLinkId = params[0];
            } else {
                // Try to parse as position (row col)
                int row, col;
                if (iss >> row >> col) {
                    abilityParams.targetPos = {row, col};
                } else {
                    // Could be a link ID followed by nothing
                    abilityParams.targetLinkId = params[0];
                }
            }
        }

        AbilityResult result = game_.useAbility(abilityIdx - 1, abilityParams);  // 1-indexed to 0-indexed

        if (!result.header.success) {
            out_ << "Ability failed: " << result.header.msg << "\n";
        } else if (result.used) {
            out_ << "Ability used successfully.\n";
            
            if (game_.isGameOver()) {
                display_.printWinner(game_.getWinnerId());
            }
        }
    }

    void processSequence(const string& filename) {
        ifstream file(filename);
        if (!file) {
            out_ << "Could not open file: " << filename << "\n";
            return;
        }

        string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            processCommand(line);
            if (game_.isGameOver()) break;
        }
    }

    void processCommand(const string& line) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') return;
        
        istringstream iss(line);
        string cmd;
        iss >> cmd;

        if (cmd == "move") {
            string linkId, dir;
            iss >> linkId >> dir;
            processMove(linkId, dir);
        } else if (cmd == "abilities") {
            display_.printAbilities(game_.getCurrentPlayerIndex());
        } else if (cmd == "ability") {
            int abilityIdx;
            if (iss >> abilityIdx) {
                // Rest of the line is parameters
                string params;
                std::getline(iss, params);
                // Trim leading whitespace
                size_t start = params.find_first_not_of(" \t");
                if (start != string::npos) {
                    params = params.substr(start);
                } else {
                    params = "";
                }
                processAbility(abilityIdx, params);
            } else {
                out_ << "Invalid ability command.\n";
            }
        } else if (cmd == "board") {
            display_.printBoard(game_.getCurrentPlayerIndex());
        } else if (cmd == "sequence") {
            string filename;
            iss >> filename;
            processSequence(filename);
        } else if (cmd == "quit") {
            // Will be handled in run()
        } else if (!cmd.empty()) {
            out_ << "Unknown command: " << cmd << "\n";
        }
    }

public:
    Controller(Game& game, TextDisplay& display, istream& in, ostream& out)
        : game_{game}, display_{display}, in_{in}, out_{out} {}

    void run() {
        // Initial board display
        display_.printBoard(game_.getCurrentPlayerIndex());

        string line;
        while (std::getline(in_, line)) {
            if (line.empty()) continue;
            
            istringstream iss(line);
            string cmd;
            iss >> cmd;

            if (cmd == "quit") {
                break;
            }

            processCommand(line);

            if (game_.isGameOver()) {
                break;
            }
        }
    }
};


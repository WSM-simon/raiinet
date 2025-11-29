import <iostream>;
import <string>;
import <vector>;
import <memory>;

import game;
import textdisplay;
import graphicsdisplay;
import controller;
import util.playerSetup;
import util.linkSetup;
import util.linkType;
import util.abilityKind;
import util.position;

using std::string;
using std::vector;
using std::cerr;
using std::cout;
using std::cin;
using std::unique_ptr;
using std::make_unique;

// Check if a character is a valid ability (L, F, D, S, P, H, E, A)
bool isValidAbility(char c) {
    return c == 'L' || c == 'F' || c == 'D' || c == 'S' || c == 'P' || 
           c == 'H' || c == 'E' || c == 'A';
}

// Validate ability string:
// - Must be exactly 5 abilities
// - All characters must be valid (L, F, D, S, P)
// - Max 2 of the same ability
// Returns error message or empty string if valid
string validateAbilities(const string& str) {
    // Check length
    if (str.length() != 5) {
        return "Must choose exactly 5 abilities.";
    }
    
    // Check for unknown abilities
    for (char c : str) {
        if (!isValidAbility(c)) {
            return string("Unknown ability: ") + c + ". Valid: L, F, D, S, P, H, E, A.";
        }
    }
    
    // Check max 2 of same ability
    int counts[8] = {0, 0, 0, 0, 0, 0, 0, 0};  // L, F, D, S, P, H, E, A
    for (char c : str) {
        switch (c) {
            case 'L': counts[0]++; break;
            case 'F': counts[1]++; break;
            case 'D': counts[2]++; break;
            case 'S': counts[3]++; break;
            case 'P': counts[4]++; break;
            case 'H': counts[5]++; break;
            case 'E': counts[6]++; break;
            case 'A': counts[7]++; break;
        }
    }
    for (int i = 0; i < 8; ++i) {
        if (counts[i] > 2) {
            return "Cannot have more than 2 of the same ability.";
        }
    }
    
    return "";  // Valid
}

// Parse ability string (e.g., "LFDSP") into AbilityKind vector
// If extraFeatures is false, replaces H/E/A with defaults (L/F/D)
vector<AbilityKind> parseAbilities(const string& str, bool extraFeatures = false) {
    vector<AbilityKind> abilities;
    
    // Validate abilities
    string error = validateAbilities(str);
    string abilityStr = str;
    if (!error.empty()) {
        cerr << "Error: " << error << " Using default LFDSP.\n";
        abilityStr = "LFDSP";
    }
    
    // If extraFeatures disabled, replace H/E/A with defaults
    if (!extraFeatures) {
        if (abilityStr.find('H') != string::npos || 
            abilityStr.find('E') != string::npos || 
            abilityStr.find('A') != string::npos) {
            cerr << "Error: Additional abilities (H/E/A) require -enableExtraFeature. Replaced with defaults.\n";
            abilityStr = "LFDSP";
        }
    }
    
    for (char c : abilityStr) {
        switch (c) {
            case 'L': abilities.push_back(AbilityKind::LinkBoost); break;
            case 'F': abilities.push_back(AbilityKind::Firewall); break;
            case 'D': abilities.push_back(AbilityKind::Download); break;
            case 'S': abilities.push_back(AbilityKind::Scan); break;
            case 'P': abilities.push_back(AbilityKind::Polarize); break;
            case 'H': abilities.push_back(AbilityKind::Headshot); break;
            case 'E': abilities.push_back(AbilityKind::Exchange); break;
            case 'A': abilities.push_back(AbilityKind::Ambush); break;
            default: break;  // Already validated, won't happen
        }
    }
    return abilities;
}

// Parse link string (e.g., "V1D4V3V2D3V4D2D1") into LinkSetup vector
// Player 1 uses a-h, Player 2 uses A-H
vector<LinkSetup> parseLinks(const string& str, int playerId) {
    vector<LinkSetup> links;
    char startChar = (playerId == 1) ? 'a' : 'A';
    
    for (size_t i = 0; i + 1 < str.length(); i += 2) {
        char typeChar = str[i];
        char strengthChar = str[i + 1];
        
        LinkType type = (typeChar == 'V') ? LinkType::Virus : LinkType::Data;
        int strength = strengthChar - '0';
        char id = startChar + (links.size());
        
        links.push_back({id, type, strength, {-1, -1}});  // Position set later
    }
    
    return links;
}

// Set up default positions for links
// Player 1: row 0 and row 1 (top of board)
// Player 2: row 7 and row 6 (bottom of board)
void setDefaultPositions(vector<LinkSetup>& links, int playerId) {
    if (playerId == 1) {
        // Player 1: a,b,c at row 0 cols 0,1,2; d,e at row 1 cols 3,4; f,g,h at row 0 cols 5,6,7
        // Positions around server ports (row 0, cols 3,4)
        if (links.size() >= 8) {
            links[0].pos = {0, 0};  // a
            links[1].pos = {0, 1};  // b
            links[2].pos = {0, 2};  // c
            links[3].pos = {1, 3};  // d (below server port)
            links[4].pos = {1, 4};  // e (below server port)
            links[5].pos = {0, 5};  // f
            links[6].pos = {0, 6};  // g
            links[7].pos = {0, 7};  // h
        }
    } else {
        // Player 2: A,B,C at row 7 cols 0,1,2; D,E at row 6 cols 3,4; F,G,H at row 7 cols 5,6,7
        if (links.size() >= 8) {
            links[0].pos = {7, 0};  // A
            links[1].pos = {7, 1};  // B
            links[2].pos = {7, 2};  // C
            links[3].pos = {6, 3};  // D (above server port)
            links[4].pos = {6, 4};  // E (above server port)
            links[5].pos = {7, 5};  // F
            links[6].pos = {7, 6};  // G
            links[7].pos = {7, 7};  // H
        }
    }
}

int main(int argc, char* argv[]) {
    // Default values
    string ability1 = "LFDSP";
    string ability2 = "LFDSP";
    string link1 = "V1V2V3V4D1D2D3D4";
    string link2 = "V1V2V3V4D1D2D3D4";
    bool useGraphics = true;  // Default to graphics mode
    bool useText = true;     // Use text is default to true
    bool extraFeatures = false;
    bool enableTextFlip = false;

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-ability1" && i + 1 < argc) {
            ability1 = argv[++i];
        } else if (arg == "-ability2" && i + 1 < argc) {
            ability2 = argv[++i];
        } else if (arg == "-link1" && i + 1 < argc) {
            link1 = argv[++i];
        } else if (arg == "-link2" && i + 1 < argc) {
            link2 = argv[++i];
        } else if (arg == "-text") {
            useText = true;
            useGraphics = false;
        } else if (arg == "-enableExtraFeature") {
            extraFeatures = true;
        } else if (arg == "-enableTextFlip") {
            enableTextFlip = true;
        }
    }

    // Create game
    Game game{useGraphics, extraFeatures, enableTextFlip};

    // Set up player 1
    PlayerSetup p1;
    p1.linkSetups = parseLinks(link1, 1);
    setDefaultPositions(p1.linkSetups, 1);
    p1.abilities = parseAbilities(ability1, extraFeatures);

    // Set up player 2
    PlayerSetup p2;
    p2.linkSetups = parseLinks(link2, 2);
    setDefaultPositions(p2.linkSetups, 2);
    p2.abilities = parseAbilities(ability2, extraFeatures);

    // Initialize game
    game.initializeGame(p1, p2);

    // Create displays using smart pointers
    // TextDisplay is always needed for console messages, even in graphics mode
    unique_ptr<TextDisplay> textDisplay = make_unique<TextDisplay>(game, cout, extraFeatures);
    unique_ptr<GraphicsDisplay> graphicsDisplay;

    if (useText) {
        // Text mode - attach text display as observer
        game.attach(textDisplay.get());
    } 
    if (useGraphics) {
        // Graphics mode
        graphicsDisplay = make_unique<GraphicsDisplay>(game);
        game.attach(graphicsDisplay.get());
        // TextDisplay is still created for console messages, but not attached as observer
        
        // Draw graphics display immediately after initialization
        game.notifyObservers();
    }

    // Create and run controller (always needs TextDisplay for console output)
    Controller controller{game, *textDisplay, cin, cout};
    controller.run();

    // Smart pointers automatically clean up - no manual delete needed

    return 0;
}

export module player;

import <string>;
import <vector>;
import <memory>;

import link;                 // Link class
import util.linkType;        // if needed for construction
import util.abilityParams;   // for ability calls (optional)
import ability;              // forward declarations inside ability module

using std::string, std::vector, std::unique_ptr, std::move;

export class Player {
    int id_;                             // index of the player (0..numPlayers-1)
    string name_;                   // display name: "Player 1", etc.

    vector<Link> links_;            // owns its 8 links
    vector<unique_ptr<Ability>> abilities_;  // owns ability cards

    int downloadedData_ = 0;             // # of data downloaded
    int downloadedViruses_ = 0;          // # of viruses downloaded

public:
    // ---- constructor ----

    //TODO : needs to inplement initialAbilities
    Player(int id, string name,
           vector<Link> initialLinks,
           vector<unique_ptr<Ability>> initialAbilities)
        : id_{id}
        , name_{move(name)}
        , links_{move(initialLinks)}
        , abilities_{move(initialAbilities)}
    {} // transfer ownership to Player

    // ---- identity ----
    int getId() { return id_; }
    string& getName() { return name_; }

    // ---- links ----
    vector<Link>& getLinks() { return links_; }

    Link& getLinkById(char ch) {
        for (auto &ln : links_) {
            if (ln.getId() == ch) return &ln;
        }
        return nullptr;
    }

    // ---- abilities ----
    vector<unique_ptr<Ability>>& getAbilities() { return abilities_; }

    Ability* getAbility(int idx) {
        if (idx < 0 || idx >= (int)abilities_.size()) return nullptr;
        return abilities_[idx].get(); // return the ability pointer, not the ability object
    }

    // ---- download tracking ----
    int getDownloadedData() { return downloadedData_; }
    int getDownloadedViruses() { return downloadedViruses_; }

    void incrementDownloadedData() { ++downloadedData_; }
    void incrementDownloadedViruses() { ++downloadedViruses_; }

    // ---- victory status ----
    int victoryStatus() {
        if (downloadedData_ >= 4 && downloadedViruses_ < 4) return 1;
        if (downloadedViruses_ >= 4 && downloadedData_ < 4) return 0;
        if (downloadedData_ >= 4 && downloadedViruses_ >= 4) return -1; // if this happens, this should be an error
        return 0; // still playing
    }
};
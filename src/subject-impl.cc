module subject;

import <algorithm>;
import <vector>;
import observer;

Subject::~Subject() {}

void Subject::attach(Observer* o) {
    observers.push_back(o);
}

void Subject::detach(Observer* o) {
    auto it = std::find(observers.begin(), observers.end(), o);
    if (it != observers.end()) {
        observers.erase(it);
    }
}

void Subject::notifyObservers() {
    for (Observer* o : observers) {
        o->notify();
    }
}
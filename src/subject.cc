export module subject;

import <vector>;
import observer;

export class Subject {
protected:
    std::vector<Observer*> observers;

public:
    virtual ~Subject();

    void attach(Observer* o);
    void detach(Observer* o);
    void notifyObservers();
};
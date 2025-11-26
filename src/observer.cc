export module observer;

export class Observer {
public:
    virtual ~Observer();
    virtual void notify() = 0;
};
#ifndef PHIDGET_H
#define PHIDGET_H

class Phidget
{
public:
    ~Phidget();
    static Phidget *self();
    static bool init();

private:
    Phidget();
    static Phidget *m_instance;
};

#endif // PHIDGET_H

#ifndef GDMINIAUDIO_H
#define GDMINIAUDIO_H
#include "miniaudio.h"

#include <godot_cpp/classes/node.hpp>


namespace godot {

class GDMiniaudio : public Node {
    GDCLASS(GDMiniaudio, Node)

private:

protected:
    static void _bind_methods();

public:
    GDMiniaudio();
    ~GDMiniaudio();
    ma_result result;
    ma_engine engine;

    void _process(double delta);
    void _ready();
};

}

#endif
#ifndef GDMINIAUDIO_H
#define GDMINIAUDIO_H

#include <godot_cpp/classes/object.hpp>

namespace godot {

class GDMiniaudio : public Object {
    GDCLASS(GDMiniaudio, Object)

private:

protected:
    static void _bind_methods();

public:
    GDMiniaudio();
    ~GDMiniaudio();

    void _process(double delta);
    void _ready();
};

}

#endif
#ifndef MINIAUDIOFILE_H
#define MINIAUDIOFILE_H
#include <godot_cpp/classes/ref_counted.hpp>
namespace godot {
class MiniaudioSound : public RefCounted {
    GDCLASS(MiniaudioSound, RefCounted)
    protected:
        static void _bind_methods(){   
        }
    public:
        MiniaudioSound(){
        }
        ~MiniaudioSound(){
        }
        ma_sound sound;
};
}
#endif
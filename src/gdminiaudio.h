#ifndef GDMINIAUDIO_H
#define GDMINIAUDIO_H
#include "miniaudio.h"

#include <miniaudio_file.h>
#include <godot_cpp/classes/object.hpp>

namespace godot {

class GDMiniaudio : public Object {
    GDCLASS(GDMiniaudio, Object)
    static GDMiniaudio *singleton;

    private:

    protected:
        static void _bind_methods();

    public:
        GDMiniaudio();
        ~GDMiniaudio();
        
        static GDMiniaudio* get_singleton();
        
        ma_engine engine;
        
        bool engine_initialized;
        bool engine_started;
        
        void initialize_engine();
        void start_engine();
        void play_file(String path);
        void load_audio_file(String path, MiniaudioSound file);
        void read_from_data_source();
        void play_sound(MiniaudioSound sound);

};

}

#endif
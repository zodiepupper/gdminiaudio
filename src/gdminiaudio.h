#ifndef GDMINIAUDIO_H
#define GDMINIAUDIO_H
#include "../miniaudio/miniaudio.h"

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/ref.hpp>

using namespace godot;

class MiniaudioSound : public RefCounted {
    GDCLASS(MiniaudioSound, RefCounted);
    protected:
        static void _bind_methods(){   
        }
    public:
        MiniaudioSound(){
            sound = memnew(ma_sound);
        }
        ~MiniaudioSound(){
        }
        ma_sound *sound;
};

class GDMiniaudio : public Object {
    GDCLASS(GDMiniaudio, Object);
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
        void load_audio_file(String path, Ref<MiniaudioSound> file);
        void read_from_data_source();
        void play_sound(Ref<MiniaudioSound>  sound);
        void set_sound_position(Ref<MiniaudioSound> sound, float posx, float posy, float posz);
        Vector3 get_sound_position(Ref<MiniaudioSound> sound);
};


#endif
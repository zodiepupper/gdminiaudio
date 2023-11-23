#define MINIAUDIO_IMPLEMENTATION
#include "gdminiaudio.h"
#include <godot_cpp/core/class_db.hpp>
#include <iostream>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void GDMiniaudio::_bind_methods() {
    ClassDB::bind_static_method("GDMiniaudio", D_METHOD("get_singleton"), &GDMiniaudio::get_singleton);
    ClassDB::bind_method(D_METHOD("initialize_engine"), &GDMiniaudio::initialize_engine);
    ClassDB::bind_method(D_METHOD("start_engine"), &GDMiniaudio::start_engine);
    ClassDB::bind_method(D_METHOD("play_file", "path_to_file"), &GDMiniaudio::play_file);
    ClassDB::bind_method(D_METHOD("load_audio_file", "path_to_file"), &GDMiniaudio::load_audio_file);
    ClassDB::bind_method(D_METHOD("play_sound", "sound"), &GDMiniaudio::play_sound);
    ClassDB::bind_method(D_METHOD("set_sound_position", "sound", "X Position", "Y Position", "Z Position"), &GDMiniaudio::set_sound_position);
    ClassDB::bind_method(D_METHOD("get_sound_position", "sound"), &GDMiniaudio::get_sound_position);
    
}

// Initialize any variables here.
GDMiniaudio::GDMiniaudio() {
    singleton = this;
    engine_initialized = false;
    engine_started = false;
}

// Add your cleanup here.
GDMiniaudio::~GDMiniaudio() {
    
}

GDMiniaudio* GDMiniaudio::singleton = nullptr;

GDMiniaudio *GDMiniaudio::get_singleton() {
    return singleton;
}

/**
 * @class GDMiniaudio
 * @brief This class is responsible for handling audio in the application.
 */
void GDMiniaudio::initialize_engine() {
    if (!engine_initialized){
        ma_result result = ma_engine_init(NULL, &engine);
        switch (result){
            case MA_SUCCESS:
                printf("Audio engine initialized.\n");
                UtilityFunctions::print('test');
                engine_initialized = true;
                return;
            default:
                printf("Failed to initialize audio engine.\n");
                printf("Error code: %d\n", result);
                engine_initialized = false;
                return;
        }
    }
}

void GDMiniaudio::start_engine() {
    if (!engine_started){
        ma_result result = ma_engine_start(&engine);
        switch (result){
            case MA_SUCCESS:
                printf("Audio engine started.\n");
                engine_started = true;
                return;
            default:
                printf("Failed to start audio engine.\n");
                printf("Error code: %d\n", result);
                engine_started = false;
                return;
        }
    }
    
}

// void GDMiniaudio::read_from_data_source() {
//     ma_result result;
//     ma_uint64 framesRead;

//     result = ma_data_source_read_pcm_frames(pDataSource, pFramesOut, frameCount, &framesRead);
//     if (result != MA_SUCCESS) {
//         return result;  // Failed to read data from the data source.
//     }
// }

void GDMiniaudio::play_file(String path) {
    ma_engine_play_sound(&engine, path.utf8().get_data(), NULL);
}

void GDMiniaudio::load_audio_file(String path, Ref<MiniaudioSound> file) {
    UtilityFunctions::print("Creating miniaudiosound file");
    UtilityFunctions::print("Creating ma_sound sound");
    UtilityFunctions::print("Creating ma_result and initializing sound from file");
    ma_result result = ma_sound_init_from_file(&engine, path.utf8().get_data(), NULL, NULL, NULL, file->sound);
    if (result != MA_SUCCESS) {
        printf("Failed to load audio file.\n");
        printf("Error code: %d\n", result);
    }
    UtilityFunctions::print("adding ma_sound to minaudiosound object");
}

void GDMiniaudio::play_sound(Ref<MiniaudioSound> sound) {
    ma_sound_start(sound->sound);
}

void GDMiniaudio::set_sound_position(Ref<MiniaudioSound> sound, float posx, float posy, float posz){
    ma_sound_set_position(sound->sound, posx, posy, posz);
    UtilityFunctions::print("set position "+Vector3(posx, posy, posz));
}

Vector3 GDMiniaudio::get_sound_position(Ref<MiniaudioSound> sound){
    ma_vec3f result = ma_sound_get_position(sound->sound);
    return Vector3(result.x, result.y, result.z);
}
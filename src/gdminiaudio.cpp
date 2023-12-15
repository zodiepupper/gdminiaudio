#define MINIAUDIO_IMPLEMENTATION
#include "gdminiaudio.h"
#include <godot_cpp/core/class_db.hpp>
#include <iostream>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void GDMiniaudio::_bind_methods() {
    // setup and initialization stuffs
    // this one is specifically for quick start for the voip test setup
    ClassDB::bind_method(D_METHOD("setup_default_node_graph"), &GDMiniaudio::setup_default_node_graph);
    ClassDB::bind_method(D_METHOD("initialize_engine"), &GDMiniaudio::initialize_engine);
    ClassDB::bind_method(D_METHOD("start_engine"), &GDMiniaudio::start_engine);
    ClassDB::bind_method(D_METHOD("initialize_node_graph"), &GDMiniaudio::initialize_node_graph);
    
    // get input device
    ClassDB::bind_method(D_METHOD("get_input_device"), &GDMiniaudio::get_input_device);
    // get default remote stream (for testing)
    ClassDB::bind_method(D_METHOD("get_remote_stream"), &GDMiniaudio::get_remote_stream);
    
    // add and modify loaded sounds
    ClassDB::bind_method(D_METHOD("play_file", "path_to_file"), &GDMiniaudio::play_file);
    ClassDB::bind_method(D_METHOD("load_audio_file", "path_to_file"), &GDMiniaudio::load_audio_file);
    ClassDB::bind_method(D_METHOD("play_sound", "sound"), &GDMiniaudio::play_sound);
    ClassDB::bind_method(D_METHOD("set_sound_position", "sound", "X Position", "Y Position", "Z Position"), &GDMiniaudio::set_sound_position);
    ClassDB::bind_method(D_METHOD("get_sound_position", "sound"), &GDMiniaudio::get_sound_position);
    ClassDB::bind_method(D_METHOD("set_sound_velocity", "sound", "X Velocity", "Y Velocity", "Z Velocity"), &GDMiniaudio::set_sound_velocity);
    
    // class bindings for set/get of engine initialized bool
    ClassDB::bind_method(D_METHOD("get_engine_initialized"), &GDMiniaudio::get_engine_initialized);
    ClassDB::bind_method(D_METHOD("set_engine_initialized", "is_engine_started"), &GDMiniaudio::set_engine_initialized);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "engine_initialized"), "set_engine_initialized", "get_engine_initialized");
    
    // class bindings for set/get of enginge started bool
    ClassDB::bind_method(D_METHOD("get_engine_started"), &GDMiniaudio::get_engine_started);
    ClassDB::bind_method(D_METHOD("set_engine_started", "is_engine_started"), &GDMiniaudio::set_engine_started);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "engine_started"), "set_engine_started", "get_engine_started");
    
    // class bindings for set/get of node graph initialized bool
    ClassDB::bind_method(D_METHOD("get_node_graph_initialized"), &GDMiniaudio::get_node_graph_initialized);
    ClassDB::bind_method(D_METHOD("set_node_graph_initialized", "is_node_graph_initialized"), &GDMiniaudio::set_node_graph_initialized);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "node_graph_inintialized"), "set_node_graph_initialized", "get_node_graph_initialized");
}

// Initialize any variables here.
GDMiniaudio::GDMiniaudio() {
    input_device.instantiate();
    default_remote_stream.instantiate();
    engine_initialized = false;
    engine_started = false;
    node_graph_initialized = false;
}


// Add your cleanup here.
GDMiniaudio::~GDMiniaudio() {}

Ref<MiniaudioInputDevice> GDMiniaudio::get_input_device() {
    return input_device;
}

Ref<GDMiniaudioRemoteStreamDataSource> GDMiniaudio::get_remote_stream() {
    return default_remote_stream;
}

/**
 * @brief The method for setting the engine initialized variable.
 * This method is only here for godot to allow the property to be shown in the editor. 
 * This method only notifies the user that the variable is only set by the 'initialize_engine' method.
*/
void GDMiniaudio::set_engine_initialized(bool is_engine_initialized){
    UtilityFunctions::printerr("'engine_initialized' is only set by the 'initialize_engine' method, if the engine is not being initialized, you can contact me for help at support@pupper.dev or find zodiepupper on discord or elsewhere.");
}

/**
 * @brief The method for getting the engine initialized variable.
 * This allows the user to check if the engine is initialized manually.
 * @returns bool
*/
bool GDMiniaudio::get_engine_initialized(){
    return engine_initialized;
}

/**
 * @brief The method for setting the engine started variable.
 * This method is only here for godot to allow the property to be shown in the editor.
 * This method only notifies the user that the variable is only set by the 'start_engine' method.
*/
void GDMiniaudio::set_engine_started(bool is_engine_started){
    UtilityFunctions::printerr("'engine_started' is only set by the 'start_engine' method, if the engine is not starting, you can contact me for help at support@pupper.dev or find zodiepupper on discord or elsewhere.");
}

/**
 * @brief The method for getting the engine started variable.
 * This allows the user to check if the engine is started manually.
 * @returns bool
*/
bool GDMiniaudio::get_engine_started(){
    return engine_started;
}

/**
 * @brief The method for setting the node graph initialized variable.
 * This method is only here for godot to allow the property to be shown in the editor.
 * This method only notifies the user that the variable is only set by the 'initialize_node_graph' method.
*/
void GDMiniaudio::set_node_graph_initialized(bool is_node_graph_initialized){
    UtilityFunctions::printerr("'node_graph_initialized' is only set by the 'initialize_node_graph' if the node graph is not being initialized, you can contact me for help at support@pupper.dev or find zodiepupper on discord or elsewhere.");
}

/**
 * @brief The method for getting the node graph initialized variable.
 * This allows the user to check if the node graph is initialized manually.
 * @returns bool
*/
bool GDMiniaudio::get_node_graph_initialized(){
    return node_graph_initialized;
}

/**
 * @brief Checks if the engine is initialized and started.
 * This is a helper to check if the engine is initialized and started.
 * If the engine is not initialized, a message is printed indicating that the engine is not initialized.
 * If the engine is not started, a message is printed indicating that the engine is not started.
 * @returns bool
*/
bool GDMiniaudio::_check_engine(){
    if (!engine_initialized){
        UtilityFunctions::printerr("miniaudio engine not yet initialized.");
        return false;
    }
    if (!engine_started){
        UtilityFunctions::printerr("miniaudio engine not yet started");
        return false;
    }
    return true;
}


/**
 * @brief Initializes the audio engine.
 * If the engine is not already initialized, this function initializes it.
 * If the engine is already initialized, a message is printed indicating that the engine is already initialized.
 * @returns void
 */
void GDMiniaudio::initialize_engine() {
    if (!engine_initialized){
        ma_result result = ma_engine_init(NULL, &engine);
        switch (result){
            case MA_SUCCESS:
                engine_initialized = true;
                return;
            default:
                UtilityFunctions::printerr("Failed to initialize audio engine.\n");
                UtilityFunctions::printerr("Error code: %d\n", result);
                engine_initialized = false;
                return;
        }
    } else if(engine_initialized) {
        UtilityFunctions::printerr("The engine is already initialized.");
    }
}

/**
 * @brief Starts the audio engine.
 * If the engine is not already started, this function initializes it.
 * If the engine is already started, a message is printed indicating that the engine is already started.
 * @returns void
 */
void GDMiniaudio::start_engine() {
    if (!engine_started && engine_initialized){
        ma_result result = ma_engine_start(&engine);
        switch (result){
            case MA_SUCCESS:
                engine_started = true;
                return;
            default:
                UtilityFunctions::printerr("Failed to start audio engine.\n");
                UtilityFunctions::printerr("Error code: %d\n", result);
                engine_started = false;
                return;
        }
    } else if(!engine_initialized) {
        UtilityFunctions::printerr("The engine must be initialized before you can start it.");
    } else if (engine_started) {
        UtilityFunctions::printerr("The engine is already started.");
    }
    
}

/**
 * @brief Initializes the miniaudio node graph.
 * If the node graph is not initialized, this function initializes it.
 * If the node graph is already initialized, a message is printed indicating that the node graph is already initialized.
 * @returns void
*/
void GDMiniaudio::initialize_node_graph(){
    if (!node_graph_initialized){
        node_graph_config = ma_node_graph_config_init(2);
        
        ma_result result = ma_node_graph_init(&node_graph_config, NULL, &node_graph);
        switch (result){
            case MA_SUCCESS:
                node_graph_initialized = true;
                return;
            default:
                UtilityFunctions::printerr("Failed to initialize node graph.\n");
                UtilityFunctions::printerr("Error code: %d\n", result);
                node_graph_initialized = false;
                return;
        }
    } else if (node_graph_initialized) {
        UtilityFunctions::printerr("The node graph was already initialized.");
    }
}

/**
 * @brief Single function setup serving as a placeholder to quickly get the necessary audio config for testing voip
*/
void GDMiniaudio::setup_default_node_graph(){
    if (!engine_initialized){
        printf("initializing engine\n");
        initialize_engine();
    }
    if (!engine_started){
        printf("starting engine\n");
        start_engine();
    }
    printf("initializing default input device\n");
    input_device->initialize_default_input_device(&engine);
    default_remote_stream->gdma_init_remote_stream_data_source(&engine);
    ma_result result;
    result = ma_node_attach_output_bus(&default_remote_stream->data_source_node, 0, ma_node_graph_get_endpoint(&engine.nodeGraph), 0);
    printf("remote data source: %d\n", result);
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
    if (_check_engine()){
        ma_engine_play_sound(&engine, path.utf8().get_data(), NULL);
    }
}

void GDMiniaudio::load_audio_file(String path, Ref<MiniaudioSound> file) {
    if (_check_engine()){
        ma_result result = ma_sound_init_from_file(&engine, path.utf8().get_data(), NULL, NULL, NULL, file->sound);
        if (result != MA_SUCCESS) {
            UtilityFunctions::printerr("Failed to load audio file.\n");
            UtilityFunctions::printerr("Error code: %d\n", result);
        }
        file->path = path;
    }
}

void GDMiniaudio::play_sound(Ref<MiniaudioSound> sound) {
    if (_check_engine()){
        ma_sound_start(sound->sound);
        UtilityFunctions::print("started sound");
    }
}

void GDMiniaudio::set_sound_position(Ref<MiniaudioSound> sound, float posx, float posy, float posz){
    if (_check_engine()){
        ma_sound_set_position(sound->sound, posx, posy, posz);
        // UtilityFunctions::print("set position "+Vector3(posx, posy, posz));
    }
}

Vector3 GDMiniaudio::get_sound_position(Ref<MiniaudioSound> sound){
    if (_check_engine()){
        ma_vec3f result = ma_sound_get_position(sound->sound);
        // UtilityFunctions::print("got position");
        return Vector3(result.x, result.y, result.z);
    }
    return Vector3();
}

void GDMiniaudio::set_sound_velocity(Ref<MiniaudioSound> sound, float velx, float vely, float velz){
    if (_check_engine()){
        ma_sound_set_velocity(sound->sound, velx, vely, velz);
        // UtilityFunctions::print("set velocity "+Vector3(velx, vely, velz));
    }
}


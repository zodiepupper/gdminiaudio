#define MINIAUDIO_IMPLEMENTATION
#include "gdminiaudio.h"
#include <godot_cpp/core/class_db.hpp>
#include <iostream>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void GDMiniaudio::_bind_methods() {
    
    ClassDB::bind_method(D_METHOD("setup_default_node_graph"), &GDMiniaudio::setup_default_node_graph);
    
    ClassDB::bind_static_method("GDMiniaudio", D_METHOD("get_singleton"), &GDMiniaudio::get_singleton);
    ClassDB::bind_method(D_METHOD("initialize_engine"), &GDMiniaudio::initialize_engine);
    ClassDB::bind_method(D_METHOD("start_engine"), &GDMiniaudio::start_engine);
    ClassDB::bind_method(D_METHOD("initialize_node_graph"), &GDMiniaudio::initialize_node_graph);
    ClassDB::bind_method(D_METHOD("play_file", "path_to_file"), &GDMiniaudio::play_file);
    ClassDB::bind_method(D_METHOD("load_audio_file", "path_to_file"), &GDMiniaudio::load_audio_file);
    ClassDB::bind_method(D_METHOD("play_sound", "sound"), &GDMiniaudio::play_sound);
    ClassDB::bind_method(D_METHOD("set_sound_position", "sound", "X Position", "Y Position", "Z Position"), &GDMiniaudio::set_sound_position);
    ClassDB::bind_method(D_METHOD("get_sound_position", "sound"), &GDMiniaudio::get_sound_position);
    ClassDB::bind_method(D_METHOD("set_sound_velocity", "sound", "X Velocity", "Y Velocity", "Z Velocity"), &GDMiniaudio::set_sound_velocity);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "engine_initialized"), "set_engine_initialized", "get_engine_initialized");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "engine_started"), "set_engine_started", "get_engine_started");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "node_graph_inintialized"), "set_node_graph_initialized", "get_node_graph_initialized");
}

// Initialize any variables here.
GDMiniaudio::GDMiniaudio() {
    singleton = this;
    engine_initialized = false;
    engine_started = false;
    node_graph_initialized = false;
}


// Add your cleanup here.
GDMiniaudio::~GDMiniaudio() {}

void GDMiniaudio::set_engine_initialized(bool is_engine_initialized){
    UtilityFunctions::printerr("'engine_initialized' is only set by the 'initialize_engine' method, if the engine is not being initialized, you can contact me for help at support@pupper.dev or find zodiepupper on discord or elsewhere.");
}

bool GDMiniaudio::get_engine_initialized(){
    return engine_initialized;
}

void GDMiniaudio::set_engine_started(bool is_engine_started){
    UtilityFunctions::printerr("'engine_started' is only set by the 'start_engine' method, if the engine is not starting, you can contact me for help at support@pupper.dev or find zodiepupper on discord or elsewhere.");
}

bool GDMiniaudio::get_engine_started(){
    return engine_started;
}

void GDMiniaudio::set_node_graph_initialized(bool is_node_graph_initialized){
    UtilityFunctions::printerr("'node_graph_initialized' is only set by the 'initialize_node_graph' if the node graph is not being initialized, you can contact me for help at support@pupper.dev or find zodiepupper on discord or elsewhere.");
}

bool GDMiniaudio::get_node_graph_initialized(){
    return node_graph_initialized;
}

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

GDMiniaudio* GDMiniaudio::singleton = nullptr;

GDMiniaudio *GDMiniaudio::get_singleton() {
    return singleton;
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

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount){
    void* buffer;
    ma_uint32 framesToWrite = frameCount;
    if (ma_pcm_rb_acquire_write((ma_pcm_rb*)pDevice->pUserData, &framesToWrite, &buffer) == MA_SUCCESS) {
        // Calculate the number of bytes to copy.
        ma_uint32 bytesToWrite = framesToWrite * ma_get_bytes_per_frame(pDevice->capture.format, pDevice->capture.channels);

        // Copy the data from the input buffer to the ring buffer.
        memcpy(buffer, pInput, bytesToWrite);

        // Commit the written data.
        ma_pcm_rb_commit_write((ma_pcm_rb*)pDevice->pUserData, framesToWrite);
    }
}

static ma_result ma_input_device_read_pcm_frames(ma_device_data_source* pDeviceSource, void* pFramesOut, ma_uint64 frameCount, ma_uint64* pFramesRead){
    ma_uint32 framesToRead = ma_pcm_rb_available_read(&pDeviceSource->buffer);
    void* buffer;
    if ( ma_pcm_rb_acquire_read( (ma_pcm_rb*) &pDeviceSource->buffer, &framesToRead, &buffer) == MA_SUCCESS) {
        pFramesRead = (ma_uint64*)&framesToRead;
        pFramesOut = buffer;
        // Print the buffer
        float* floatBuffer = (float*)buffer;
        for (ma_uint32 i = 0; i < framesToRead; i++) {
            printf("%f ", floatBuffer[i]);
            if ((i + 1) % 16 == 0) {
                printf("\n");
            }
        }
        printf("\n");
    }
    return MA_SUCCESS;
}

static ma_result ma_input_device__data_source_on_read(ma_data_source* pDataSource, void* pFramesOut, ma_uint64 frameCount, ma_uint64* pFramesRead) {
    return ma_input_device_read_pcm_frames((ma_device_data_source*)pDataSource, pFramesOut, frameCount, pFramesRead);
}
static ma_result my_data_source_seek(ma_data_source* pDataSource, ma_uint64 frameIndex)
{
    return MA_NOT_IMPLEMENTED;
    // Seek to a specific PCM frame here. Return MA_NOT_IMPLEMENTED if seeking is not supported.
}

static ma_result my_data_source_get_data_format(ma_data_source* pDataSource, ma_format* pFormat, ma_uint32* pChannels, ma_uint32* pSampleRate, ma_channel* pChannelMap, size_t channelMapCap)
{
    *pFormat = ma_format_f32;
    *pChannels = 2;
    *pSampleRate = 48000;
    return MA_SUCCESS;
    // Return the format of the data here.
}

static ma_result my_data_source_get_cursor(ma_data_source* pDataSource, ma_uint64* pCursor)
{
    pCursor = 0;
    return MA_NOT_IMPLEMENTED;
    // Retrieve the current position of the cursor here. Return MA_NOT_IMPLEMENTED and set *pCursor to 0 if there is no notion of a cursor.
}

static ma_result my_data_source_get_length(ma_data_source* pDataSource, ma_uint64* pLength)
{
    pLength = 0;
    return MA_NOT_IMPLEMENTED;
    // Retrieve the length in PCM frames here. Return MA_NOT_IMPLEMENTED and set *pLength to 0 if there is no notion of a length or if the length is unknown.
}

static ma_data_source_vtable g_ma_device_data_source_vtable = {
  ma_input_device__data_source_on_read,
  my_data_source_seek,
  my_data_source_get_data_format,
  my_data_source_get_cursor,
  my_data_source_get_length,
};

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
    
    printf("Create input data source config\n");
    ma_data_source_config baseConfig;
    
    baseConfig = ma_data_source_config_init();
    baseConfig.vtable = &g_ma_device_data_source_vtable;
    
    ma_result result = ma_data_source_init(&baseConfig, &input_device_data_source.base);
    printf("Initialized data source: %d\n", result);
    
    result = ma_pcm_rb_init(ma_format_f32, 2, 48000*.2, NULL, NULL, &input_device_data_source.buffer);
    printf("Initialized input data source ring buffer: %d\n", result);
    
    printf("Configuring device\n");
    ma_device_config config = ma_device_config_init(ma_device_type_capture);
    
    // Set to ma_format_unknown to use the device's native format.
    config.capture.format   = ma_format_f32;
    // Set to 0 to use the device's native channel count.
    config.capture.channels = 2;
    // Set to 0 to use the device's native sample rate.
    config.sampleRate        = 48000;
    // This function will be called when miniaudio needs more data.
    config.dataCallback      = data_callback;
    // Can be accessed from the device object (device.pUserData).
    config.pUserData         = &input_device_data_source.buffer;
    
    result = ma_device_init(NULL, &config, &device);
    printf("initialized device: %d\n", result);
    
    // The device is sleeping by default so you'll need to start it manually.
    ma_device_start(&device);
    
    ma_data_source_node_config ds_node_config = ma_data_source_node_config_init(&input_device_data_source);
    result = ma_data_source_node_init(ma_engine_get_node_graph(&engine), &ds_node_config, NULL, &dataSourceNode);
    printf("Initialized data source node: %d\n", result);
    printf("source node bus count: %d\n",ma_node_get_output_bus_count(&dataSourceNode));
    printf("endpoint node bus count: %d\n",ma_node_get_output_bus_count(ma_engine_get_endpoint(&engine)));
    printf("source node channel count: %d\n",ma_node_get_output_channels(&dataSourceNode, 0));
    printf("endpoint node channel count: %d\n",ma_node_get_output_channels(ma_engine_get_endpoint(&engine), 0));
    result = ma_node_attach_output_bus(&dataSourceNode, 0, ma_node_graph_get_endpoint(&engine.nodeGraph), 0);
    printf("Attached data source node: %d\n", result);
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


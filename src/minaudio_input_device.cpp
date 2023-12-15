#include "gdminiaudio.h"

using namespace godot;

/**
 * @brief Callback for reading data from the input device.
*/
void input_device_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount){
    MiniaudioInputDevice::ma_device_data_source* tmpdevice = (MiniaudioInputDevice::ma_device_data_source*)pDevice->pUserData;
    
    // if buffer is 4x frameCount, then reset it to prevent audio delay
    if (ma_pcm_rb_available_read(&tmpdevice->ring_buffer) > frameCount * 4){
        ma_pcm_rb_reset(&tmpdevice->ring_buffer);
    }
    
    // simple noise gate
    
    // amplify the audio
    for (int i = 0; i < frameCount * pDevice->capture.channels; i++){
        // if ( ((float*)pInput)[i] > .02 ){
        //     ((float*)pInput)[i] *= 16.0;
        // }
        ((float*)pInput)[i] *= (((float*)pInput)[i]+1.0)*8.0;
    }
    
    // calculate the required size
    size_t required_size = frameCount * pDevice->capture.channels * sizeof(float);
    
    float* tmpbuffer;
    
    // get write pointer from ring buffer
    ma_result result = ma_pcm_rb_acquire_write((ma_pcm_rb*)&tmpdevice->ring_buffer, &frameCount, (void**)&tmpbuffer);
    if (result != MA_SUCCESS) {
        printf("Failed to acquire write pointer from ring buffer.\n");
        printf("result: %d\n", result);
        return;
    }
    // copy the input data to the buffer
    memcpy(tmpbuffer, (float*)pInput, required_size);
    
    // increment the buffer size so we know how much can be read from gdscript
    tmpdevice->buffer_size += frameCount;
    
    // commit the write
    result = ma_pcm_rb_commit_write(&tmpdevice->ring_buffer, frameCount);
    if (result != MA_SUCCESS) {
        printf("Failed to commit write pointer from ring buffer.\n");
        printf("result: %d\n", result);
        return;
    }
}

/**
 * @brief Initializes the input device and the data source node.
*/
void MiniaudioInputDevice::initialize_default_input_device(ma_engine *pEngine){
    engine = pEngine;
    
    // initialize ring buffer
    ma_result result = ma_pcm_rb_init(ma_format_f32, 2, 48000*.2, NULL, NULL, &input_device_data_source.ring_buffer);
    if (result != MA_SUCCESS) {
        printf("Failed to initialize ring buffer.\n");
        printf("result: %d\n", result);
    }
    
    // create, initialize and start the input device
    ma_device_config device_config = ma_device_config_init(ma_device_type_capture);
    // Set to ma_format_unknown to use the device's native format.
    device_config.capture.format   = ma_format_f32;
    // Set to 0 to use the device's native channel count.
    device_config.capture.channels  = 2;
    // Set to 0 to use the device's native sample rate.
    device_config.sampleRate        = 48000;
    // This function will be called when miniaudio needs more data.
    device_config.dataCallback      = input_device_data_callback;
    // Can be accessed from the device object (device.pUserData).
    device_config.pUserData         = &input_device_data_source;
    
    result = ma_device_init(NULL, &device_config, &device);
    if (result != MA_SUCCESS) {
        printf("Failed to initialize input device.\n");
        printf("result: %d\n", result);
    }
    
    ma_device_start(&device);
}
#include "miniaudio_input_device.h"

/**
 * @brief Callback for reading data from the input device.
*/
void input_device_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount){
    MiniaudioInputDevice::ma_device_data_source* tmpdevice = (MiniaudioInputDevice::ma_device_data_source*)pDevice->pUserData;
    
    // if buffer is 4x frameCount, then reset it to prevent audio delay
    if (ma_pcm_rb_available_read(&tmpdevice->ring_buffer) > frameCount * 4){
        ma_pcm_rb_reset(&tmpdevice->ring_buffer);
    }
    
    // manually amplify the input data by just multiplying it
    for (int i = 0; i < frameCount * pDevice->capture.channels; i++){
        ((float*)pInput)[i] *= 8.0;
        // if (((float*)pInput)[i] > .05) {
        //     ((float*)pInput)[i] = .05;
        // }
        // if (((float*)pInput)[i] < -.05) {
        //     ((float*)pInput)[i] = -.05;
        // }
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
    // write the amount of frames read to the buffer size
    tmpdevice->buffer_size = frameCount;
    
    // commit the write
    result = ma_pcm_rb_commit_write(&tmpdevice->ring_buffer, frameCount);
    if (result != MA_SUCCESS) {
        printf("Failed to commit write pointer from ring buffer.\n");
        printf("result: %d\n", result);
        return;
    }
    
    // printf("in frameCount: %d\n", (int)frameCount);
}
/**
 * @brief actual function where we read the ringbuffer populated by the input device to get 
*/
ma_result ma_input_device_read_pcm_frames(MiniaudioInputDevice::ma_device_data_source* pDeviceSource, void* pFramesOut, ma_uint64 frameCount, ma_uint64* pFramesRead){
    float* tmpbuffer;
    int buffer_size = pDeviceSource->buffer_size;
    int rb_available = ma_pcm_rb_available_read(&pDeviceSource->ring_buffer)/4.0;
    if ( rb_available > frameCount){
        // get read pointer from ring buffer
        ma_result result = ma_pcm_rb_acquire_read(&pDeviceSource->ring_buffer, (ma_uint32*)&frameCount, (void**)&tmpbuffer);
        if (result != MA_SUCCESS) {
            printf("Failed to acquire read pointer from ring buffer.\n");
            printf("result: %d\n", result);
        }
        // copy the buffer to the output
        memcpy((float*)pFramesOut, tmpbuffer, frameCount * 2 * sizeof(float));
        *pFramesRead = frameCount;
        // commit the read
        result = ma_pcm_rb_commit_read(&pDeviceSource->ring_buffer, frameCount);
        if (result != MA_SUCCESS) {
            // printf("Failed to commit read pointer from ring buffer.\n");
            // printf("result: %d\n", result);
        }
        
        return MA_SUCCESS;
    }
    pFramesRead = 0;
    return MA_SUCCESS;
    
}
/**
 * @brief Callback for reading data from the input device.
 * This just calls the ma_input_device_read_pcm_frames function.
*/
ma_result ma_input_device__data_source_on_read(ma_data_source* pDataSource, void* pFramesOut, ma_uint64 frameCount, ma_uint64* pFramesRead) {
    MiniaudioInputDevice::ma_device_data_source* pDeviceSource = (MiniaudioInputDevice::ma_device_data_source*)pDataSource;
    return ma_input_device_read_pcm_frames(pDeviceSource, pFramesOut, frameCount, pFramesRead);
}
/**
 * @brief Callback for seeking data from the input device.
 * This is not implemented.
*/
static ma_result my_data_source_seek(ma_data_source* pDataSource, ma_uint64 frameIndex)
{
    return MA_NOT_IMPLEMENTED;
}
/**
 * @brief Callback for getting the data format from the input device.
*/
static ma_result my_data_source_get_data_format(ma_data_source* pDataSource, ma_format* pFormat, ma_uint32* pChannels, ma_uint32* pSampleRate, ma_channel* pChannelMap, size_t channelMapCap)
{
    *pFormat = ma_format_f32;
    *pChannels = 2;
    *pSampleRate = 48000;
    return MA_SUCCESS;
}
/**
 * @brief Callback for getting the cursor from the input device.
 * This is not implemented.
*/
static ma_result my_data_source_get_cursor(ma_data_source* pDataSource, ma_uint64* pCursor)
{
    pCursor = 0;
    return MA_NOT_IMPLEMENTED;
    // Retrieve the current position of the cursor here. Return MA_NOT_IMPLEMENTED and set *pCursor to 0 if there is no notion of a cursor.
}
/**
 * @brief Callback for getting the length from the input device.
 * This is not implemented.
*/
static ma_result my_data_source_get_length(ma_data_source* pDataSource, ma_uint64* pLength)
{
    pLength = 0;
    return MA_NOT_IMPLEMENTED;
    // Retrieve the length in PCM frames here. Return MA_NOT_IMPLEMENTED and set *pLength to 0 if there is no notion of a length or if the length is unknown.
}
/**
 * @brief The vtable for our input device data source.
*/
static inline ma_data_source_vtable g_ma_device_data_source_vtable = {
ma_input_device__data_source_on_read,
my_data_source_seek,
my_data_source_get_data_format,
my_data_source_get_cursor,
my_data_source_get_length,
};
/**
 * @brief Initializes the input device and the data source node.
*/
void MiniaudioInputDevice::initialize_default_input_device(){printf("Initalize data source ring buffer\n");
    ma_result result = ma_pcm_rb_init(ma_format_f32, 2, 48000*.2, NULL, NULL, &input_device_data_source.ring_buffer);
    if (result != MA_SUCCESS) {
        printf("Failed to initialize ring buffer.\n");
        printf("result: %d\n", result);
    }
    
    ma_data_source_config baseConfig;
    
    baseConfig = ma_data_source_config_init();
    baseConfig.vtable = &g_ma_device_data_source_vtable;
    
    result = ma_data_source_init(&baseConfig, &input_device_data_source.base);
    
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
    
    // The device is sleeping by default so you'll need to start it manually.
    ma_device_start(&device);

    dataSourceNodeConfig = ma_data_source_node_config_init(&input_device_data_source);
}
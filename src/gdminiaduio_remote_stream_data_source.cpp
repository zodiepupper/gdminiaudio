#include "gdminiaduio_remote_stream_data_source.h"


/**
 * @brief Function where we actually do what the below function claims to do lol
 * @returns ma_result
*/
ma_result gdma_remote_stream_read_pcm_frames(GDMiniaudioRemoteStreamDataSource::gdma_remote_stream_data_source* pDataSource, void* pFramesOut, ma_uint64 frameCount, ma_uint64* pFramesRead) {
    float* tmpbuffer;
    int buffer_size = pDataSource->buffer_size;
    int rb_available = ma_pcm_rb_available_read(&pDataSource->ring_buffer)/4.0;
    if ( rb_available > frameCount){
        
        // get read pointer from ring buffer
        ma_result result = ma_pcm_rb_acquire_read(&pDataSource->ring_buffer, (ma_uint32*)&frameCount, (void**)&tmpbuffer);
        if (result != MA_SUCCESS) {
            printf("Failed to acquire read pointer from ring buffer.\n");
            printf("result: %d\n", result);
        }
        
        // copy the buffer to the output
        memcpy((float*)pFramesOut, tmpbuffer, frameCount * 2 * sizeof(float));
        *pFramesRead = frameCount;
        // commit the read to the ring buffer
        result = ma_pcm_rb_commit_read(&pDataSource->ring_buffer, frameCount);
        if (result != MA_SUCCESS) {
            printf("Failed to commit read pointer from ring buffer.\n");
            printf("result: %d\n", result);
        }
        
        return MA_SUCCESS;
    }
    pFramesRead = 0;
    return MA_SUCCESS;
}

/**
 * @brief Read from the ringbuffer and return it to *pFramesOut so the engine can play it
 * @returns ma_result
*/
ma_result gdma_remote_stream_data_source_on_read(ma_data_source* pDataSource, void* pFramesOut, ma_uint64 frameCount, ma_uint64* pFramesRead) {
    GDMiniaudioRemoteStreamDataSource::gdma_remote_stream_data_source* actual_data_source = (GDMiniaudioRemoteStreamDataSource::gdma_remote_stream_data_source*)pDataSource;
    return gdma_remote_stream_read_pcm_frames(actual_data_source, pFramesOut, frameCount, pFramesRead);
}

// FILLERS FOR OTHER DATA SOURCE ACTIONS, MOST OF WHICH ARE NOT SUPPORTED BECAUSE IT'S A REMOTE STREAM

/**
 *  @brief Callback for seeking data from the input device.
 * This is not implemented.
*/
static ma_result my_data_source_seek(ma_data_source* pDataSource, ma_uint64 frameIndex)
{
    return MA_NOT_IMPLEMENTED;
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

// END OF FILLERS

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
 * @brief The vtable for our input device data source.
*/
static inline ma_data_source_vtable g_ma_device_data_source_vtable = {
gdma_remote_stream_data_source_on_read,
my_data_source_seek,
my_data_source_get_data_format,
my_data_source_get_cursor,
my_data_source_get_length,
};

ma_result GDMiniaudioRemoteStreamDataSource::gdma_init_remote_stream_data_source(ma_engine *pEngine){
    engine = pEngine;
    
    // initialize spatializer for this data source
    // init config
    ma_spatializer_config defaultSpatializerConfig = ma_spatializer_config_init(2, 2);
    ma_spatializer_init(&defaultSpatializerConfig, NULL, &stream_data_source.spatializer);
    
    // initialize ring buffer
    ma_result result = ma_pcm_rb_init(ma_format_f32, 2, 48000*2.0, NULL, NULL, &stream_data_source.ring_buffer);
    if (result != MA_SUCCESS) {
        printf("Failed to initialize ring buffer.\n");
        printf("result: %d\n", result);
        return MA_ERROR;
    }
    
    // create and initialize data source config and data source
    ma_data_source_config baseConfig;
    baseConfig = ma_data_source_config_init();
    baseConfig.vtable = &g_ma_device_data_source_vtable;
    result = ma_data_source_init(&baseConfig, &stream_data_source.base);

    data_source_node_config = ma_data_source_node_config_init(&stream_data_source);
    result = ma_data_source_node_init(ma_engine_get_node_graph(engine), &data_source_node_config, NULL, &data_source_node);
    if (result != MA_SUCCESS) {
        printf("Failed to initialize default input device data source.\n");
        printf("result: %d\n", result);
        return MA_ERROR;
    }
    return MA_SUCCESS;
}

int GDMiniaudioRemoteStreamDataSource::gdma_push_buffer(PackedFloat32Array data) {
    
    size_t required_size = data.size() * 2 * sizeof(float);
    ma_uint32 frameCount = data.size()/2.0;
    
    float* tmpbuffer;
    
    ma_result result = ma_pcm_rb_acquire_write(&stream_data_source.ring_buffer, &frameCount, (void**)&tmpbuffer);
    printf("push to write buffer\n");
    if (result != MA_SUCCESS) {
        printf("Failed to acquire write pointer from ring buffer.\n");
        printf("result: %d\n", result);
        return MA_ERROR;
    }
    for (int i = 0; i < frameCount*2.0; i ++) {
        tmpbuffer[i] = data[i];
    }
    
    result = ma_pcm_rb_commit_write(&stream_data_source.ring_buffer, frameCount);
    
    return MA_SUCCESS;
}
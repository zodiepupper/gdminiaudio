#ifndef GDMINIAUDIO_INPUT_DEVICE_H
#define GDMINIAUDIO_INPUT_DEVICE_H
#include "../miniaudio/miniaudio.h"

#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/ref.hpp>

using namespace godot;

class MiniaudioInputDevice : public RefCounted {
    GDCLASS(MiniaudioInputDevice, RefCounted);
    protected:
        static void _bind_methods(){}
    public:
        MiniaudioInputDevice(){}
        ~MiniaudioInputDevice(){}
        
        struct ma_device_data_source {
            ma_data_source_base base;
            ma_spatializer spatializer;
            ma_pcm_rb ring_buffer;
            int buffer_size;
        };
        
        ma_device device;
        ma_device_data_source input_device_data_source;
        ma_data_source_node dataSourceNode;
        ma_data_source_node_config dataSourceNodeConfig;
        
        // void input_device_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
        static ma_result ma_input_device__data_source_on_read(ma_data_source* pDataSource, void* pFramesOut, ma_uint64 frameCount, ma_uint64* pFramesRead);
        ma_result ma_input_device_read_pcm_frames(MiniaudioInputDevice::ma_device_data_source* pDeviceSource, void* pFramesOut, ma_uint64 frameCount, ma_uint64* pFramesRead);
        void initialize_default_input_device();
};

#endif // GDMINIAUDIO_INPUT_DEVICE_H
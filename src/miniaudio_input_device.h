#ifndef GDMINIAUDIO_INPUT_DEVICE_H
#define GDMINIAUDIO_INPUT_DEVICE_H
#include "../miniaudio/miniaudio.h"

#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

class MiniaudioInputDevice : public RefCounted {
    GDCLASS(MiniaudioInputDevice, RefCounted);
    protected:
        static void _bind_methods(){
            // class bindings for set/get of engine initialized bool
            ClassDB::bind_method(D_METHOD("get_input_device_buffer_size"), &MiniaudioInputDevice::get_input_device_buffer_size);
            ClassDB::bind_method(D_METHOD("set_input_device_buffer_size", "new_buffer_size"), &MiniaudioInputDevice::set_input_device_buffer_size);
            // bind buffer size property
            ADD_PROPERTY(PropertyInfo(Variant::INT, "buffer_size"), "set_input_device_buffer_size", "get_input_device_buffer_size");
            // class bindings for set/get of engine initialized bool
            ClassDB::bind_method(D_METHOD("get_buffer"), &MiniaudioInputDevice::get_buffer);
            ClassDB::bind_method(D_METHOD("set_buffer", "new_buffer_data"), &MiniaudioInputDevice::set_buffer);
            // bind buffer size property
            // ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "buffer"), "set_buffer", "get_buffer");
        }
    public:
        MiniaudioInputDevice(){}
        ~MiniaudioInputDevice(){}
        
        struct ma_device_data_source {
            ma_pcm_rb ring_buffer;
            int buffer_size;
        };
        
        ma_device device;
        ma_device_data_source input_device_data_source;
        ma_engine *engine;
        
        void initialize_default_input_device(ma_engine *pEngine);
        
        void set_input_device_buffer_size(int new_buffer_size) {
            UtilityFunctions::printerr("the buffer size should not be set manually and should be set automatically by other functions\nyou can contact me for help at support@pupper.dev or find zodiepupper on discord or elsewhere.");
        }
        
        int get_input_device_buffer_size() {
            return input_device_data_source.buffer_size;
        }
        
        void set_buffer(PackedFloat32Array new_buffer_data) {
            UtilityFunctions::printerr("the buffer data should not be set manually and should be set automatically by other functions\nif you modify the buffer data it will not be accurate to the data from the input device\nyou can contact me for help at support@pupper.dev or find zodiepupper on discord or elsewhere.");
        }
        
        /**
         * @brief Gets and returns the buffer data from the ring buffer
         * This will return the buffer if it contains at least 960 frames of data, otherwise it will return an empty array
        */
        PackedFloat32Array get_buffer() {
            PackedFloat32Array output = PackedFloat32Array();
            float* tmpbuffer;
            int frameCount = 960;
            int rb_available = ma_pcm_rb_available_read(&input_device_data_source.ring_buffer);
            if ( rb_available > frameCount){
                
                // get read pointer from ring buffer
                ma_result result = ma_pcm_rb_acquire_read(&input_device_data_source.ring_buffer, (ma_uint32*)&frameCount, (void**)&tmpbuffer);
                if (result != MA_SUCCESS) {
                    printf("Failed to acquire read pointer from ring buffer.\n");
                    printf("result: %d\n", result);
                }
                
                output.resize(frameCount*2.0);
                for (int i = 0; i < frameCount*2.0; i++) {
                    output.append( (tmpbuffer)[i] );
                }
                
                // commit the read to the ring buffer
                result = ma_pcm_rb_commit_read(&input_device_data_source.ring_buffer, frameCount);
                if (result != MA_SUCCESS) {
                    printf("Failed to commit read pointer from ring buffer.\n");
                    printf("result: %d\n", result);
                }
                
                return output;
            }
            return output;
        }
};

#endif // GDMINIAUDIO_INPUT_DEVICE_H
#ifndef GDMINIAUDIO_STREAM_DATA_SOURCE_H
#define GDMINIAUDIO_STREAM_DATA_SOURCE_H
#include "../miniaudio/miniaudio.h"

#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/ref.hpp>

using namespace godot;

class GDMiniaudioRemoteStreamDataSource : public RefCounted {
    GDCLASS(GDMiniaudioRemoteStreamDataSource, RefCounted);
    protected:
        static void _bind_methods(){
            ClassDB::bind_method(D_METHOD("gdma_push_buffer"), &gdma_push_buffer);
        }
    public:
        GDMiniaudioRemoteStreamDataSource(){}
        ~GDMiniaudioRemoteStreamDataSource(){}
        
        struct gdma_remote_stream_data_source {
            ma_data_source_base base;
            ma_spatializer spatializer;
            ma_pcm_rb ring_buffer;
            int buffer_size;
        };
        
        gdma_remote_stream_data_source stream_data_source;
        ma_data_source_node_config data_source_node_config;
        ma_data_source_node data_source_node;
        
        // This should be set so we can access the engine (ex: for spatializer)
        ma_engine *engine;
        
        ma_result gdma_init_remote_stream_data_source(ma_engine *pEngine);
        int gdma_push_buffer(PackedFloat32Array data);
};

#endif // GDMINIAUDIO_STREAM_DATA_SOURCE
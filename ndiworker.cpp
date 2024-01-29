#include "ndiworker.h"

void NDIWorker::setSourceName(QString sourceName) {
    this->sourceName = sourceName;
}

void NDIWorker::start() {
    run = true;

    // Initialize library
    if (!NDIlib_initialize())
        exit(0);

     // Create finder instance
     NDIlib_find_instance_t pNDI_find = NDIlib_find_create_v2();
     if (!pNDI_find)
         exit(0);

     // Find sources
     int source = -1;
     uint32_t no_sources = 0;

     const NDIlib_source_t *p_sources = NULL;
     while (source == -1) {
         qDebug("Looking for sources...\n");

         NDIlib_find_wait_for_sources(pNDI_find, 1000);
         p_sources = NDIlib_find_get_current_sources(pNDI_find, &no_sources);

         for (uint32_t i = 0; i < no_sources; i++) {
             if (strcmp(p_sources[i].p_ndi_name, sourceName.toStdString().c_str()) == 0)
                 source = i;
         }
     }


     // Create receiver
     NDIlib_recv_create_v3_t opts;
     opts.color_format = NDIlib_recv_color_format_RGBX_RGBA;

     pNDI_recv = NDIlib_recv_create_v3(&opts);
     if (!pNDI_recv)
         exit(0);


     // Connect receiver to video source and destroy finder
     NDIlib_recv_connect(pNDI_recv, p_sources + source);
     NDIlib_find_destroy(pNDI_find);


     // Run loop
     while (run == true) {
         NDIlib_video_frame_v2_t video_frame;
         NDIlib_audio_frame_v2_t audio_frame;

         switch (NDIlib_recv_capture_v2(pNDI_recv, &video_frame, &audio_frame, nullptr, 5000)) {

         case NDIlib_frame_type_none:
             break;

         case NDIlib_frame_type_video:
             QImage *frame;
             frame = new QImage(video_frame.p_data, video_frame.xres, video_frame.yres, QImage::Format_RGBA8888);
             emit frameUpdated(*frame);

             NDIlib_recv_free_video_v2(pNDI_recv, &video_frame);
             break;

         case NDIlib_frame_type_audio:
             NDIlib_recv_free_audio_v2(pNDI_recv, &audio_frame);

         case NDIlib_frame_type_metadata:
         case NDIlib_frame_type_error:
         case NDIlib_frame_type_status_change:
         case NDIlib_frame_type_max:
             break;
         }

     }

     NDIlib_recv_destroy(pNDI_recv);
     NDIlib_destroy();

     emit finished();
}

void NDIWorker::stop() {
    run = false;
}

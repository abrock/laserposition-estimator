#include "cameramanager.h"

#include <glog/logging.h>

#include "misc.h"
using namespace Misc;

void CameraManager::runCamera() {
    println("Running CameraManager::runCamera");

    camera = arv_camera_new (NULL, &error);

    CHECK(ARV_IS_CAMERA(camera));

    ArvStream *stream = NULL;

    println ("Found camera '{}'",
             arv_camera_get_model_name (camera, NULL));

    arv_camera_set_acquisition_mode (camera, ARV_ACQUISITION_MODE_CONTINUOUS, &error);
    println("arv_camera_set_acquisition_mode done.");

    /* Create the stream object without callback */
    stream = arv_camera_create_stream (camera, NULL, NULL, &error);
    CHECK_EQ(nullptr, error) << error->message;

    CHECK(ARV_IS_STREAM (stream));

    size_t payload;

    /* Retrieve the payload size for buffer creation */
    payload = arv_camera_get_payload (camera, &error);
    CHECK_EQ(nullptr, error) << error->message;

    /* Insert some buffers in the stream buffer pool */
    for (int i = 0; i < 5; i++)
        arv_stream_push_buffer (stream, arv_buffer_new (payload, NULL));

    arv_camera_start_acquisition (camera, &error);
    CHECK_EQ(nullptr, error) << error->message;

    for (size_t ii = 0; !stopped; ++ii) {
        ArvBuffer *buffer;

        buffer = arv_stream_pop_buffer(stream);
        if (ARV_IS_BUFFER (buffer)) {
            /* Display some informations about the retrieved buffer */
            println ("Acquired {}×{} buffer #{}",
                    arv_buffer_get_image_width (buffer),
                    arv_buffer_get_image_height (buffer),
                    ii);
            /* Don't destroy the buffer, but put it back into the buffer pool */
            arv_stream_push_buffer (stream, buffer);
        }
    }

    CHECK_EQ(nullptr, error) << error->message;
    arv_camera_stop_acquisition (camera, &error);
    CHECK_EQ(nullptr, error) << error->message;

    /* Destroy the stream object */
    g_clear_object (&stream);

    /* Destroy the camera instance */
    g_clear_object (&camera);
}

void CameraManager::stop() {
    stopped = true;
}

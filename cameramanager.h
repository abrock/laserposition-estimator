#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <arv.h>

#include <stdlib.h>
#include <stdio.h>

class CameraManager {
    ArvCamera *camera;
    GError *error = nullptr;

    bool stopped = false;

public:
    void runCamera();


    void stop();
};

#endif // CAMERAMANAGER_H

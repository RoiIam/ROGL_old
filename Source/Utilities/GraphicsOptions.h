//
// Created by RoiIam on 16. 6. 2023.
//

#ifndef ROGL_GRAPHICSOPTIONS_H
#define ROGL_GRAPHICSOPTIONS_H


class GraphicsOptions {



    public:
    enum RendererType {forward,deferred};
    RendererType rendererType = forward;
    bool enableShadows = false;
    bool enableWater = false;
};


#endif //ROGL_GRAPHICSOPTIONS_H

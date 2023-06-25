#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/type_ptr.hpp> //to use make_vec3
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <vector>
#ifndef ROGL_PATH_H
#define ROGL_PATH_H


class Path {

public:
    Path();

    ~Path();

    glm::vec3 lerp(  const glm::vec3 &p0, const glm::vec3 &p1, const float t);
    glm::vec3 bezierPoint(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2,
                          const glm::vec3 &p3,const glm::vec3 &p4, const float t);
    void bezierShape(int count);


    std::vector<glm::vec3> finalPoints;
    //R is now defined using 4 control points, but curve is for 5, thats why it looks wierd
    std::vector<glm::vec3> controlPoints =
        //it should be 2*x+3 becs they are control points

            {
                    {2,10,0},
                    {4,10,0},
                    {6,10,0},
                    {8,10,0},
                    {10,10,3},
                    {10,10,6},
                    {10,10,10},
                    {10,10,10},
                    {6,10,10},
                    {3,10,10},
                    {2,10,10}
                    //{2,10,7}
                    //{2,10,4}
                    //{2,10,3},
                    //{2,10,0}




            };
    /* {
     { 0.0*3,  -1.0*3,0},
     { 0.0*3,  -0.3*3,0},
     { 0.0*3,   0.3*3,0},
     { 0.0*3,   1.0*3,0},
     { 0.3*3,   1.0*3,0},
     { 0.5*3,   1.0*3,0},
     { 0.5*3,   0.5*3,0},
     { 0.5*3,   0.0*3,0},
     { 0.3*3,   0.0*3,0},
     { 0.0*3,   0.0*3,0},
     { 0.3*3,  -0.3*3,0},
     { 0.5*3,  -0.5*3,0},
     { 0.5*3,  -1.0*3,0},

}*/;




};


#endif //ROGL_PATH_H

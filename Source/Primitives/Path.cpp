//
// Created by RoiIam on 22. 6. 2023.
//
#include "Path.h"


Path::Path() = default;

Path::~Path() = default;


glm::vec3 Path::lerp(const glm::vec3 &p0, const glm::vec3 &p1, const float t) {
    glm::vec3 dest = {};
    dest.x = p0.x + (p1.x - p0.x) * t;
    dest.y = p0.y + (p1.y - p0.y) * t;
    dest.z = p0.z + (p1.z - p0.z) * t;
    return dest;
}

//https://www.cubic.org/docs/bezier.htm CODE adapter from this site,
// Compute points for Bezier curve using 4 control points
glm::vec3 Path::bezierPoint(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3,
                            const glm::vec3 &p4, const float t) {
    // TODO: Compute point on the Bezier curve
    glm::vec3 ab, bc, cd, de, abbc, bccd, cdde, x1, x2, dest;
    ab = lerp(p0, p1, t);           // point between a and b (green)
    bc = lerp(p1, p2, t);           // point between b and c (green)
    cd = lerp(p2, p3, t);           // point between c and d (green)
    de = lerp(p3, p4, t);
    abbc = lerp(ab, bc, t);       // point between ab and bc (blue)
    bccd = lerp(bc, cd, t);       // point between bc and cd (blue)
    cdde = lerp(cd, de, t);
    x1 = lerp(abbc, bccd, t);   // point on the bezier-curve (black)
    x2 = lerp(bccd, cdde, t);
    dest = lerp(x1, x2, t);
    return dest;
}

// Compute points for a sequence of Bezier curves defined by a vector of control points
// Each bezier curve will reuse the end point of the previous curve
// count - Number of points to generate on each curve
void Path::bezierShape(int count) {
    //float i =0;
    for (int i = 0; i < (int) controlPoints.size() - 3; i += 4) {
        //glm::vec2 last = bezierPoint(controlPoints[i],controlPoints[i+1],controlPoints[i+2],controlPoints[i+3],t += 0.1f);
        //oops float in for loop...
        for (float j = 0.01; j <= count; j += 0.08f) {
            // TODO: Generate points for each Bezier curve and insert them


            glm::vec3 point = bezierPoint(controlPoints[i], controlPoints[i + 1], controlPoints[i + 2],
                                          controlPoints[i + 3], controlPoints[i + 4], j);
            //last = point;
            finalPoints.emplace_back(point);

            std::cout << "added point: " << glm::to_string(point) << std::endl;
        }
    }
}
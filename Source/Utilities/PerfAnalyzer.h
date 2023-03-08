#pragma once //for now guard it like this-scene instance includes cube and this, cube also includes this...
// note: unguarded header; consider using #ifdef guards or #pragma once

/*
 * Simple Utility ,to count drawcalls
 * PerfAnalyzer::drawcallCount needs to be added after every DrawCall to count properly,
 * not counting ImGui calls currently
 */
    class PerfAnalyzer {
    public:
        static int drawcallCount; //je toto spravne definovane aj v c++ aby to bolo akoze static?
    };



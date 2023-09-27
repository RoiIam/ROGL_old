#include "Log.h"
//Added here Salanci pc games subject 2.nd exercise
// 28.9.23 - 3.rd assigment implement fast sine and compare it to built in

double fastSine(double x)
{
    // dobre fungjue len pre <-π / 2, +π / 2>
//x - 0.16605 * x3 + 0.00761 * x5
    //return x - 0.16605 * pow(x,3) + 0.00761 * pow(x,5); //vstavane pow je strasne draha operacia

    return x - 0.16605 * x*x*x + 0.00761 * x*x*x*x*x;

}

double standardSine(double x)
{
    return sin(x);
}
double resultSS[100];
double resultFS[100];

void TestSine() {


    double start = -90;
    double end = 90;
    double steps =10000000;
    double step = (abs(start)+abs(end)) /steps;
    double it = start;

    std::chrono::high_resolution_clock::time_point startT = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point endFs=startT;
    //std::chrono::high_resolution_clock::time_point finishT = startT;

    for (int i = 0; i < steps; ++i) {
        //resultSS[i] = standardSine(it);
        standardSine(it);
        it+=step;
    }

    endFs = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedSS = endFs - startT;


    //reset iterator
    it = start;

    startT = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < steps; ++i) {
        fastSine(it);
        //resultFS[i] = fastSine(it);
        it+=step;
    }

    endFs = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedFS = endFs - startT;

    //reset iterator
    it = start;
    /*
    //print it all together
    for (int i = 0; i < steps; ++i) {
        std::cout<< "fast " << it <<  ": " << resultFS[i]
        <<"standard " << it <<  ": " << resultSS[i]
        << "delta is f-s" << resultFS[i]- resultSS[i]
        <<std::endl;
        it+=step;
    }*/
    std::cout << "elapsed time F_S: " << elapsedFS.count() << "\n";
    std::cout << "elapsed time SS: " << elapsedSS.count() << "\n";
}

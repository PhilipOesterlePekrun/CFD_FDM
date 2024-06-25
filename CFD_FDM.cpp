#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

#include <chrono>

#include <limits>
#include <cmath>
#include <numbers>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include "Utilities.h"
#include "Simulation_FDM_ChorinsProj_Viscous_Unsteady.h";
//#include "ISimulation.h"


// data and file parameters
//const string outputDirectory = "Outputs\\"; // relative
//const string dataTxtFile = "Simulation1.txt";
//const string metaTxtFile = deleteInterval(dataTxtFile, dataTxtFile.length() - 4, dataTxtFile.length()) + "META.txt";
//const int memorySpanN = 5; // see samsung notes; minimum value = 2
const size_t UVP_maxRAM = 8*(unsigned long long)(1024 * 1024 * 1024); // maximum total size of U_reduced+V_reduced+P_reduced in Bytes
// simple reduced control:
enum dxdydt
{
    edx,
    edy,
    edt,
    edxANDdy
};
const dxdydt reducedControlPriority =edt; // decide between which is prioritized to be minimized
const double maxReduced_dt = 0.0005; // max value of dt; if reducedControl==edt, then it will be minimized even lower until it is = dt, then the others will be minimized; *GUARANTEES* AT LEAST THIS RESOLUTION
const double maxReduced_dx = 0.1; //
const double maxReduced_dy = 0.1; //

// physical parameters
const double bodyForce[2] ={ 0, -9.81 };
const double density = 998.23;
const double viscosity_mu = 1.002 * pow(10, -3);
const double viscosity_nu=density* viscosity_mu;

// numerical parameters
const int xCount = 80;
const double dx = 0.05;
double* xBounds; // rectangle; implicitly given

const int yCount = 80;
const double dy = 0.05;
double* yBounds; //

const int nCount = 50000; // num of time steps
const double dt = 0.00005;
double t_f;

// iteration parameters
const int iterMax = 100; // max iter steps

// mutable arrays and variables
double U_local[xCount][yCount][2]; // contains U at all nodes at time n and n-1 within loop n
double V_local[xCount][yCount][2]; //
double P_local[xCount][yCount][2]; //
float*** U_reduced; // contains rounded (for now just to float, but further rounding is planned) U at a portion of the nodes at a portion of the time (e.g. i=0, 4, 8, ...; j=0, 4, 8, ...; n=0, 20, 40, ...); this is automatically calculated so the combined size is < UVP_maxRAM
float*** V_reduced; //
float*** P_reduced; //
fieldExtrema pE;

double divAtAPoint[nCount];


// // scenario functions (e.g. BC velocities in time and along boundary or body force in time and position)
double U_onUpperBoundary(double x, double t)
{
    double frequ = 0.5; // Hz
    return max(0.4 * function_step(t - 0.5), -2 * pow(t - 0.5, 2) + 0.4);//0.4f *(2*function_sigmoid(t)-1);//System.Math.Sin(2 * System.Math.PI * frequ*t);
}
double U_onLeftBoundary(double y, double t)
{
    return 0 * max(0.7 * function_step(t - 0.8), -2 * pow(t - 0.8, 2) + 0.7);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
	/*ofstream oF;
    oF.open(outputDirectory+dataTxtFile);
    if (!oF.is_open()) {
        std::cerr << "CERR: Failed to open file "<< outputDirectory + dataTxtFile;
        return 1;
    }
    oF << "-------------- CFD_FDM data file --------------\n\n";
    cout << "Created File " << dataTxtFile << "\n";
    oF.close();*/

    Simulation_FDM_ChorinsProj_Viscous_Unsteady sim1=Simulation_FDM_ChorinsProj_Viscous_Unsteady(0001);
    sim1.printInformation();
    sim1.xCount*=5;
    sim1.yCount*=5;
    sim1.nCount*=2;
    sim1.printInformation();
    sim1.maxReducedFactor_x=10;
    sim1.maxReducedFactor_y=10;
    sim1.maxReducedFactor_t=10;
    //std::cout<<sim1.run()<<"\n";
    //std::cout<<simulationMethodName;
    
    //auto startOfWrite = chrono::high_resolution_clock::now();
    //auto end = chrono::high_resolution_clock::now();
    //std::cout << "Time taken: " << end-startOfWrite<<"\n";

    return 0;
}
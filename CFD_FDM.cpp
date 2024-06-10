#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>

#include <chrono>

#include <limits>
#include <cmath>
#include <numbers>
using namespace std;
#include "Utilities.h"

// data file parameters
string outputDirectory = "Outputs\\"; // relative
string dataTxtFile = "Simulation1.txt";
string metaTxtFile = deleteInterval(dataTxtFile, dataTxtFile.length() - 4, dataTxtFile.length()) + "META.txt";
int memorySpanN = 5; // see samsung notes; minimum value = 2

// physical parameters
double bodyForce[2] ={ 0, -9.81 };
const double density = 998.23;
double viscosity_mu = 1.002 * pow(10, -3);
double viscosity_nu;

// numerical parameters
const int xCount = 30;
const double dx = 0.05;
double* xBounds; // rectangle; implicitly given

const int yCount = 30;
const double dy = 0.05;
double* yBounds; //

const int nCount = 500000; // num of time steps
const double dt = 0.00005;
double t_f;

// iteration parameters
int iterMax = 100; // max iter steps

// global variables
double** U; // U in i, j
double** V; //
double** P; //
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
    auto start = chrono::high_resolution_clock::now();

    ofstream oF;
    oF.open(outputDirectory+dataTxtFile);
    if (!oF.is_open()) {
        std::cerr << "Failed to open file "<< outputDirectory + dataTxtFile;
        return 1;
    }
    oF << "-------------- CFD_FDM data file --------------\n\n";
    cout << "Created File " << dataTxtFile << "\n";

    // grid
    xBounds = new double[] { 0, (xCount - 1)* dx };
    yBounds = new double[] { 0, (yCount - 1)* dy };
    t_f = (nCount - 1) * dt;
    cout << "x length,y length, t_f: "<<(xBounds[1] - xBounds[0])<<"m , "<<(yBounds[1] - yBounds[0])<<"m , "<<t_f<<"s";

    // material properties and constants
    viscosity_nu = density * viscosity_mu;

    // vectors and visuals
    //U = new double[xCount, yCount, nCount]; // x-component of velocity
    //V = new double[xCount, yCount, nCount]; // y-component of velocity
    //P = new double[xCount, yCount, nCount]; // pressure
    //auto startOfWrite = chrono::high_resolution_clock::now();
    int ints[5000];
    for (int i = 1; i < 5000-1; i++) {
        ints[i] = i;
        oF << i << "\n";
    }
    oF << 5000;
    oF.close();
    //auto startOfWrite = chrono::high_resolution_clock::now();
    ifstream iF(outputDirectory + dataTxtFile);
    if (!iF.is_open()) {
        return 1;
    }
    string l;
    getline(iF, l);
    getline(iF, l);
    auto startOfWrite = chrono::high_resolution_clock::now();
    while (getline(iF, l)) {
        int j = stoi(l);
    }
    //for (int i = 1; i < 5000; i++) {
        //int j =;
    //}
    auto end = chrono::high_resolution_clock::now();
    std::cout << "Time taken: " << end-startOfWrite<<"\n";

    return 0;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Openw > Project and select the .sln file

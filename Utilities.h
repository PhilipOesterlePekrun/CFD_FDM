#pragma once

#include <iostream>
#include <limits>
#include <cmath>
#include <numbers>
using namespace std;

struct fieldExtrema
{
    double max = std::numeric_limits<double>::min(); // default access modifier is public for structs btw
    int iMax = -1;
    int jMax = -1;
    int nMax = -1;
    double min = std::numeric_limits<double>::max();
    int iMin = -1;
    int jMin = -1;
    int nMin = -1;

    void print()
    {
        std::cout << "Field Extrema: max " << (*this).max << " @ " << (*this).iMax << ", " << (*this).jMax << ", " << (*this).nMax <<
            " | min " << (*this).min << " @ " << (*this).iMin << ", " << (*this).jMin << ", " << (*this).nMin << "\n";
    }
    void setExtrema(double fieldVal, int i, int j, int n)
    {
        if (fieldVal > this->max)
        {
            this->max = fieldVal;
            this->iMax = i;
            this->jMax = j;
            this->nMax = n;
        }
        if (fieldVal < this->min)
        {
            this->min = fieldVal;
            this->iMin = i;
            this->jMin = j;
            this->nMin = n;
        }
    }
};

// // misc utilities
double function_sigmoid(double x);
double function_step(double x);
double xYToAngle(double x, double y);

// // string utilities
int* checkForIn(string checkFor, string checkIn);
string deleteInterval(string text, int from, int to);
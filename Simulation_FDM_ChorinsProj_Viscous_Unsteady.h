#pragma once
#include "ISimulation.h"
#include "Utilities.h"

class Simulation_FDM_ChorinsProj_Viscous_Unsteady : public ISimulation
{
// // default values if applicable

public:
    size_t UVP_maxRAM = 8 * (unsigned long long)(1024 * 1024 * 1024); // maximum total size of U_reduced+V_reduced+P_reduced in Bytes
// simple reduced control:
    enum dxdydt
    {
        edx,
        edy,
        edt,
        edxANDdy
    };
    dxdydt reducedControlPriority = edt; // decide between which is prioritized to be minimized
    /*double maxReduced_dt = 0.0005; // max value of dt; if reducedControl==edt, then it will be minimized even lower until it is = dt, then the others will be minimized; *GUARANTEES* AT LEAST THIS RESOLUTION
    double maxReduced_dx = 0.1; //
    double maxReduced_dy = 0.1; //*/
private:
    double reduced_dx;
    double reduced_dy;
    double reduced_dt;
public:
    double maxReducedFactor_x=4; // e.g. =4, then max(reduced_dx)=4*dx and min(reduced_xCount)=xCount/4. has to be integer to keep equal spacing
    double maxReducedFactor_y=4; //
    double maxReducedFactor_t=4; //
    // note: minimum reduced_dx is dx

// physical parameters
    double bodyForce[2] = { 0, -9.81 };
    double density = 998.23;
    double viscosity_mu = 1.002 * pow(10, -3);
private:
    double viscosity_nu;//= density * viscosity_mu;

// numerical parameters
public:
    int localTimeScope=4; // depends on the time discretization method; might make it a selection of the method rather than the time in future

public:
    int xCount = 80;
    double dx = 0.05;
private:
    double* xBounds; // rectangle; implicitly given if we assume starts at x=0 and length in +x

public:
    int yCount = 80;
    double dy = 0.05;
private:
    double* yBounds; //

public:
    int nCount = 50000; // num of time steps
    double dt = 0.00005;
private:
    double t_f;

// iteration parameters
public:
    int iterMax = 50; // max iter steps

// mutable arrays and variables
private:
    size_t standardTypeSizeEleU=8; // in Bytes
    double*** U_local;//[xCount] [yCount] [2]; // contains U at all nodes at time n and n-1 within loop n
    double*** V_local;//[xCount] [yCount] [2]; //
    double*** P_local;//[xCount] [yCount] [2]; //
    size_t reducedTypeSizeEleU=4; // in Bytes
    float*** U_reduced; // contains rounded (for now just to float, but further rounding is planned) U at a portion of the nodes at a portion of the time (e.g. i=0, 4, 8, ...; j=0, 4, 8, ...; n=0, 20, 40, ...); this is automatically calculated so the combined size is < UVP_maxRAM
    float*** V_reduced; //
    float*** P_reduced; //
    fieldExtrema pE;

public:
    Simulation_FDM_ChorinsProj_Viscous_Unsteady(int simID);
    bool run() override;
    void printInformation() override;

    //!

private:
    long long int reducedLength(int reducedFactor, int originalLength);

    template <typename T>
    void allocate3DArray(T ****arr, int xCount, int yCount, int zCount) {
        *arr = (T***) malloc(xCount * sizeof(T**));
        if (*arr == NULL) {
            perror("Failed to allocate memory for arr");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < xCount; i++) {
            (*arr)[i] = (T**) malloc(yCount * sizeof(T*));
            if ((*arr)[i] == NULL) {
                perror("Failed to allocate memory for arr[i]");
                exit(EXIT_FAILURE);
            }

            for (int j = 0; j < yCount; j++) {
                (*arr)[i][j] = (T*) malloc(zCount * sizeof(T));
                if ((*arr)[i][j] == NULL) {
                    perror("Failed to allocate memory for arr[i][j]");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    template <typename T>
    void free3DArray(T ***arr, int xCount, int yCount) {
        for (int i = 0; i < xCount; i++) {
            for (int j = 0; j < yCount; j++) {
                free(arr[i][j]); // Free each array of elements
            }
            free(arr[i]); // Free each array of pointers
        }
        free(arr); // Free the array of pointers to pointers
    }

};
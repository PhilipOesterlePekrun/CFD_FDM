#include "Simulation_FDM_ChorinsProj_Viscous_Unsteady.h"

// public:
string Simulation_FDM_ChorinsProj_Viscous_Unsteady::simulationMethodName="Simulation_FDM_ChorinsProj_Viscous_Unsteady";

Simulation_FDM_ChorinsProj_Viscous_Unsteady::Simulation_FDM_ChorinsProj_Viscous_Unsteady(int simID){ // ignore the dumb warning
	this->simulationID=simID;
}

bool Simulation_FDM_ChorinsProj_Viscous_Unsteady::run(){
    // reduced control:
    size_t minReducedArrSizeTotal= 3 * reducedLength(xCount, maxReducedFactor_x) * reducedLength(yCount, maxReducedFactor_y) * reducedLength(nCount, maxReducedFactor_t) * reducedTypeSizeEleU;
    size_t nonReducedArrSizeTotal= 3 * (long long)(xCount) * (long long)(yCount) * (long long)(nCount) * reducedTypeSizeEleU;
    if (minReducedArrSizeTotal >UVP_maxRAM)
    {
        std::cerr << "CERR: Reduced steps are still too large\n\n";
    }
    std::cout << "minReducedArrSizeTotal in RAM: "<< minReducedArrSizeTotal<<" Bytes = "<<(double)(minReducedArrSizeTotal/1024/1024)<<" MB"<<
        " | nonReducedArrSizeTotal in RAM: "<< nonReducedArrSizeTotal<<" Bytes = "<<(double)(nonReducedArrSizeTotal/1024/1024)<<" MB"<<
        " | UVP_maxRAM: "<< UVP_maxRAM<<" Bytes = "<<(double)UVP_maxRAM/1024/1024<<" MB\n\n";
    int reducedFactor_x=1;
    int reducedFactor_y=1;
    int reducedFactor_t=1;
    if (reducedControlPriority == edt)
    {
        if(3 * reducedLength(xCount, maxReducedFactor_x) * reducedLength(yCount, maxReducedFactor_y) * (nCount) * reducedTypeSizeEleU>UVP_maxRAM){ // => reduced_dt strictly between dt and min
            reducedFactor_x=maxReducedFactor_x;
            reducedFactor_y=maxReducedFactor_y;
            reducedFactor_t=(int)std::ceil((3 * reducedLength(xCount, maxReducedFactor_x) * reducedLength(yCount, maxReducedFactor_y) * (nCount) * reducedTypeSizeEleU)/UVP_maxRAM);
        }
        else{
            reducedFactor_t=1;
            // PROBABLY REQUIRED ASSUMPTION THAT xCount==yCount
            reducedFactor_x=std::max(1,(int)std::ceil(std::sqrt((3 * (long long)(xCount) * (long long)(yCount) * (long long)(nCount) * reducedTypeSizeEleU)/(long long)UVP_maxRAM)));
            reducedFactor_y=std::max(1,(int)std::ceil(std::sqrt((3 * (long long)(xCount) * (long long)(yCount) * (long long)(nCount) * reducedTypeSizeEleU)/(long long)UVP_maxRAM)));
        }
    }
    else{
        std::cerr << "(reducedControlPriority != edt) not implemented yet\n\n"; // IMPLEMENT LATER
    }
    reduced_dx=dx*reducedFactor_x;
    reduced_dy=dy*reducedFactor_y;
    reduced_dt=dt*reducedFactor_t;

    size_t reducedArrSizeTotal=3 * reducedLength(xCount, reducedFactor_x) * reducedLength(yCount, reducedFactor_y) * reducedLength(nCount, reducedFactor_t) * reducedTypeSizeEleU;
    std::cout<<"Reduced: reducedFactor_x="<<reducedFactor_x<<", reducedFactor_y="<<reducedFactor_y<<", reducedFactor_t="<<reducedFactor_t<<
        " | reduced_dx="<<reduced_dx<<", reduced_dy="<<reduced_dy<<", reduced_dt="<<reduced_dt<<"\n"<<
        " | Reduced RAM Usage="<<reducedArrSizeTotal<<" Bytes = "<<(double)reducedArrSizeTotal/1024/1024<<" MB\n\n";



    // // // SECTION: SIMULATION ALGORITHMS
    // grid
    xBounds = new double[2] { 0, (xCount - 1)* dx };
    yBounds = new double[2] { 0, (yCount - 1)* dy };
    t_f = (nCount - 1) * dt;
    cout << "Boundary Parameters: x length,y length, t_f: "<<(xBounds[1] - xBounds[0])<<"m , "<<(yBounds[1] - yBounds[0])<<"m , "<<t_f<<"s\n\n";

    allocate3DArray(&U_local, xCount,yCount, localTimeScope);//(double***)malloc(xCount*yCount*2*sizeof(double));
    allocate3DArray(&V_local, xCount,yCount, localTimeScope);
    allocate3DArray(&P_local, xCount,yCount, localTimeScope);

    allocate3DArray(&U_reduced,reducedLength(xCount, reducedFactor_x), reducedLength(yCount, reducedFactor_y), reducedLength(nCount, reducedFactor_t));
    allocate3DArray(&V_reduced,reducedLength(xCount, reducedFactor_x), reducedLength(yCount, reducedFactor_y), reducedLength(nCount, reducedFactor_t));
    allocate3DArray(&P_reduced,reducedLength(xCount, reducedFactor_x), reducedLength(yCount, reducedFactor_y), reducedLength(nCount, reducedFactor_t));

    // // ICs: uniform =0
    double p_0 = 0; // reference pressure @ y=0m
    for (int i = 0; i < xCount; i++)
    {
        for (int j = 0; j < yCount; j++)
        {
            U_local[i][j][0] = 0;//Max(0, -Pow(i -10, 2)-Pow(j - yCount / 2, 2) + 0.8);
            V_local[i][j][0] = 0;
            P_local[i][j][0] = 0;
            //P[i, j, 0] =p_0 + density * bodyForce[1] * (j * dy);
        }
    }
    // pressure ICBCs
    for (int i = 1; i < xCount - 1; i++) // top and bottom impermeable
    {
        P_local[i][0][0] = P_local[i][1][0];
        P_local[i][yCount - 1][0] = P_local[i][yCount - 2][0];
    }
    for (int j = 1; j < yCount - 1; j++) // right and left impermeable
    {
        P_local[0][j][0] = P_local[1][j][0];
        P_local[xCount - 1][j][0] = P_local[xCount - 2][j][0];
    }

    // // time loop
    for (int n = 1; n < nCount; n++)
    {


        // // 1. intermediate velocity
        double** U_star;
        double** V_star;
        allocate2DArray(&U_star, xCount, yCount);
        allocate2DArray(&V_star, xCount, yCount);

        // BCs
        for (int i = 1; i < xCount - 1; i++) // top and bottom
        {
        U_star[i][yCount - 1] = 0;//U_onUpperBoundary(xBounds[0] + i * dx, n * dt);
        V_star[i][yCount - 1] = 0;
        U_star[i][0] = 0;
        V_star[i][0] = 0;
        }
        for (int j = 1; j < yCount - 1; j++) // right and left
        {
        U_star[xCount - 1][j] = 0;//U_onLeftBoundary(xBounds[0] + j* dy, n * dt);
        V_star[xCount - 1][j] = 0;
        U_star[0][j] = 0;//U_onLeftBoundary(xBounds[0] + j * dy, n * dt);
        V_star[0][j] = 0;
        }
        for (int i =1; i < xCount-1; i++)
        {
            for (int j =1; j < yCount-1; j++)
            {
                U_star[i][j] = U_local[i][j][localArrayPosAtN(n-1)] + dt * (-(U_local[i][j][localArrayPosAtN(n-1)] * (U_local[i+1][j][localArrayPosAtN(n-1)] - U_local[i-1][j][localArrayPosAtN(n-1)]) / (2 * dx) +
                    V_local[i][j][localArrayPosAtN(n-1)] * (U_local[i][j+1][localArrayPosAtN(n-1)] - U_local[i][j-1][localArrayPosAtN(n-1)]) / (2 * dy)) + bodyForce[0]);
                //V_star[i, j] = V_local[i][j][localArrayPosAtN(n-1)] + dt * (-(U[i, j, n - 1] * (V[i + 1, j, n - 1] - V[i - 1, j, n - 1]) / (2 * dx) + V[i, j, n - 1] * (V[i, j + 1, n - 1] - V[i, j - 1, n - 1]) / (2 * dy)) + bodyForce[1]);
            }
        }

        // // 2. iterate poisson eq
        // initial pressure guess = pressure from time step n-1
        for (int i = 0; i < xCount; i++)
        {
            for (int j = 0; j < yCount; j++)
            {
                //P[i, j, n] = P[i, j, n - 1];
            }
        }
        // iterate
        double** pNew;
        allocate2DArray(&pNew, xCount, yCount);
        for (int iter = 0; iter < iterMax; iter++)
        {
            for (int i = 1; i < xCount - 1; i++)
            {
                for (int j = 1; j < yCount - 1; j++)
                {
                    //pNew[i, j] = 1 / (2* (dx2 + dy2)) * ((P[i + 1, j, n] + P[i - 1, j, n]) * dy2 + (P[i, j + 1, n] + P[i, j - 1, n]) * dx2 -
                        //dx2 * dy2 * density / dt * ((U_star[i + 1, j] - U_star[i - 1, j]) / (2 * dx) + (V_star[i, j + 1] - V_star[i, j - 1]) / (2 * dy)));
                }
            }

            // pressure BCs
            for (int i = 1; i < xCount - 1; i++) // top and bottom impermeable
            {
                pNew[i, 0] = pNew[i, 1];
                pNew[i, yCount - 1] = pNew[i, yCount - 2];
            }
            for (int j = 1; j < yCount - 1; j++) // right and left impermeable
            {
                pNew[0, j] = pNew[1, j];
                pNew[xCount - 1, j] = pNew[xCount - 2, j];
            }

            for (int i = 0; i < xCount; i++)
            {
                for (int j = 0; j < yCount; j++)
                {
                    //P[i, j, n] = pNew[i, j];
                }
            }
        }

        // // 3. get u^{n+1}
        //BCs
        for (int i = 1; i < xCount - 1; i++) // top and bottom
        {
            //U[i, yCount - 1, n] = U_onUpperBoundary(xBounds[0] + i * dx, n * dt);
            //V[i, yCount - 1, n] = 0;
            //U[i, 0, n] = 0;
            //V[i, 0, n] = 0;
        }
        for (int j = 1; j < yCount - 1; j++) // right and left
        {
            //U[xCount - 1, j, n] = U_onLeftBoundary(xBounds[0] + j * dy, n * dt);
            //V[xCount - 1, j, n] = 0;
            //U[0, j, n] = U_onLeftBoundary(xBounds[0] + j * dy, n * dt);
            //V[0, j, n] = 0;
        }
        for (int i = 1; i < xCount-1; i++)
        {
            for (int j = 1; j < yCount-1; j++)
            {
                //U[i, j, n] = U_star[i, j] - dt / density * (P[i + 1, j, n] - P[i - 1, j, n]) / (2 * dx);
                //V[i, j, n] = V_star[i, j] - dt / density * (P[i, j + 1, n] - P[i, j - 1, n]) / (2 * dy);
            }
        }
        free2DArray(U_star, xCount);
        free2DArray(V_star, xCount);
        free2DArray(pNew, xCount);
    }




    return 1;
}

void Simulation_FDM_ChorinsProj_Viscous_Unsteady::printInformation(){
    std::cout<<"Simulation Information:\n"<<
        "  ID = "<<simulationID<<"\n"<<
        "  Method name = "<<simulationMethodName<<"\n"<<
        "  dx, dy, dt = "<<dx<<", "<<dy<<", "<<dt<<"\n"<<
        "  xCount, yCount, nCount = "<<xCount<<", "<<yCount<<", "<<nCount<<"\n\n";
}

// private:

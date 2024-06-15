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
            // PROBABLY ASSUMED THAT xCount==yCount
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



    // // SECTION: SIMULATION ALGORITHMS
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
long long int Simulation_FDM_ChorinsProj_Viscous_Unsteady::reducedLength(int originalLength, int reducedFactor){
    return std::ceil((originalLength-1)/reducedFactor)+1;
}
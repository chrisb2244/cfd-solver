#include <iostream>

#include "DataStructures/Mesh.h"
#include "DataStructures/Field.tpp"
//#include "CFD-Solver"

using namespace std;

int main()
{
    cout << "Hello World!" << endl;
    MeshScalingType cnst = MeshScalingType::Constant;

    MeshDimension x(10, 0, 1);
    MeshDimension y(5, 0, 2);
    Mesh<1> mesh1(cnst, x);
    Mesh<2> mesh2(cnst, x, y);
    Mesh<3> mesh3(cnst, x, y, MeshDimension(7, 0, 1.5));

    Field<double, 1, 1> fY(mesh1, "fieldY");

}


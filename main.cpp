#include <iostream>
#include <memory>
#include <cmath>

#include "DataStructures/Mesh.h"
#include "DataStructures/Field.tpp"

template<size_t mD>
using vectorField = Field<double, mD, mD>;
template<size_t mD>
using scalarField = Field<double, 1, mD>;

int main()
{
    MeshScalingType cnst = MeshScalingType::Constant;

    MeshDimension x(10, 0, 1);
    MeshDimension y(10, 0, 1);
    auto meshPtr = std::make_shared<Mesh<2>>(cnst, x, y);
    const Mesh<2>& mesh = *meshPtr;

    vectorField<2> U(meshPtr, "U");
    scalarField<2> Rho(meshPtr, "Rho");

    Rho.setFixed(1);

    double d = 1.0;
    constexpr double pi = 3.141592653589793238463;
//    for (size_t i=0; i<mesh.xCells(); i++) {
//        for (size_t j=0; j<mesh.yCells(); j++) {
//            double r = std::pow(std::pow(mesh.x(i)+0.25,2) +
//                                std::pow(mesh.y(j),2),0.5);
//            Rho(i,j) = r <= 12.25 ? std::exp((std::pow(r,2)*-1)/d) : 0;
//            U.x(i,j) =  0.01 * pi * mesh.y(j);
//            U.y(i,j) = -0.01 * pi * mesh.x(i);
//        }
//    }

}


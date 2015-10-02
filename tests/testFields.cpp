#include "DataStructures/Field.tpp"
#include "DataStructures/Mesh.h"
#include "FieldOperations/FieldOperations.h"
#include "catch.hpp"

template<size_t fD, size_t mD>
bool allVals(const Field<double, fD, mD>& f, const double val) {
    for (auto& vec : f.data()) {
        for(auto& v :vec) {
            if (v != Approx(val)) {
                return false;
            }
        }
    }
    return true;
}

TEST_CASE("Mesh tests", "[mesh]") {
    MeshDimension defaultDim(10, 0, 1);
    MeshScalingType s(MeshScalingType::Constant);
    Mesh<1> mesh1D(s, defaultDim);
    Mesh<2> mesh2D(s, defaultDim, defaultDim);
    Mesh<3> mesh3D(s, defaultDim, defaultDim, defaultDim);

    MeshScalingType piv(MeshScalingType::Pivot);
    Mesh<1> pivMesh1(piv, defaultDim);

    SECTION ("Mesh comparison") {
        REQUIRE(mesh1D == mesh1D);
        REQUIRE(mesh1D != mesh2D);

        Mesh<2> tmpMesh(s, defaultDim, MeshDimension(10,0,2));
        REQUIRE(mesh2D != tmpMesh);
        REQUIRE(!mesh2D.isSameMesh(tmpMesh));

        Mesh<1> tmpMesh1(s, defaultDim);
        REQUIRE(mesh1D == tmpMesh1);
        REQUIRE(!mesh1D.isSameMesh(tmpMesh1));

        REQUIRE(mesh1D != pivMesh1);
        REQUIRE(!pivMesh1.isSameMesh(mesh1D));
        REQUIRE(!mesh1D.isSameMesh(pivMesh1));
    }

    SECTION ("Mesh accessors") {
        REQUIRE(mesh1D.xCells() == mesh3D.xCells());
        REQUIRE(mesh1D.xCells() == mesh3D.yCells());
        REQUIRE(mesh2D.xCells() == mesh3D.zCells());
        REQUIRE(mesh1D.scalingType() == mesh2D.scalingType());
        REQUIRE(mesh1D.scalingType() != pivMesh1.scalingType());
        // TODO - Tests for values once operators implemented
    }
}

TEST_CASE("Field tests", "[field]" ) {
    MeshDimension defaultDim(10, 0, 1);
    MeshScalingType s(MeshScalingType::Constant);
    Mesh<1> mesh1D(s, defaultDim);
    Mesh<2> mesh2D(s, defaultDim, defaultDim);

    Field<double, 1, 1> testOneS(mesh1D, "oneD");
    Field<double, 1, 2> testTwoS(mesh2D, "scalarIn2D");
    Field<double, 2, 2> testTwoV(mesh2D, "vectorIn2D");

    SECTION ("Construction") {
        REQUIRE(testOneS.numCells() == 10);

        REQUIRE(testTwoS.numCells() == 100);
        int tTwoS_size = 0;
        for (auto &i : testTwoS.data()) {
            tTwoS_size += i.size();
        }
        REQUIRE(tTwoS_size == 100);

        REQUIRE(testTwoV.numCells() == 100);
        int tTwoV_size = 0;
        for (auto &i : testTwoV.data()) {
            tTwoV_size += i.size();
        }
        REQUIRE(tTwoV_size == 200);
    }

    SECTION ("Assignment") {
        // setFixed
        testOneS.setFixed(4.3);
        REQUIRE(allVals(testOneS, 4.3));
        testTwoV.setFixed(2.51);
        REQUIRE(allVals(testTwoV, 2.51));
        // setZero
        testOneS.setZero();
        REQUIRE(allVals(testOneS, 0));

        SECTION ("Comparison") {
            Field<double, 1, 1> temp1d(mesh1D, "temp1D");
            temp1d.setFixed(2.7);
            testOneS.setFixed(2.7);
            REQUIRE(temp1d == testOneS);
            temp1d.setFixed(2.2);
            REQUIRE(temp1d != testOneS);
        }
    }

    SECTION ("Addition") {
        SECTION ("One Dimensional fields") {
            testOneS.setFixed(2.5);
//            Field<double, 1, 1> temp = testOneS + 2.5;
            Field<double, 1, 1> temp(mesh1D, "temp");
            temp = testOneS;
            REQUIRE(temp.x()[0] == Approx(2.5));
            temp = temp + 2.5;
            REQUIRE(temp.x()[0] == Approx(5.0));
        }
    }
}

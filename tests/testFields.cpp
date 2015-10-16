#include "DataStructures/Field.tpp"
#include "DataStructures/Mesh.h"
#include "FieldOperations/FieldOperations.h"
#include "DataStructures/BoundingBox.h"

#include "catch.hpp"

#include <memory>
#include <iostream>
#include <sstream>

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

template<typename T>
bool matchVectorsApprox(const std::vector<T>& v1, const std::vector<T>& v2) {
    if (v1.size() != v2.size()) {
        return false;
    }
    for (unsigned int i=0; i<v1.size(); i++) {
        if (v1[i] != Approx(v2[i])) {
            return false;
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
    auto mesh1D = std::make_shared<Mesh<1>>(s, defaultDim);
    auto mesh2D = std::make_shared<Mesh<2>>(s, defaultDim, defaultDim);

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

    SECTION ("Individual cell assignment and comparison") {
        testOneS.setFixed(9.15);
        REQUIRE(testOneS.x(4.2) == 9.15);
    }

    SECTION ("Copy and move assignment and construction") {
        SECTION ("Copying") {
            Field<double, 1, 1> copiedField(testOneS);
            REQUIRE(copiedField == testOneS);
            REQUIRE(!copiedField.strictlyEqual(testOneS));

            Field<double, 2, 2> copiedFieldV(testTwoV);
            REQUIRE(copiedFieldV == testTwoV);
            REQUIRE(!copiedFieldV.strictlyEqual(testTwoV));
        }
        SECTION ("Moving") {
            Field<double, 1, 1> copiedField(testOneS);
            Field<double, 1, 1> movedField(std::move(copiedField));
            REQUIRE(movedField == testOneS);

            Field<double, 2, 2> CTV(testTwoV);
            Field<double, 2, 2> movedFieldV(std::move(CTV));
            REQUIRE(movedFieldV == testTwoV);
        }
    }

    SECTION ("Addition and subtraction of a scalar/uniform vector") {
        SECTION ("One Dimensional Fields") {
            testOneS.setFixed(2.5);
            REQUIRE(allVals(testOneS, 2.5));
            testOneS = (testOneS + 2.5);
            REQUIRE(allVals(testOneS,5.0));
            testOneS += 2.5;
            REQUIRE(allVals(testOneS,7.5));
            testOneS -= 3;
            REQUIRE(allVals(testOneS, 4.5));
            testOneS = testOneS - 2.5;
            REQUIRE(allVals(testOneS, 2));
        }

        SECTION ("Two Dimensional Fields") {
            testTwoV.setFixed(2.5);
            REQUIRE(allVals(testTwoV, 2.5));
            testTwoV = (2.5 + testTwoV);
            REQUIRE(allVals(testTwoV,5.0));
            testTwoV += 2.5;
            REQUIRE(allVals(testTwoV,7.5));
            testTwoV -= 3;
            REQUIRE(allVals(testTwoV, 4.5));
            testTwoV = testTwoV - 2.5;
            REQUIRE(allVals(testTwoV, 2));
        }
    }

    SECTION ("Multiplication by a scalar") {
        SECTION ("One Dimensional Fields") {
            testOneS.setFixed(2.5);
            REQUIRE(allVals(testOneS, 2.5));
            testOneS = (testOneS * 2);
            REQUIRE(allVals(testOneS,5.0));
            testOneS *= 2;
            REQUIRE(allVals(testOneS,10));
            testOneS *= (0.5);
            REQUIRE(allVals(testOneS, 5));
            testOneS = 3 * testOneS;
            REQUIRE(allVals(testOneS, 15));
        }

        SECTION ("Two Dimensional Fields") {
            testTwoV.setFixed(2.5);
            REQUIRE(allVals(testTwoV, 2.5));
            testTwoV = (testTwoV * 2);
            REQUIRE(allVals(testTwoV,5.0));
            testTwoV *= 2;
            REQUIRE(allVals(testTwoV,10));
            testTwoV *= (1.0/2);
            REQUIRE(allVals(testTwoV, 5));
            testTwoV = 3 * testTwoV;
            REQUIRE(allVals(testTwoV, 15));
        }
    }
}

TEST_CASE("Bounding boxes", "[bounds]") {
    auto s = MeshScalingType::Constant;
    auto x = MeshDimension(10, 0, 1);
    auto y = MeshDimension(10, -2, 2);
    auto z = MeshDimension(10, 0, 4);

    auto mesh1D = std::make_shared<Mesh<1>>(s, x);
    auto mesh2D = std::make_shared<Mesh<2>>(s, x, y);
    auto mesh3D = std::make_shared<Mesh<3>>(s, x, y, z);

    SECTION("Whole mesh bounding boxes") {
        auto box = mesh1D->bounds();
        std::vector<double> comparisonBounds {0, 1};
        REQUIRE(comparisonBounds == box.bounds());

        auto box2D = mesh2D->bounds();
        std::vector<double> comp2D {0, 1, -2, 2};
        REQUIRE(comp2D == box2D.bounds());
    }

    SECTION("Cell bounding boxes with n=meshDim indices") {
        auto box = mesh1D->bounds(size_t(0));
        std::vector<double> comp1D { 0, 0.1 };
        REQUIRE(comp1D == box.bounds());

        auto box2D = mesh2D->bounds(size_t(0), size_t(1));
        std::vector<double> comp2D { 0, 0.1, -1.6, -1.2 };
        REQUIRE(comp2D == box2D.bounds());

        auto box3D = mesh3D->bounds(size_t(8), size_t(3), size_t(1));
        std::vector<double> comp3D { 0.8, 0.9, -0.8, -0.4, 0.4, 0.8};
        REQUIRE(matchVectorsApprox(comp3D,box3D.bounds()));
    }

    SECTION("Cell bounding boxes with n=1 in multiple dimensions") {
        // 2D
        auto box2D_single_1 = mesh2D->bounds(size_t(0));
        std::vector<double> comp2D_1 { 0, 0.1, -2, -1.6 };
        REQUIRE(comp2D_1 == box2D_single_1.bounds());

        auto box2D_single_2 = mesh2D->bounds(size_t(1));
        std::vector<double> comp2D_2 { 0.1, 0.2, -2, -1.6 };
        REQUIRE(comp2D_2 == box2D_single_2.bounds());

        auto box2D_single_3 = mesh2D->bounds(size_t(10));
        std::vector<double> comp2D_3 { 0, 0.1, -1.6, -1.2 };
        REQUIRE(comp2D_3 == box2D_single_3.bounds());

        // 3D
        auto box3D_single_1 = mesh3D->bounds(size_t(0));
        std::vector<double> comp3D_1 { 0, 0.1, -2, -1.6 , 0, 0.4};
        REQUIRE(comp3D_1 == box3D_single_1.bounds());

        auto box3D_single_2 = mesh3D->bounds(size_t(1));
        std::vector<double> comp3D_2 { 0.1, 0.2, -2, -1.6, 0, 0.4};
        REQUIRE(comp3D_2 == box3D_single_2.bounds());

        auto box3D_single_3 = mesh3D->bounds(size_t(10));
        std::vector<double> comp3D_3 { 0, 0.1, -1.6, -1.2, 0, 0.4};
        REQUIRE(comp3D_3 == box3D_single_3.bounds());

        auto box3D_single_4 = mesh3D->bounds(size_t(100));
        std::vector<double> comp3D_4 { 0, 0.1, -2, -1.6, 0.4, 0.8};
        REQUIRE(comp3D_4 == box3D_single_4.bounds());

        auto box3D_single_5 = mesh3D->bounds(size_t(138));
        std::vector<double> comp3D_5 { 0.8, 0.9, -0.8, -0.4, 0.4, 0.8};
//        REQUIRE(comp3D_5 == box3D_single_5.bounds());
        REQUIRE(matchVectorsApprox(comp3D_5, box3D_single_5.bounds()));

        SECTION("Output to stream") {
            std::ostringstream ss;
            ss << box2D_single_2;
            REQUIRE(ss.str() == "X: [0.1, 0.2], Y: [-2, -1.6]");

            std::ostringstream ss3d;
            ss3d << box3D_single_5;
            REQUIRE(ss3d.str() == "X: [0.8, 0.9], Y: [-0.8, -0.4], Z: [0.4, 0.8]");
        }
    }
}

TEST_CASE("Spacial gradients", "[grad]") {

}

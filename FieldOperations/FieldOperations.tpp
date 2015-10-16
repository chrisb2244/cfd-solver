#include "DataStructures/Field.tpp"
#include "TemplateFunctions.H"
#include <cstddef>

#include "FieldOperations.h"

namespace FieldOps
{
    // Template aliases
    template<size_t mD>
    using vectorField<mD> = Field<double, mD, mD>;
    template<size_t mD>
    using scalarField<mD> = Field<double, 1, mD>;

    // Gradient functions
    template<typename gType, size_t mD,
             EnableIf<gType==gradType::CentralDifferencing>...>
    vectorField<mD>& ddx(const vectorField<mD>& f) {
        const Mesh& mesh = f.mesh();
        bool constSpacing =
                mesh.scalingType() == MeshScalingType::Constant ? true : false;
        if (constSpacing) {

        } else {

        }
        return f;
    }

    template<typename gType, size_t mD,
             EnableIf<gType==gradType::Upwind>...>
    vectorField<mD>& ddx(const vectorField<mD>& f) {
        const Mesh& mesh = f.mesh();
        bool constSpacing =
                mesh.scalingType() == MeshScalingType::Constant ? true : false;

        return f;
    }

    // Divergence of a flux field functions
    template<typename divType, size_t mD,
             EnableIf<divType==divergenceType::Type1>...>
    scalarField<mD>& div(const vectorField<mD>& flux) {

    }
}

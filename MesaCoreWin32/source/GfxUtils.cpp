#include <Mesa/GfxUtils.h>

namespace Mesa
{
    MeshDx11* ModelDx11::GetMesh(const size_t& index)
    {
        if (index < mv_Meshes.size()) return &mv_Meshes[index];
        else return nullptr;
    }
}



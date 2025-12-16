#include "ModelLoader.h"
#include <string>
#include <map>
#include <filesystem>
#include "ModelData.h"
#include "../Material/ModelMaterial.h"
#include "../Mesh/MeshLib.h"
#include "../../../../Third_Party/GEMLoader.h"

ModelLoader::ModelLoader(RenderContext& renderContext) : _renderContext(renderContext) {
    _meshLib = new MeshLibrary(renderContext);
}

ModelLoader::~ModelLoader() {
    for (auto const& pair : _loadedModelCache) {
        delete pair.second;
    }
    _loadedModelCache.clear();
}

MeshLibrary* ModelLoader::meshLib() { return _meshLib; }

ModelData* ModelLoader::loadModel(const ModelLoadArgs& args) {
    const std::string& filePath = args.filePath;

    // Load from cache
    if (_loadedModelCache.count(filePath))
        return _loadedModelCache[filePath];

    // Load from primitive
    if (filePath.starts_with(_prefix))
        return loadPrimitiveModel(args);

    // Load from GEM
    if (_loader.isAnimatedModel(filePath))
        return loadAnimatedGEMModel(args);
    return loadStaticGEMModel(args);
}

ModelData* ModelLoader::loadPrimitiveModel(const ModelLoadArgs& args) {
    std::string primitiveName = args.filePath.substr(_prefix.size());

    ModelData* data = new ModelData();
    Mesh* mesh = &_meshLib->getMesh(primitiveName);
    ModelMaterial* material = new ModelMaterial(
        args.psoName,
        args.shaderName,
        args.cbufferName,
        args.textureName
    );
    material->setUVScale(args.uvScale);

    data->subMeshes.push_back({ mesh, material });

    _loadedModelCache[args.filePath] = data;
    return data;

}

ModelData* ModelLoader::loadStaticGEMModel(const ModelLoadArgs& args) {
    ModelData* data = new ModelData();
    std::vector<GEMLoader::GEMMesh> gemmeshes;

    _loader.load(args.filePath, gemmeshes);

    // ------------ Load Mesh & Textures ------------

    for (auto& gemmesh : gemmeshes) {
        Mesh* mesh = new Mesh();

        // Load Meshes
        std::vector<STATIC_VERTEX> vertices;
        for (auto& gemVertex : gemmesh.verticesStatic) {
            STATIC_VERTEX vertex;
            memcpy(&vertex, &gemVertex, sizeof(STATIC_VERTEX));
            vertices.push_back(vertex);
        }

        mesh->createStatic(_renderContext.device().dxDevice(), _renderContext.uploader(), vertices, gemmesh.indices);

        // Load Texture
        std::string texName = gemmesh.material.find("albedo").getValue();
        std::string fileName = std::filesystem::path(texName).filename().string();
        if (!texName.empty()) {
            std::string fullPath = "Src/Assets/Models/Textures/" + fileName;
            _renderContext.textureManager().loadTexture(
                _renderContext.device().dxDevice(),
                _renderContext.uploader(),
                _renderContext.srvHeap(),
                texName,
                fullPath
            );
        }

        ModelMaterial* material = new ModelMaterial(args.psoName, args.shaderName, args.cbufferName, texName);
        data->subMeshes.push_back({ mesh, material });
    }

    return data;
}

ModelData* ModelLoader::loadAnimatedGEMModel(const ModelLoadArgs& args) {
    ModelData* data = new ModelData();
    std::vector<GEMLoader::GEMMesh> gemmeshes;
    GEMLoader::GEMAnimation gemanimation;

    _loader.load(args.filePath, gemmeshes, gemanimation);

    listAnimationNames(gemanimation);

    // ------------ Load Mesh & Textures ------------

    for (auto& gemmesh : gemmeshes) {
        Mesh* mesh = new Mesh();

        // Load Meshes
        std::vector<ANIMATED_VERTEX> animatedVertices;
        for (auto& gemAnimatedVertex : gemmesh.verticesAnimated) {
            ANIMATED_VERTEX vanimatedVertex;
            memcpy(&vanimatedVertex, &gemAnimatedVertex, sizeof(ANIMATED_VERTEX));
            animatedVertices.push_back(vanimatedVertex);
        }

        mesh->createAnimated(_renderContext.device().dxDevice(), _renderContext.uploader(), animatedVertices, gemmesh.indices);

        // Load Texture
        std::string texName = gemmesh.material.find("albedo").getValue();
        std::string fileName = std::filesystem::path(texName).filename().string();
        if (!texName.empty()) {
            std::string fullPath = "Src/Assets/Models/Textures/" + fileName;
            _renderContext.textureManager().loadTexture(
                _renderContext.device().dxDevice(),
                _renderContext.uploader(),
                _renderContext.srvHeap(),
                texName,
                fullPath
            );
        }

        ModelMaterial* material = new ModelMaterial(args.psoName, args.shaderName, args.cbufferName, texName);
        data->subMeshes.push_back({ mesh, material });
    }

    // ------------ Load Animation ------------
    data->animation = new AnimationData();

    // Load globalInverse
    memcpy(&data->animation->skeleton.globalInverse, &gemanimation.globalInverse, sizeof(Matrix));

    // Load Skeleton & bones
    for (auto& gemBone : gemanimation.bones) {
        Bone bone;
        bone.name = gemBone.name;
        memcpy(&bone.offset, &gemBone.offset, sizeof(Matrix));
        bone.parentIndex = gemBone.parentIndex;
        data->animation->skeleton.bones.push_back(bone);
    }

    // Load animation sequence data
    for (auto& gemAnimation : gemanimation.animations) {
        std::string name = gemAnimation.name;
        AnimationSequence aseq;
        aseq.ticksPerSecond = gemAnimation.ticksPerSecond;
        for (auto& gemFrame : gemAnimation.frames) {
            AnimationFrame frame;
            for (int index = 0; index < gemFrame.positions.size(); index++) {
                Vec3 p;
                memcpy(&p, &gemFrame.positions[index], sizeof(Vec3));
                Quaternion q;
                memcpy(&q, &gemFrame.rotations[index], sizeof(Quaternion));
                Vec3 s;
                memcpy(&s, &gemFrame.scales[index], sizeof(Vec3));

                frame.boneLocalTransforms.push_back(Transform(p, q, s));
            }
            aseq.frames.push_back(frame);
        }
        data->animation->animations.insert({ name, aseq });
    }

    _loadedModelCache.insert({ args.filePath, data });
    return data;
}

void ModelLoader::listAnimationNames(const GEMLoader::GEMAnimation& gemanimation) {
    for (int i = 0; i < gemanimation.animations.size(); i++)
    {
        std::cout << gemanimation.animations[i].name << std::endl;
    }
}
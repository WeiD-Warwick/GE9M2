#pragma once
#include <map>
#include <string>
#include "../Assets/ModelData.h"
#include "ModelMaterial.h"
#include "../RenderContext.h"


class Material;

class MaterialManager {
	std::map<std::string, Material*> materials;

public:
	bool add(const std::string& name, Material* material) {
		auto [it, inserted] = materials.emplace(name, material);
		assert(inserted && "Material already exists");
		return inserted;
	}

	Material* find(const std::string& name) {
		auto it = materials.find(name);
		assert(it != materials.end());
		return it->second;
	}

	ModelMaterial* createInstance(RenderContext& ctx, const SubMesh& subMesh) {

		// Select material from Cache As templete
		ModelMaterial* base =
			static_cast<ModelMaterial*>(find(subMesh.materialKey));

		// One mesh match one material
		ModelMaterial* instance = new ModelMaterial(*base);

		// apply the model texture
		if (!subMesh.albedoTex.empty()) {
			instance->addTexture("albedoTex", subMesh.albedoTex);
		}

		if (!subMesh.normalTex.empty()) {
			instance->addTexture("normalTex", subMesh.normalTex);
		}

		return instance;
	}
};
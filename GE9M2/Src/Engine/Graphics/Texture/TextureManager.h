#pragma once
#include <map>
#include "Texture.h"

class TextureManager {

public:
	std::map<std::string, Texture*> textures;

	Texture* loadTexture(ID3D12Device5* device, DX12Upload& uploader, DX12CBVSRVUAVHeap& srvHeap, const std::string& name, const std::string& file) {
		auto it = textures.find(name);
		if (it != textures.end())
			return it->second;

		Texture* t = new Texture();
		t->init(device, uploader, srvHeap, file);

		textures[name] = t;
		return t;
	}

	int find(const std::string& name) {
		auto it = textures.find(name);
		if (it == textures.end()) return -1;
		return it->second->heapOffset;
	}

	~TextureManager() {
		for (auto const& pair : textures) {
			delete pair.second;
		}
	}
};


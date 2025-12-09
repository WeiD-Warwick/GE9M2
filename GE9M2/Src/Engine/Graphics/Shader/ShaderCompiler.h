#pragma once
#include <string>
#include <d3dcompiler.h>
#include <fstream>
#include <sstream>
#include <wrl/client.h>
#include <filesystem>

using Microsoft::WRL::ComPtr;

class ShaderCompiler {

public:
    ComPtr<ID3DBlob> compileOrLoad(const std::string& hlslPath, const char* entry, const char* profile) {

        //std::string csoPath = hlslPath + "." + entry + ".cso";

        //if (!needsRecompile(hlslPath, csoPath)) {
        //    return loadCSO(csoPath);
        //}

        // auto blob = compile(hlslPath, entry, profile);
        //saveCSO(csoPath, blob);
        return compile(hlslPath, entry, profile);
    }

private:
    /*bool needsRecompile(const std::string& hlsl, const std::string& cso) {
        if (!std::filesystem::exists(cso))
            return true;

        auto tHlsl = std::filesystem::last_write_time(hlsl);
        auto tCso = std::filesystem::last_write_time(cso);

        return tHlsl > tCso;
    }*/

    //ID3DBlob* loadCSO(const std::string& csoPath)
    //{
    //    ComPtr<ID3DBlob> blob;
    //    D3DReadFileToBlob(std::wstring(csoPath.begin(), csoPath.end()).c_str(), blob.GetAddressOf());
    //    return blob.Get();
    //}

    //static void saveCSO(const std::string& path, ComPtr<ID3DBlob> blob) {
    //    D3DWriteBlobToFile(blob.Get(), std::wstring(path.begin(), path.end()).c_str(), false);
    //}

    std::string readShader(const std::string& filename) {
        std::ifstream file(filename);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    ComPtr<ID3DBlob> compile(const std::string& filePath, const char* entry, const char* profile) {
        std::string src = readShader(filePath);
        
        ComPtr<ID3DBlob> shader, error;

        HRESULT hr = D3DCompile(
            src.c_str(),
            strlen(src.c_str()),
            nullptr, nullptr, nullptr,
            entry, profile,
            0, 0,
            &shader, &error
        );

        if (FAILED(hr)) {
            OutputDebugStringA((char*) error->GetBufferPointer());
            return nullptr;
        }
        return shader;
    }
};

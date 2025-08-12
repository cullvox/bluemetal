#include "ResourceProcessor.h"

#include "Graphics/ModelFormat.h"

#include <ofstream>

#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

/**
 * @class ModelProcessor
 * @brief Processes model files into a custom binary format.
 */
class ModelProcessor : public ResourceProcessor
{

public:

    /**
     * @brief Default constructor for ModelProcessor.
     */
    ModelProcessor() = default;

    /**
     * @brief Default destructor for ModelProcessor.
     */
    ~ModelProcessor() override = default;

    /**
     * @brief Processes a model resource file.
     * @param state The processor state containing resources and paths.
     * @param resource The resource file to process.
     * @return True if processing was successful, false otherwise.
     */
    bool Process(ProcessorState& state, ResourceFile& resource) override;

private:
    uint32_t nextNodeIndex = 0; // Current node index.
    const aiScene* scene;
    std::vector<uint32_t> nodeParents; // Indices of the node parents in the model.
    std::vector<std::string> strings; // String table for node names.
    std::ofstream& stream; // Output stream for writing the model data.

    uint32_t GetNodeCount(const aiNode* node);
    void ProcessNodes(uint32_t parent, const aiNode* node);
    void ProcessMeshes();
    uint32_t GetMaterialTextureCount();
};
#include <string>
#include <vector>
#include <iostream>


#include "mesh.h"
//#include "texture.h" je v meshi


class Model {
public:





    // model data
    std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh>    meshes;
    std::string directory;
    bool gammaCorrection;


    glm::vec3 boundMin = glm::vec3 (100000,100000,100000);
    glm::vec3 boundMax = glm::vec3 (-100000,-100000,-100000);


    Model();
     explicit Model(std::string const &path, bool gamma);
    virtual ~Model();
    virtual void Draw(Shader &shader, bool simple);


private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(std::string const &path);
    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene);

    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);

    unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);
};
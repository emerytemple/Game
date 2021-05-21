#include <string>

class Scene {
    public:
        std::string name;
};

class SceneManager {
    public:
        int numScenes;
        Scene scene;
        int maxSize;
        int top;

};
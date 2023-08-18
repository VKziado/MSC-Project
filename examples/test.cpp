#include "appbase.h"

const char *testModelPath = ASSETS_DIR "/models/viking_room/viking_room.obj";
//const char* testModelPath = ASSETS_DIR "/models/sponza/sponza.obj";
//const char *testModelPath = ASSETS_DIR "/models/nanosuit/nanosuit.obj";

class Hello : public Game {
    std::unique_ptr<Scene> scene;
    std::unique_ptr<GUI> gui;

    std::vector<Node *> cubeNodes;

public:
    void loadAssets() {}
    void configScene0() {
        scene = std::make_unique<Scene>();
        scene->environment->setClearColor(0.2, 0.2, 0.2, 1);

        auto lightNode = scene->createLight();

        auto model = ModelManager::getSingleton().createModel(testModelPath);
        model->load();
        auto node = scene->addModel(model);
        node->getComponent<Transform>()->yaw(glm::radians(-90.f))->pitch(glm::radians(-90.f));

        scene->prepare();

        gui = std::make_unique<GUI>(this);
        gui->renderGUISignal.Connect(std::bind(&Hello::renderGUI, this));

        RenderServer::getSingleton().postProcessType = POST_PROCESS_SSAO;
        RenderServer::getSingleton().showGrid = false;
    }
    void renderGUI() {
        bool flag = false;
        auto &&aoParam = PostProcessSSAO::getSingleton().params;
        auto &&shadingParam = PostProcessSSAO::getSingleton().shadingParams;

        ImGui::Begin("settings");
        ImGui::SetWindowFontScale(2.f);
        {
            static const char *items[]{
                "color",
                "ao",
                "color+ao",
            };
            if (ImGui::BeginCombo("shading mode", items[shadingParam.shadingMode])) {
                for (int i = 0; i < std::size(items); ++i) {
                    const bool is_selected = (shadingParam.shadingMode == i);
                    if (ImGui::Selectable(items[i], is_selected)) {
                        shadingParam.shadingMode = i;
                        PostProcessSSAO::getSingleton().updateShadingParams();
                    }
                }
                ImGui::EndCombo();
            }
        }
        {
            static const char *items[]{
                "GTAOUniform",  // HBAO
                "GTAOCosine",   // GTAO
            };
            if (ImGui::BeginCombo("ao mode", items[aoParam.aoType])) {
                for (int i = 0; i < std::size(items); ++i) {
                    const bool is_selected = (aoParam.aoType == i);
                    if (ImGui::Selectable(items[i], is_selected)) {
                        aoParam.aoType = i;
                        flag = true;
                    }
                }
                ImGui::EndCombo();
            }
        }
        flag |= ImGui::InputInt("sampleStep", &aoParam.sampleStep);
        flag |= ImGui::InputInt("sampleStepNum", &aoParam.sampleStepNum);
        flag |= ImGui::InputInt("sampleDirections", &aoParam.sampleDirections);
        flag |= ImGui::SliderFloat("maxRadius0", &aoParam.maxRadius0, 0, aoParam.maxRadius1);
        flag |= ImGui::SliderFloat("maxRadius1", &aoParam.maxRadius1, aoParam.maxRadius0, 100);

        ImGui::End();

        if (flag) PostProcessSSAO::getSingleton().updateAOParams();
    }

    void init() override {
        loadAssets();
        configScene0();
    }

    void render(float dt) override {
        scene->update(dt);
        RenderServer::getSingleton().renderScene(scene.get());
        gui->render(dt);
    }
};

int main(int argc, const char **argv) {
    try {
        AppBase app;
        app.run<Hello>();

    } catch (std::exception const &e) {
        std::cout << e.what() << std::endl;
    }
}
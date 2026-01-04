```cpp
/*  
    First Iteration of the Hex UI Library.

    This is a windows only retained ui library that uses modern opengl(4.6)

    Any images/masks required for the ui are currently loaded once 
    during initializtion so will folders will need to be created with resource inside
*/

#include "src/opengl/context.hpp"
#include "src/opengl/shader.hpp"
#include "src/opengl/texture.hpp"
#include "src/opengl/buffer.hpp"

#include "src/ui/util/canvas.hpp"
#include "src/ui/util/backend.hpp"
#include "src/ui/layout.hpp"
#include "src/ui/label.hpp"
#include "src/ui/combobox.hpp"
#include "src/ui/scroll.hpp"
#include "src/ui/menu.hpp"
#include "src/ui/tab.hpp"
#include "src/ui/list.hpp"
#include "src/ui/checkbox.hpp"
#include "src/ui/splitter.hpp"
#include "src/ui/tree.hpp"
#include "src/ui/slider.hpp"

class UiRenderer {
    std::unique_ptr<ui::Canvas> canvas;
    std::unique_ptr<gl::Program> programInstanced;
    std::unique_ptr<gl::Texture2D> imageTexture;
    std::unique_ptr<gl::Texture2D> maskTexture;
    std::unique_ptr<gl::Texture2D> fontTexture;

    gl::Context* context;
public:
    UiRenderer(std::string const& asset_str, gl::Context* context) : context(context) {
        canvas = std::make_unique<ui::Canvas>(asset_str, "C:/Windows/Fonts/Calibri.ttf", 48);

        std::map<GLenum, std::string> uiSource = {
            {GL_VERTEX_SHADER, load_file_source(asset_str + "shaders/ui/ui.vert")},
            {GL_GEOMETRY_SHADER, load_file_source(asset_str + "shaders/ui/ui.geom")},
            {GL_FRAGMENT_SHADER, load_file_source(asset_str + "shaders/ui/ui.frag")},
        };

        programInstanced = std::make_unique<gl::Program>(uiSource);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        imageTexture = std::make_unique<gl::Texture2D>(canvas->imageAtlas.w_, canvas->imageAtlas.h_, canvas->imageAtlas.c_, canvas->imageAtlas.data_);
        maskTexture = std::make_unique<gl::Texture2D>(canvas->maskAtlas.w_, canvas->maskAtlas.h_, canvas->maskAtlas.c_, canvas->maskAtlas.data_);
        fontTexture = std::make_unique<gl::Texture2D>(uf::gAtlas.w(), uf::gAtlas.h(), 1, uf::gAtlas.pixels());

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void render(ui::Canvas* canvas, int ww, int wh, HDC hdc) {
        context->makeCurrent(hdc);

        glViewport(0, 0, ww, wh);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto [objects, data] = canvas->data();

        if (!objects.empty()) {
            std::reverse(objects.begin(), objects.end());
            programInstanced->use();
            programInstanced->SetUniform("u_resolution", ww, wh);
            programInstanced->SetUniform("u_imageDim", imageTexture->w, imageTexture->h);
            programInstanced->SetUniform("u_maskDim", maskTexture->w, maskTexture->h);
            programInstanced->SetUniform("u_fontDim", fontTexture->w, fontTexture->h);

            glBindTextureUnit(0, maskTexture->id);
            glBindTextureUnit(1, fontTexture->id);
            glBindTextureUnit(2, imageTexture->id);
            glBindTextureUnit(3, 0);

            gl::sbuffer<ui::RenderObject> buffer(objects);
            buffer.bind_base(0);

            gl::sbuffer<int> data_buffer(data);
            data_buffer.bind_base(1);

            glDrawArraysInstanced(GL_POINTS, 0, 1, objects.size());
        }

        canvas->clear();

        SwapBuffers(hdc);
    }

private:
    std::string load_file_source(std::string const& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }
        std::ostringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
};

int main()
{
    ui::Native native(-1, 0, 0, 0, 0, ui::fPersistent);

    std::unique_ptr<gl::Context> context = std::make_unique<gl::Context>(native.hdc());
    context->makeCurrent(native.hdc());

    gl::load_extensions();
    gl::SetDebugMode(true);

    std::string asset_str = "C:/Users/Tom/source/repos/repositorys/hexui_v1/hexui_v1/assets/";

    std::unique_ptr<UiRenderer> uRenderer = std::make_unique<UiRenderer>(asset_str, context.get());
    std::unique_ptr<ui::Backend> uBackend = std::make_unique<ui::Backend>(asset_str, "C:/Windows/Fonts/Calibri.ttf", 58);
    uBackend->setProcessDpiAware();

    std::unique_ptr<ui::VLayout> widget = std::make_unique<ui::VLayout>();
    widget->makeWindow(50, 50, 800, 600);
    widget->window()->show();

    auto menuLayout = widget->addChild<ui::HLayout>();
    menuLayout->setFixedH(50);

    auto fileMenu = menuLayout->addChild<ui::Menu>("File", widget->id());

    auto newItem = fileMenu->addItem("New", [](std::string text) { std::cout << "New" << std::endl; });
    auto openItem = fileMenu->addItem("Open", [](std::string text) { std::cout << "Open" << std::endl; });
    auto saveItem = fileMenu->addItem("Save", [](std::string text) { std::cout << "Save" << std::endl; });
    auto saveAsMenu = fileMenu->addSubmenu("Save As");
    saveAsMenu->addItem("PNG", [](std::string text) { std::cout << "Save As PNG" << std::endl; });
    saveAsMenu->addItem("JPG", [](std::string text) { std::cout << "Save As JPG" << std::endl; });

    auto editMenu = menuLayout->addChild<ui::Menu>("Edit", widget->id());
    auto viewMenu = menuLayout->addChild<ui::Menu>("View", widget->id());

    auto slider = widget->addChild<ui::Slider>([](float v) { std::cout << v << std::endl; });
    slider->setFixedH(50);

    auto tab = widget->addChild<ui::Tab>();
    tab->addTab<ui::Label>("Tab 1", "This is the tab 1 content");
    tab->addTab<ui::Label>("Tab 2", "This is the tab 2 content");

    tab->addTab<ui::List>("List Tab", std::vector<std::string>{ "Banana", "Apple", "Orange" }, [](std::string const& s) {
        std::cout << "Selected: " << s << std::endl;
    });

    auto cbg = tab->addTab<ui::CheckboxGroup>("Checkbox", std::vector<std::string>{ "Option 1", "option 2", "option 3" }, [](bool state) {
        std::cout << "Checkbox state: " << state << std::endl;
    });

    auto tree = tab->addTab<ui::Tree>("Tree", "Root");
    tree->addItem("Root", "Child 1");
    tree->addItem("Root", "Child 2");
    tree->addItem("Child 2", "Child 4");
    tree->addItem("Root", "Child 3");

    auto s1 = tab->addTab<ui::VSplitter>("Splitter");
    s1->setLeading<ui::BoxWidget>();
    s1->setTrailing<ui::BoxWidget>();

    cbg->setMultiSelect(true);

    while (uBackend->update(0.0)) {
        for (auto& [canvas, ww, wh, hdc] : uBackend->extractCanvases()) {
            uRenderer->render(canvas, ww, wh, hdc);
        }
    }

    return -1;
}
```

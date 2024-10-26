#include <iostream>
#include <string>
#include <vector>
#include <ciso646>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <nothofagus.h>
#include <sol/sol.hpp>
#include <argparse/argparse.hpp>

/* Convenience struct to represent a Texture in the scripting world */
struct TextureScript
{
    std::size_t width = 1, height = 1;
    std::vector<std::uint8_t> pixels = { 0 };
    std::vector<float> palette = { 1.0f, 1.0f, 1.0f, 1.0f };

    static void addUserType(sol::state& lua)
    {
        lua.new_usertype<TextureScript>("Texture",
            sol::constructors<TextureScript()>(),
            "width", sol::property(&TextureScript::width),
            "height", sol::property(&TextureScript::height),
            "pixels", sol::property(&TextureScript::pixels),
            "palette", sol::property(&TextureScript::palette)
        );
    }

    Nothofagus::Texture toCpp() const
    {
        Nothofagus::Texture out({ width, height }, { 1.0f, 1.0f, 1.0f, 1.0f });

        if (palette.size() % 4 != 0)
            throw;

        Nothofagus::ColorPallete colorPalette(1, { 1.0f, 1.0f, 1.0f, 1.0f });

        std:size_t numberOfColors = palette.size() / 4;
        colorPalette.colors.resize(numberOfColors);

        for (std::size_t i = 0; i < palette.size(); i += 4)
        {
            glm::vec4 color(
                palette[i],
                palette[i + 1],
                palette[i + 2],
                palette[i + 3]
            );
            colorPalette.colors.at(i / 4) = color;
        }
        out.setPallete(colorPalette);

        out.setPixels(pixels.begin(), pixels.end());

        return out;
    }
};

/* Convenience struct to represent a Bellota in the scripting world */
struct BellotaScript
{
    float x = 0.0f, y = 0.0f, scaleX = 1.0f, scaleY = 1.0f, scale = 1.0f, angle = 0.0f;
    unsigned int texture = 0;
    int depthOffset = 0;
    bool visible = true;

    static void addUserType(sol::state& lua)
    {
        lua.new_usertype<BellotaScript>("Bellota",
            sol::constructors<BellotaScript()>(),
            "x", sol::property(&BellotaScript::x),
            "y", sol::property(&BellotaScript::y),
            "scaleX", sol::property(&BellotaScript::scaleX),
            "scaleY", sol::property(&BellotaScript::scaleY),
            "scale", sol::property(&BellotaScript::scale),
            "angle", sol::property(&BellotaScript::angle),
            "texture", sol::property(&BellotaScript::texture),
            "depthOffset", sol::property(&BellotaScript::depthOffset),
            "visible", sol::property(&BellotaScript::visible)
        );
    }

    Nothofagus::Bellota toCpp() const
    {
        Nothofagus::Transform transform
        {
            {x, y},
            {scale * scaleX, scale * scaleY},
            angle
        };

        Nothofagus::Bellota bellota(transform, { texture }, depthOffset);
        bellota.visible() = visible;

        return bellota;
    }
};

int main(int argc, char *argv[])
{
    spdlog::info("Welcome to Nothofagus Lua Demo App!");

    argparse::ArgumentParser program("loading_lua_file");

    std::string inputFilename;

    program.add_argument("-i", "--input")
        .help("file name of the lua file to process")
        .required()
        .store_into(inputFilename);

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        spdlog::error("Invalid arguments: ", err.what());
        std::cerr << program;
        return 1;
    }

    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math, sol::lib::table);
    TextureScript::addUserType(lua);
    BellotaScript::addUserType(lua);

    try
	{
		lua.safe_script_file(inputFilename);
		spdlog::info("Lua script loaded successfully");
	}
	catch (const sol::error& e)
	{
		spdlog::error("Error while processing the lua file: ", e.what());
		return 0;
	}

    auto bellotas = lua["bellotas"].get<std::vector<BellotaScript>>();
    auto textures = lua["textures"].get<std::vector<TextureScript>>();

    Nothofagus::ScreenSize screenSize{150, 100};
    Nothofagus::Canvas canvas(screenSize, "Nothofagus Lua Demo", {0.7, 0.7, 0.7}, 6);

    std::vector<Nothofagus::BellotaId> bellotaIds;
    bellotaIds.reserve(bellotas.size());

    for (auto& bellotaScript : bellotas)
    {
        Nothofagus::Bellota bellota = bellotaScript.toCpp();
        Nothofagus::BellotaId bellotaId = canvas.addBellota(bellota);
        bellotaIds.push_back(bellotaId);
    }

    std::vector<Nothofagus::TextureId> textureIds;
    textureIds.reserve(textures.size());

    for (auto& textureScript : textures)
    {
        Nothofagus::Texture texture = textureScript.toCpp();
        Nothofagus::TextureId textureId = canvas.addTexture(texture);
        textureIds.push_back(textureId);
    }

    float time = 0.0f;
    constexpr float angularSpeed = 0.1;
    constexpr float growingSpeed = 0.1;
    constexpr float horizontalSpeed = 0.1;
    bool rotate = true;
    bool leftKeyPressed = false;
    bool rightKeyPressed = false;

    auto update = [&](float dt)
    {
        time += dt;

        Nothofagus::Bellota& bellota = canvas.bellota(bellotaIds[0]);

        ImGui::Begin("Hello there!");
        ImGui::Text("Discrete control keys: W, S, ESCAPE");
        ImGui::Text("Continuous control keys: A, D");
        ImGui::End();

        if (rotate)
            bellota.transform().angle() += angularSpeed * dt;

        if (leftKeyPressed)
            bellota.transform().location().x -= horizontalSpeed * dt;

        if (rightKeyPressed)
            bellota.transform().location().x += horizontalSpeed * dt;

        bellota.transform().location().x = std::clamp<float>(bellota.transform().location().x, 10, screenSize.width-10);
    };

    Nothofagus::Controller controller;
    controller.registerAction({Nothofagus::Key::W, Nothofagus::DiscreteTrigger::Press}, [&]()
    {
        canvas.bellota(bellotaIds[0]).transform().location().y += 10.0f;
    });
    controller.registerAction({Nothofagus::Key::S, Nothofagus::DiscreteTrigger::Press}, [&]()
    {
        canvas.bellota(bellotaIds[0]).transform().location().y -= 10.0f;
    });
    controller.registerAction({Nothofagus::Key::A, Nothofagus::DiscreteTrigger::Press}, [&]()
    {
        leftKeyPressed = true;
    });
    controller.registerAction({Nothofagus::Key::A, Nothofagus::DiscreteTrigger::Release}, [&]()
    {
        leftKeyPressed = false;
    });
    controller.registerAction({Nothofagus::Key::D, Nothofagus::DiscreteTrigger::Press}, [&]()
    {
        rightKeyPressed = true;
    });
    controller.registerAction({Nothofagus::Key::D, Nothofagus::DiscreteTrigger::Release}, [&]()
    {
        rightKeyPressed = false;
    });
    controller.registerAction({Nothofagus::Key::SPACE, Nothofagus::DiscreteTrigger::Press}, [&]()
    {
        rotate = not rotate;
    });
    controller.registerAction({Nothofagus::Key::ESCAPE, Nothofagus::DiscreteTrigger::Press}, [&]() { canvas.close(); });
    
    canvas.run(update, controller);
    
    return 0;
}
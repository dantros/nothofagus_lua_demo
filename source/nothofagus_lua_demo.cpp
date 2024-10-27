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
            "x", sol::property(&BellotaScript::x, &BellotaScript::x),
            "y", sol::property(&BellotaScript::y, &BellotaScript::y),
            "scaleX", sol::property(&BellotaScript::scaleX, &BellotaScript::scaleX),
            "scaleY", sol::property(&BellotaScript::scaleY, &BellotaScript::scaleY),
            "scale", sol::property(&BellotaScript::scale, &BellotaScript::scale),
            "angle", sol::property(&BellotaScript::angle, &BellotaScript::angle),
            "texture", sol::property(&BellotaScript::texture, &BellotaScript::texture),
            "depthOffset", sol::property(&BellotaScript::depthOffset, &BellotaScript::depthOffset),
            "visible", sol::property(&BellotaScript::visible, &BellotaScript::visible)
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

    static BellotaScript fromCpp(const Nothofagus::Bellota& bellota)
    {
        BellotaScript bellotaScript;
        bellotaScript.x = bellota.transform().location().x;
        bellotaScript.y = bellota.transform().location().y;
        bellotaScript.scaleX = bellota.transform().scale().x;
        bellotaScript.scaleY = bellota.transform().scale().y;
        bellotaScript.angle = bellota.transform().angle();
        bellotaScript.texture = bellota.texture().id;
        bellotaScript.depthOffset = bellota.depthOffset();
        bellotaScript.visible = bellota.visible();

        return bellotaScript;
    }

    void modify(const Nothofagus::Bellota& bellota)
    {
        x = bellota.transform().location().x;
        y = bellota.transform().location().y;
        scaleX = bellota.transform().scale().x;
        scaleY = bellota.transform().scale().y;
        angle = bellota.transform().angle();
        //texture = bellota.texture().id; // texture cannot be updated during runtime
        depthOffset = bellota.depthOffset();
        visible = bellota.visible();
    }

    static void modify(const BellotaScript& bellotaScript, Nothofagus::Bellota& bellota)
    {
        bellota.transform().location().x = bellotaScript.x;
        bellota.transform().location().y = bellotaScript.y;
        bellota.transform().scale().x = bellotaScript.scaleX * bellotaScript.scale;
        bellota.transform().scale().y = bellotaScript.scaleY * bellotaScript.scale;
        bellota.transform().angle() = bellotaScript.angle;
        //bellota.texture().id = bellotaScript.texture // texture cannot be updated during runtime
        bellota.depthOffset() = bellotaScript.depthOffset;
        bellota.visible() = bellotaScript.visible;
    }
};

int main(int argc, char *argv[])
{
    const std::string appName = "Nothofagus Lua Demo";
    spdlog::info("Welcome to ", appName, "!");

    argparse::ArgumentParser program("loading_lua_file");

    std::string initFilename = "";
    std::string updateFilename = "";

    program.add_argument("-i", "--init")
        .help("lua file to initialize our canvas")
        .required()
        .store_into(initFilename);
    program.add_argument("-u", "--update")
        .help("lua file to update some bellotas")
        .store_into(updateFilename);

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        spdlog::error("Invalid arguments: ", err.what());
        std::cerr << program;
        return 1;
    }

    Nothofagus::ScreenSize screenSize{ 150, 100 };
    Nothofagus::Canvas canvas(screenSize, appName, { 0.7, 0.7, 0.7 }, 6);

    std::vector<Nothofagus::TextureId> textureIds;
    std::vector<Nothofagus::BellotaId> bellotaIds;
    
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math, sol::lib::table);
    TextureScript::addUserType(lua);
    BellotaScript::addUserType(lua);

    try
    {
        lua.safe_script_file(initFilename);
        spdlog::info("Initialization lua script loaded successfully");
    }
    catch (const sol::error& e)
    {
        spdlog::error("Error while processing the lua file: ", e.what());
        return 0;
    }

    std::vector<BellotaScript> bellotas = lua["bellotas"].get<std::vector<BellotaScript>>();
    std::vector<TextureScript> textures = lua["textures"].get<std::vector<TextureScript>>();

    lua["bellotas"] = &bellotas;
    lua["textures"] = &textures;

    textureIds.reserve(textures.size());

    for (auto& textureScript : textures)
    {
        Nothofagus::Texture texture = textureScript.toCpp();
        Nothofagus::TextureId textureId = canvas.addTexture(texture);
        textureIds.push_back(textureId);
    }

    bellotaIds.reserve(bellotas.size());

    for (auto& bellotaScript : bellotas)
    {
        Nothofagus::Bellota bellota = bellotaScript.toCpp();
        Nothofagus::BellotaId bellotaId = canvas.addBellota(bellota);
        bellotaIds.push_back(bellotaId);
    }

    std::function<void(float)> scriptUpdate = [](float dt){};

    if (updateFilename != "")
    {
        try
        {
            lua.safe_script_file(updateFilename);
        }
        catch (const sol::error& e)
        {
            spdlog::error("Error while processing the lua file: ", e.what());
            return 0;
        }
    }
    else
    {
        spdlog::info("No update lua script has been provided.");
    }

    constexpr float horizontalSpeed = 0.1;
    bool leftKeyPressed = false;
    bool rightKeyPressed = false;

    auto update = [&](float dt)
    {
        BellotaScript& playerBellotaScript = bellotas.at(bellotaIds[0].id);

        ImGui::Begin("Hello there!");
        ImGui::Text("Discrete control keys: W, S, ESCAPE");
        ImGui::Text("Continuous control keys: A, D");
        ImGui::End();

        if (leftKeyPressed)
            playerBellotaScript.x -= horizontalSpeed * dt;

        if (rightKeyPressed)
            playerBellotaScript.x += horizontalSpeed * dt;

        playerBellotaScript.x = std::clamp<float>(playerBellotaScript.x, 10, screenSize.width-10);

        sol::function luaUpdate = lua["update"];
        luaUpdate(dt);

        // updating all bellotas from script world to native world
        for (std::size_t i = 0 ; i < bellotas.size() ; ++i)
        {
            BellotaScript& bellotaScript = bellotas.at(i);
            Nothofagus::Bellota& bellota = canvas.bellota({ i });
            BellotaScript::modify(bellotaScript, bellota);
        }
    };

    Nothofagus::Controller controller;
    controller.registerAction({Nothofagus::Key::W, Nothofagus::DiscreteTrigger::Press}, [&]()
    {
        bellotas.at(0).y += 10.0f;
    });
    controller.registerAction({Nothofagus::Key::S, Nothofagus::DiscreteTrigger::Press}, [&]()
    {
            bellotas.at(0).y -= 10.0f;
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
    controller.registerAction({Nothofagus::Key::ESCAPE, Nothofagus::DiscreteTrigger::Press}, [&]() { canvas.close(); });
    
    canvas.run(update, controller);
    
    return 0;
}
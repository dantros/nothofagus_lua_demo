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

/* Convenience struct to represent a bellota in the scripting world */
struct BellotaScript
{
    // transform
    float x = 0.0f, y = 0.0f, scaleX = 1.0f, scaleY = 1.0f, scale = 1.0f, angle = 0.0f;

    // texture
    std::string texture = "";

    int depthOffset = 0;
    bool visible = true;
};

Nothofagus::Bellota convert(const BellotaScript& bellotaScript)
{
    Nothofagus::Transform transform
    {
        {bellotaScript.x, bellotaScript.y},
        {bellotaScript.scale * bellotaScript.scaleX, bellotaScript.scale * bellotaScript.scaleY},
        bellotaScript.angle
    };
    return {transform, {0}};
}

int main(int argc, char *argv[])
{
    // You can directly use spdlog to ease your logging
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
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math, sol::lib::table);

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

    BellotaScript bellota1 = lua["bellota1"].get<BellotaScript>();

    ///////
    
    Nothofagus::Bellota bellotaX = convert(bellota1);

    ///////

    Nothofagus::ScreenSize screenSize{150, 100};
    Nothofagus::Canvas canvas(screenSize, "Demo App", {0.7, 0.7, 0.7}, 6);

    Nothofagus::ColorPallete pallete{
        {0.0, 0.0, 0.0, 0.0},
        {0.0, 0.4, 0.0, 1.0},
        {0.2, 0.8, 0.2, 1.0},
        {0.5, 1.0, 0.5, 1.0},
    };
    
    Nothofagus::Texture texture({8, 8}, {0.5, 0.5, 0.5, 1.0});
    texture.setPallete(pallete)
        .setPixels(
        {
            2,1,3,0,0,3,2,1,
            2,1,1,0,0,0,2,1,
            2,1,1,1,0,0,2,1,
            2,1,2,1,1,0,2,1,
            2,1,0,2,1,1,2,1,
            2,1,0,0,2,1,2,1,
            2,1,0,0,0,2,2,1,
            2,1,3,0,0,3,2,1,
        }
    );
    Nothofagus::TextureId textureId = canvas.addTexture(texture);

    Nothofagus::BellotaId bellotaId = canvas.addBellota(bellotaX);

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

        Nothofagus::Bellota& bellota = canvas.bellota(bellotaId);
        float scale = 2.0f + 0.5f * std::sin(0.005f * time);
        bellota.transform().scale() = glm::vec2(scale, scale);

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
        canvas.bellota(bellotaId).transform().location().y += 10.0f;
    });
    controller.registerAction({Nothofagus::Key::S, Nothofagus::DiscreteTrigger::Press}, [&]()
    {
        canvas.bellota(bellotaId).transform().location().y -= 10.0f;
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
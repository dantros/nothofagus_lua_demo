
-- using some convenience script-scoped-only lua code
palletes = {}
palletes["primary"] = {
    {0.0, 0.0, 0.0, 1.0},
    {1.0, 0.0, 0.0, 1.0},
    {0.0, 1.0, 0.0, 1.0},
    {0.0, 0.0, 1.0, 1.0},
    {1.0, 1.0, 0.0, 1.0},
    {0.0, 1.0, 1.0, 1.0},
    {1.0, 1.0, 1.0, 1.0},
    {1.0, 1.0, 1.0, 0.0}
}
palletes["green"] = {
    {0.0, 0.0, 0.0, 0.0},
    {0.0, 0.4, 0.0, 1.0},
    {0.2, 0.8, 0.2, 1.0},
    {0.5, 1.0, 0.5, 1.0}
}
palletes["red"] = {
    {0.0, 0.0, 0.0, 0.0},
    {0.4, 0.0, 0.0, 1.0},
    {0.8, 0.2, 0.2, 1.0},
    {1.0, 0.5, 0.5, 1.0}
}

pixels = {}
pixels["N"] = {
    2,1,3,0,0,3,2,1,
    2,1,1,0,0,0,2,1,
    2,1,1,1,0,0,2,1,
    2,1,2,1,1,0,2,1,
    2,1,0,2,1,1,2,1,
    2,1,0,0,2,1,2,1,
    2,1,0,0,0,2,2,1,
    2,1,3,0,0,3,2,1
}

-- defining textures
textures = {}
textures["colors"] = {
    palette = palletes["primary"],
    width = 4,
    height = 4,
    pixels = {
        0,1,2,3,
        4,5,6,7,
        0,1,2,3,
        4,5,6,7
    }
}
textures["greenN"] = {
    palette = palletes["green"],
    width = 8,
    height = 8,
    pixels = pixels["N"]
}
textures["redN"] = {
    palette = palletes["red"],
    width = 8,
    height = 8,
    pixels = pixels["N"]
}

bellota1 = Bellota.new()
bellota1.x = 10.0
bellota1.y = 10.0
bellota1.scaleX = 2.0
bellota1.texture = "colors"

-- These are the actual bellotas that will populate our world
-- bellotas = {}
-- bellotas["colors1"] = bellota1
-- 
-- bellotas["colors2"] = {
--     x = 10.0,
--     y = 20.0,
--     texture = "colors"
-- }
-- bellotas["greenN"] = {
--     x = 50.0,
--     y = 50.0,
--     scale = 4.0,
--     texture = "greenN"
-- }
-- bellotas["redN"] = {
--     x = 30.0,
--     y = 50.0,
--     angle = 45.0,
--     texture = "redN"
-- }
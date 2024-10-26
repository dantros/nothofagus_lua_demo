
-- using some convenience script-scoped-only lua code
palettes = {}
palettes["primary"] = {
    0.0, 0.0, 0.0, 1.0,
    1.0, 0.0, 0.0, 1.0,
    0.0, 1.0, 0.0, 1.0,
    0.0, 0.0, 1.0, 1.0,
    1.0, 1.0, 0.0, 1.0,
    0.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 0.0
}
palettes["green"] = {
    0.0, 0.0, 0.0, 0.0,
    0.0, 0.4, 0.0, 1.0,
    0.2, 0.8, 0.2, 1.0,
    0.5, 1.0, 0.5, 1.0
}
palettes["red"] = {
    0.0, 0.0, 0.0, 0.0,
    0.4, 0.0, 0.0, 1.0,
    0.8, 0.2, 0.2, 1.0,
    1.0, 0.5, 0.5, 1.0
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
pixels["O"] = {
    2,1,1,1,1,1,1,1,
    2,1,2,2,2,2,2,1,
    2,1,0,0,0,0,2,1,
    2,1,0,0,0,0,2,1,
    2,1,0,0,0,0,2,1,
    2,1,0,0,0,0,2,1,
    2,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,0
}

-- Creating specific textures
textureColors = Texture.new()
textureColors.palette = {
    0.0, 0.0, 0.0, 1.0,
    1.0, 0.0, 0.0, 1.0,
    0.0, 1.0, 0.0, 1.0,
    0.0, 0.0, 1.0, 1.0,
    1.0, 1.0, 0.0, 1.0,
    0.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 0.0
}
textureColors.width = 4
textureColors.height = 4
textureColors.pixels = {
    0,1,2,3,
    4,5,6,7,
    0,1,2,3,
    4,5,6,7
}

-- We can also re-use poreviously defined values for convenience
textureGreenN = Texture.new()
textureGreenN.palette = palettes["green"]
textureGreenN.width = 8
textureGreenN.height = 8
textureGreenN.pixels = pixels["N"]

textureRedN = Texture.new()
textureRedN.palette = palettes["red"]
textureRedN.width = 8
textureRedN.height = 8
textureRedN.pixels = pixels["N"]

-- These are the textures that will be availables
textures = {textureColors, textureGreenN, textureRedN}

bellotaColors = Bellota.new()
bellotaColors.x = 75.0
bellotaColors.y = 50.0
bellotaColors.scale = 2.0
bellotaColors.texture = 0
bellotaColors.depthOffset = 0

bellotaGreenN = Bellota.new()
bellotaGreenN.x = 50.0
bellotaGreenN.y = 50.0
bellotaGreenN.scaleX = 3.0
bellotaGreenN.scaleY = 2.0
bellotaGreenN.texture = 1
bellotaGreenN.depthOffset = 1

bellotaRedN = Bellota.new()
bellotaRedN.x = 100.0
bellotaRedN.y = 50.0
bellotaRedN.scaleX = 2.0
bellotaRedN.scaleY = 3.0
bellotaRedN.texture = 2
bellotaRedN.depthOffset = -1

-- These are the bellotas that will populate our world
bellotas = {bellotaColors, bellotaGreenN, bellotaRedN}

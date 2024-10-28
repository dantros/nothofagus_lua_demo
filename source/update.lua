


function horizontalDistance(lhs, rhs)
    return lhs - rhs
end

function direction(target, origin)
    if target > origin then
        return 1
    else
        return -1
    end
end

time = 0
speed = 0.1
dir = 1
function update(dt)
    time = time + dt
    local followerBellota = bellotas[2]
    followerBellota.x = followerBellota.x + dir * speed * dt

    if followerBellota.x >= 180 and dir > 0 then
        dir = -1
    end
    if followerBellota.x < 10 and dir < 0 then
        dir = 1
    end

    local playerBellota = bellotas[1]

    print(time, playerBellota.x, followerBellota.x)
    --print(dt)
    -- print(testFn(5))
    -- local playerBellota = bellotas[1]
    -- local followerBellota = bellotas[2]
    -- 
    -- print(playerBellota.x, " ", followerBellota.x)
-- 
    -- local speed = 0.01
    -- local threshold = 10
    -- if horizontalDistance(playerBellota.x, followerBellota.x) > threshold then
    --     dir = direction(playerBellota.x, followerBellota.x)
    --     followerBellota.x = followerBellota.x + (dir * speed * dt)
    -- end
    -- print("after if")
-- 
    -- bellotas[2] = followerBellota
-- 
    -- print(" ", followerBellota.x)
    --print(bellota1.x)
    --print("-")
    --updateBellota(bellota1, 1)
end
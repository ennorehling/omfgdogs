local frames = {}
local song
local autoplay = true
local mute = false

function add_frames(image, frames, width, height)
    local y = 0
    local ww = image:getWidth()
    local hh = image:getHeight()
    while y <= hh - height do
        local x = 0
        while x <= ww - width do
            local frame = {}
            frame.quad = love.graphics.newQuad(x, 0, width, height, ww, hh)
            frame.image = image
            table.insert(frames, frame)
            x = x + width
        end
        y = y + height
    end
end

function love.load()
    local width = 384
    local height = 80
    local image
    image = love.graphics.newImage("spritesheet_left.png")
    add_frames(image, frames, width, height)
    image = love.graphics.newImage("spritesheet_right.png")
    add_frames(image, frames, width, height)
    song = love.audio.newSource("omfgdogs.mp3", "stream")
    song:play()
end

local speed = 40 -- ms per frame
local frame = 1
local delta = 0
function advance(dt)
    delta = delta + math.floor(dt * 1000)
    if delta > speed then
        local skip = math.floor(delta / speed)
        delta = delta % speed
        frame = frame + skip
        if frame > #frames then frame = (frame - 1) % #frames + 1 end
    end
end

function love.keypressed(key, scancode, isrepeat)
    if key == "escape" then
        love.event.quit()
    elseif key == "space" then
        if not isrepeat then
            autoplay = not autoplay
        end
    elseif key == "m" then
        if not isrepeat then
            mute = not mute
            if mute then song:pause() else song:play() end
        end
    elseif key == "home" then
        autoplay = false
        frame = 1
    elseif key == "end" then
        autoplay = false
        frame = #frames
    elseif key == "left" then
        autoplay = false
        frame = frame - 1
        if frame == 0 then frame = #frames end
    elseif key == "right" then
        autoplay = false
        frame = frame + 1
        if frame > #frames then frame = 1 end
    end
end

function love.update(dt)
    if autoplay then advance(dt) end
end

function love.draw()
    local hh = love.graphics.getHeight()
    local ww = love.graphics.getWidth()
    local image = frames[frame].image
    local quad = frames[frame].quad
    local vx, vy, vw, vh = quad:getViewport()
    local y = 0
    while y < hh do
        local x = 0
        while x < ww do
            love.graphics.draw(image, quad, x, y)
            x = x + vw
        end
        y = y + vh
    end
    love.graphics.print("frame " .. frame .. " delta " .. delta, 0, 0)
end

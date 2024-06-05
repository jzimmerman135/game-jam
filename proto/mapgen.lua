json = require("lib/json")

screen_width = 800
screen_height = 600
screen_height_25 = screen_height * 0.25
screen_height_50 = screen_height * 0.50
tube_width = 100
square_size = 64
square_ycenter = screen_height_50 - square_size*0.5

rect_types = {
    none = 0,
    death = 1,
    platform = 2,
    toggle = 3,
    red = 4,
    blue = 5,
}

function rect(p)
    return {
        ypos = p.ypos or 0,
        width = p.width or tube_width,
        height = p.height or screen_height_25,
        move = p.move or tube_width * 3,
        leftpad = p.leftpad or 0,
        types = p.types or rect_types.death,
    }
end

function upper_tube(length)
    return rect {
        ypos = 0,
        height = length,
    }
end

function lower_tube(length)
    return rect {
        ypos = screen_height - length,
        height = length,
    }
end

function square(ypos)
    return rect {
        ypos = ypos,
        height = square_size,
        width = square_size,
    }
end

function toggle(ypos)
    local r = square(ypos)
    r.types = rect_types.toggle

    return r
end

function platform(ypos, length)
    return rect {
        ypos = ypos,
        width = length,
        height = square_size,
        move = length + tube_width * 3,
        types = rect_types.platform,
    }
end

function from_bottom(amount)
    return screen_height - screen_height*amount
end

function barrier()
    return rect {
        ypos = 0,
        height = screen_height,
    }
end

function add_rect(map, rect)
    table.insert(map.rects, rect)
end


function lvl1()
    local map = {
        rects = {
        }
    }

    t = upper_tube(screen_height_50 * 0.8)
    t.leftpad = screen_width * 0.5;
    add_rect(map, t)
    t = lower_tube(screen_height_50)
    t.move = tube_width*1.3;
    add_rect(map, t)

    t = platform(from_bottom(0.5), tube_width*4)
    t.move = tube_width*4
    add_rect(map, t)

    t = platform(from_bottom(0.25), tube_width*3)
    t.move = tube_width*3
    add_rect(map, t)

    t = platform(from_bottom(0.1), tube_width*3)
    t.move = tube_width*3.5
    add_rect(map, t)

    t = toggle(from_bottom(0.3))
    t.move = tube_width*2
    add_rect(map, t)

    t = barrier()
    t.types = rect_types.blue
    add_rect(map, t)

    fp = io.open("proto/maps/01.json", "w")
    fp:write(json.encode(map))
    fp:close()
end

function lvl2()
    local map = {
        rects = {
        }
    }

    local leftpad = screen_width * 0.5
    local squeeze = 0.6

    local tubes = {
        {upper_tube, screen_height_50 * 0.5, tube_width * 2},
        {lower_tube, screen_height_50, tube_width * 2},

        {upper_tube, screen_height_50*(squeeze - 0.3), 0},
        {lower_tube, screen_height_50*(squeeze + 0.3), tube_width * 3},

        {upper_tube, screen_height_50*(squeeze - 0.3), 0},
        {lower_tube, screen_height_50*(squeeze + 0.4), tube_width * 3},

        {upper_tube, screen_height_50*(squeeze + 0.5), 0},
        {lower_tube, screen_height_50*(squeeze - 0.5), tube_width * 3},

        {upper_tube, screen_height_50*(squeeze + 0.7), 0},
        {lower_tube, screen_height_50*(squeeze - 0.7), tube_width * 3},

        {upper_tube, screen_height_50*(squeeze - 0.1), 0},
        {lower_tube, screen_height_50*(squeeze + 0.1), tube_width * 3},

        {upper_tube, screen_height_50*(squeeze - 0.2), 0},
        {lower_tube, screen_height_50*(squeeze + 0.2), tube_width * 3},

        {upper_tube, screen_height_50*(squeeze - 0.3), 0},
        {lower_tube, screen_height_50*(squeeze + 0.3), tube_width * 3},

        {upper_tube, screen_height_50*(squeeze - 0.1), 0},
        {lower_tube, screen_height_50*(squeeze + 0.1), tube_width * 3},

        {upper_tube, screen_height_50*(squeeze - 0.4), 0},
        {lower_tube, screen_height_50*(squeeze + 0.4), tube_width * 3},

        {upper_tube, screen_height_50*(squeeze - 0.5), 0},
        {lower_tube, screen_height_50*(squeeze + 0.5), tube_width * 3},

        {upper_tube, screen_height_50*(squeeze - 0.3), 0},
        {lower_tube, screen_height_50*(squeeze + 0.3), tube_width * 3},
    }

    -- get total length

    total_len = 0
    for _, tb in pairs(tubes) do
        total_len = total_len + tb[2]
    end

    -- add barriers

    total_len = total_len + leftpad

    local top_barrier = rect {
        ypos = -screen_height_25 - 100,
        width = total_len,
        height = screen_height_25,
        move = 0
    }

    local bottom_barrier = rect {
        ypos = screen_height,
        width = total_len,
        height = screen_height_25,
        move = 0
    }

    add_rect(map, top_barrier)
    add_rect(map, bottom_barrier)

    for pos, tb in pairs(tubes) do
        local tubefun = tb[1]
        local tubelen = tb[2]
        local tubemv = tb[3]
        local t = tubefun(tubelen)
        if (pos == 1) then
            t.leftpad = leftpad
        end

        t.move = tubemv
        add_rect(map, t)
    end
    -- t = upper_tube(screen_height_50 * 0.8)
    -- t.leftpad = screen_width * 0.5;
    -- add_rect(map, t)
    -- t = lower_tube(screen_height_50)
    -- t.move = tube_width*1.3;
    -- add_rect(map, t)

    local fp = io.open("proto/maps/02.json", "w")
    fp:write(json.encode(map))
    fp:close()
end

lvl1()
lvl2()

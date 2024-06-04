json = require("proto/lib/json")
map = {
    rects = {
        {
            ypos = 0,
            width = 100,
            height =  200,
            move = 150,
            leftpad = 200
        },
        {
            ypos = 250,
            width = 100,
            height = 200,
            move = 300,
            leftpad = 200
        },
        {
            ypos = 161,
            width = 64,
            height = 64,
            move = 100,
            leftpad = 200
        }
    }
}

fp = io.open("proto/maps/01.json", "w")
fp:write(json.encode(map))
fp:close()

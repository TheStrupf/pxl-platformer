local sprite = app.activeSprite

-- Check constraints
if sprite == nil then
	app.alert("No Sprite...")
  	return
end

if sprite.colorMode ~= ColorMode.INDEXED then
	app.alert("Sprite needs to be indexed")
	return
end

local function writeIndexData(img, x, y, w, h)
	local res = ""
	for y = 0,h-1 do
		res = ""
		for x = 0, w-1 do
			px = img:getPixel(x, y) - 1
			res = res .. string.format("%02x", px & 0xff)
		end
		io.write(res)
	end
end

local function exportFrame(frm)
	if frm == nil then
		frm = 1
	end
	io.write("{\"width\":" .. sprite.width .. ",\n");
	io.write("\"height\":" .. sprite.height .. ",\n");
	io.write("\"data\":\"");
	local img = Image(sprite.spec)
	img:drawSprite(sprite, frm)
	writeIndexData(img, x, y, sprite.width, sprite.height)
	io.write("\"\n}");
end

local fname = sprite.filename;
fname = fname:gsub(".aseprite", ".json")
fname = fname:gsub(".ase", ".json")

local dlg = Dialog()
dlg:file{
	id="exportFile",
        label="File",
        title="Indexed Export",
		filename=fname,
        open=false,
        save=true,
        --filename= p .. fn .. "pip",
        filetypes={"json"}}
dlg:check{
	id="onlyCurrentFrame",
        text="Only current frame",
        selected=true }

dlg:button{ id="ok", text="OK" }
dlg:button{ id="cancel", text="Cancel" }
dlg:show()

local data = dlg.data
if data.ok then
	local f = io.open(data.exportFile, "w")
	io.output(f)

	if data.onlyCurrentFrame then
		exportFrame(app.activeFrame)
	else
	 	for i = 1,#sprite.frames do
	 		io.write(string.format(";Frame %d\n", i))
	 		exportFrame(i)
		end
	end
	io.close(f)
end
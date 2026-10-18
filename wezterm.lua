-- Pull in the wezterm API
local wezterm = require("wezterm")

-- This will hold the configuration.
local config = wezterm.config_builder()

-- This is where you actually apply your config choices
-- config.color_scheme = "Tokyo Night"
-- config.color_scheme = 'Monokai Dark (Gogh)'
-- config.color_scheme = "Kanagawa (Gogh)"
config.color_scheme = "Gruvbox Dark (Gogh)"
 --config.color_scheme = 'Solarized Dark - Patched'
-- config.color_scheme = "Catppuccin Mocha"
-- config.color_scheme = 'Dracula'
config.colors = {
	--	foreground = "#CBE0F0",
	--	background = "#011423",
	-- background = "#011219",
	--	cursor_bg = "#47FF9C",
	--	cursor_border = "#47FF9C",
	--	cursor_fg = "#011423",
	--	selection_bg = "#033259",
	--	selection_fg = "#CBE0F0",
	--	ansi = { "#214969", "#E52E2E", "#44FFB1", "#FFE073", "#0FC5ED", "#a277ff", "#24EAF7", "#24EAF7" }

	--	brights = { "#214969", "#E52E2E", "#44FFB1", "#FFE073", "#A277FF", "#a277ff", "#24EAF7", "#24EAF7" },
}
config.max_fps = 120
-- config.font = wezterm.font("CaskaydiaMono Nerd Font")
--config.font = wezterm.font("BlexMono Nerd Font")
config.font = wezterm.font("Geist Mono")
config.font_size = 19

config.enable_tab_bar = false

config.window_decorations = "RESIZE"
config.window_background_opacity = 0.6
-- config.macos_window_background_blur = 10
config.wsl_domains = {
	{
		name = "WSL:Ubuntu", -- this string identifies the domain
		distribution = "Ubuntu", -- this must match the output of `wsl -l -v`
	},
}
config.adjust_window_size_when_changing_font_size = false
-- Set the default domain to the WSL one
config.default_domain = "WSL:Ubuntu"
config.default_cwd = "~"
config.win32_system_backdrop = 'Acrylic'
-- config.default_domain = "powershell.exe"
--config.default_prog = { "wsl.exe", "-d", "Ubuntu", "-e", "bash", "-l" }
-- or something like { "bash", "-l" } if inside the domain.
config.line_height = 1.0
config.window_padding = {
	left = 0,
	right = 0,
	top = 0,
	bottom = 0,
}
-- -- optional
-- config.default_cwd = "~"
--
-- and finally, return the configuration to wezterm
return config

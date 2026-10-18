local M = {}

M.base_30 = {
  white = "#262626",        -- Main text color
  darker_black = "#f4f4f4", -- UI Elements / Statusline / Sidebar
  black = "#ffffff",        -- Main Background
  black2 = "#f4f4f4",       -- Floating windows
  one_bg = "#ecf5ff",       -- Cursorline / Highlight
  one_bg2 = "#e5e5e5",      -- Statusline NC
  one_bg3 = "#cdcdcd",      -- Fold Column
  grey = "#8a99a6",         -- Comments
  grey_fg = "#8a99a6",
  grey_fg2 = "#5c6873",
  light_grey = "#cdcdcd", -- Line Numbers
  red = "#d12f1b",        -- Errors
  baby_pink = "#ad3da4",
  pink = "#ad3da4",
  line = "#e5e5e5",   -- Split lines
  green = "#2d8504",  -- Git Add / Success
  vibrant_green = "#2d8504",
  blue = "#0f68a0",   -- Functions
  nord_blue = "#0b4f79",
  yellow = "#78492a", -- Warning / Brownish in Xcode
  sun = "#fef869",    -- Search highlight background
  purple = "#804fb8",
  dark_purple = "#4b21b0",
  teal = "#3e8087",
  orange = "#272ad8", -- Used for Numbers (Strong Blue) to pop
  cyan = "#69a7fc",
  statusline_bg = "#f4f4f4",
  lightbg = "#ecf5ff",
  pmenu_bg = "#f4f4f4",
  folder_bg = "#0f68a0",
}

M.base_16 = {
  base00 = "#ffffff", -- Background
  base01 = "#f4f4f4", -- Lighter Background (Statusline)
  base02 = "#ecf5ff", -- Selection Background
  base03 = "#8a99a6", -- Comments
  base04 = "#5c6873", -- Darker Grey
  base05 = "#262626", -- Main Foreground
  base06 = "#20202A",
  base07 = "#000000",
  base08 = "#d12f1b", -- Variables / Red
  base09 = "#272ad8", -- Integers (Strong Blue in Xcode)
  base0A = "#4b21b0", -- Classes (Dark Purple)
  base0B = "#d12f1b", -- Strings (Xcode uses Red for strings, so we map Red here)
  base0C = "#3e8087", -- Regex / Special (Teal)
  base0D = "#0f68a0", -- Functions (Blue)
  base0E = "#ad3da4", -- Keywords (Pink)
  base0F = "#78492a", -- Deprecated / PreProc (Brown)
}

M.type = "light"

M = require("base46").override_theme(M, "xcodelight")

return M

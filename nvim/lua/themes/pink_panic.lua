local M = {}

M.base_30 = {
  white = "#4B0648",        -- Normal Text FG
  darker_black = "#FCD0E3", -- Sidebar/Float BG (Slightly darker than normal BG)
  black = "#FFDBEB",        -- Normal BG (hsl 330, 100, 93)
  black2 = "#F2C6D9",       -- Pmenu/lighter UI elements
  one_bg = "#FAD6E5",       -- Statusline/Tabline bg
  one_bg2 = "#F0BCCF",
  one_bg3 = "#E6B2C5",

  grey = "#B4AEE8", -- Comments/Grey text (Diagnostic Info)
  grey_fg = "#9E9E9E",
  grey_fg2 = "#8E8E8E",
  light_grey = "#A2A2A3",

  red = "#FF5301",       -- Diagnostic Error
  baby_pink = "#FF70B5", -- DiffDelete
  pink = "#E90C7A",      -- Statement
  line = "#E8B0C6",      -- Split lines

  green = "#11A19C",     -- MatchParen / String-ish
  vibrant_green = "#7ECA9C",
  nord_blue = "#428BAB",
  blue = "#E90CDE",   -- Type
  yellow = "#FFC23D", -- Diagnostic Hint
  sun = "#DEA95F",
  purple = "#E90C7A", -- Keywords
  dark_purple = "#8E79B9",
  teal = "#11A19C",
  orange = "#FF5301",
  cyan = "#0B8EC6",

  statusline_bg = "#FAD6E5",
  lightbg = "#FCD0E3",
  pmenu_bg = "#F2C6D9",
  folder_bg = "#E90CDE",
}

M.base_16 = {
  base00 = "#FFDBEB", -- BG
  base01 = "#FAD6E5", -- Lighter BG
  base02 = "#F2C6D9", -- Selection BG
  base03 = "#E8B0C6", -- Comments/Line Numbers
  base04 = "#D19EB4", -- Darker Comments
  base05 = "#4B0648", -- FG (Normal)
  base06 = "#F8F8F8",
  base07 = "#090A0B",

  base08 = "#A9331B", -- Variables / Visual FG (Red-ish)
  base09 = "#FF5301", -- Numbers / Constants (Orange)
  base0A = "#E90CDE", -- Classes / Types (Magenta)
  base0B = "#11A19C", -- Strings (Teal)
  base0C = "#0B8EC6", -- Escape/Regex (Cyan)
  base0D = "#4B0648", -- Functions (Using FG or slightly lighter purple)
  base0E = "#E90C7A", -- Keywords (Hot Pink)
  base0F = "#B4AEE8", -- Deprecated / Info
}

M.type = "light"

M.polish_hl = {
  -- Specific overrides based on your Lush specs

  Normal = { bg = M.base_30.black, fg = M.base_30.white },
  CursorLine = { bg = "#FFE6F2" }, -- Slightly lighter/different than BG

  -- Search & Match
  Search = { fg = "#380436", bg = "#DDA0DD" }, -- Light Purple BG
  IncSearch = { fg = "#380436", bg = "#DDA0DD" },
  MatchParen = { fg = "#CBF7F6", bg = "#11A19C" },

  -- Visual Mode (Red Text on Pink BG)
  Visual = { fg = "#A9331B", bg = "#FD7096" },

  -- Line Numbers
  CursorLineNr = { fg = "#8E79B9", bold = true },
  LineNr = { fg = "#D19EB4" },

  -- Splits
  VertSplit = { fg = M.base_30.line, bg = M.base_30.black },
  WinSeparator = { fg = M.base_30.line, bg = M.base_30.black },

  -- Syntax Overrides
  Statement = { fg = "#E90C7A" },
  Type = { fg = "#E90CDE" },
  PreProc = { fg = "#D47ACC" },

  -- UI Elements
  Pmenu = { bg = M.base_30.pmenu_bg },
  PmenuSel = { bg = "#FD7096", fg = "#A9331B" },

  -- Treesitter
  ["@punctuation.bracket"] = { fg = M.base_30.white },
  ["@variable"] = { fg = M.base_30.white },
  ["@keyword"] = { fg = "#E90C7A" },
  ["@string"] = { fg = "#11A19C" },
}

return M

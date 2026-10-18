-- This file needs to have same structure as nvconfig.lua
-- https://github.com/NvChad/ui/blob/v3.0/lua/nvconfig.lua
-- Please read that file to know all available options :(

---@type ChadrcConfig

local M = {}


M.base46 = {
  transparency = false,
  theme = "everblush",
  -- hl_override = {
  --   Comment = { italic = true },
  --   ["@comment"] = { italic = true },
  -- },
}

M.ui = {
  cmp = {
    lspkind_text = true,
    style = "default", -- default/flat_light/flat_dark/atom/atom_colored
    format_colors = {
      lsp = true,
    },
  },

  telescope = { style = "borderless" }, -- borderless / bordered

  -- statusline = {
  -- 	separator_style = "default", -- default/round/block/arrow
  -- 	order = nil,
  -- 	modules = nil,
  -- },
  statusline = {
    separator_style = "block", -- can be "round", "block", "arrow", etc.
    -- order = { "mode", "file", "git", "%=", "lsp_msg", "%=", "diagnostics", "lsp", "cwd", "cursor" },
    order = { "mode", "file", "diagnostics", "git", "%=", "lsp_msg", "%=", },
    modules = {
      mode = function()
        local utils = require("nvchad.stl.utils")
        if not utils.is_activewin() then
          return ""
        end

        local modes = utils.modes
        local separators = utils.separators[require("nvconfig").ui.statusline.separator_style or "default"]
        local sep_r = separators.right

        local m = vim.api.nvim_get_mode().mode
        local mode_data = modes[m] or { "", "Normal" }

        local hl = mode_data[2] .. "Mode"
        local hl_sep = mode_data[2] .. "ModeSep"

        -- local icon = "󰀘" -- Change to your preferred logo/icon

        local icon = ""
        return "%#St_"
            .. hl
            .. "#"
            .. " "
            .. icon
            .. " "
            .. "%#St_"
            .. hl_sep
            .. "#"
            .. sep_r
            .. "%#ST_EmptySpace#"
            .. sep_r
      end,
    },
  },
  tabufline = {
    enabled = true,
    lazyload = true,
    order = { "treeOffset", "buffers", "tabs" },
    -- order = { "treeOffset", "buffers", "tabs", "btns" },
    modules = nil,
    bufwidth = 21,
  },
}

M.nvdash = {
  load_on_startup = true,
  header = {
    "                            ",
    "     ▄▄         ▄ ▄▄▄▄▄▄▄   ",
    "   ▄▀███▄     ▄██ █████▀    ",
    "   ██▄▀███▄   ███           ",
    "   ███  ▀███▄ ███           ",
    "   ███    ▀██ ███           ",
    "   ███      ▀ ███           ",
    "   ▀██ █████▄▀█▀▄██████▄    ",
    "     ▀ ▀▀▀▀▀▀▀ ▀▀▀▀▀▀▀▀▀▀   ",
    "                            ",
    "     Powered By  eovim    ",
    "                            ",
  },
}

M.term = {
  base46_colors = true,
  winopts = { number = false },
  sizes = {
    sp = 0.3,
    vsp = 0.2,
    ["bo sp"] = 0.3,
    ["bo vsp"] = 0.2,
  },
  float = {
    relative = "editor",
    row = 0.3,
    col = 0.25,
    width = 0.5,
    height = 0.4,
    border = "single",
  },
}

M.lsp = {
  signature = false,
}

M.cheatsheet = {
  excluded_groups = { "terminal (t)", "autopairs", "Nvim", "Opens" },
}

M.mason = {
  pkgs = {},
  skip = {},
}

M.colorify = {
  enabled = true,
  mode = "virtual", -- fg, bg, virtual
  virt_text = "󱓻 ",
  highlight = {
    hex = true,
    lspvars = true,
  },
}

return M

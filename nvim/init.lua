vim.g.base46_cache = vim.fn.stdpath("data") .. "/base46/"
vim.g.mapleader = " "
-- require("lspconfig").clangd.setup {}
-- bootstrap lazy and all plugins
local lazypath = vim.fn.stdpath("data") .. "/lazy/lazy.nvim"

if not vim.uv.fs_stat(lazypath) then
    local repo = "https://github.com/folke/lazy.nvim.git"
    vim.fn.system({ "git", "clone", "--filter=blob:none", repo, "--branch=stable", lazypath })
end

vim.opt.rtp:prepend(lazypath)

local lazy_config = require("configs.lazy")

vim.env.CC = "C:\\msys64\\ucrt64\\bin\\gcc.exe"
-- load plugins
require("lazy").setup({
    {
        "NvChad/NvChad",
        lazy = false,
        branch = "v2.5",
        import = "nvchad.plugins",
    },

    { import = "plugins" },
    -- { import = "nvchad.blink.lazyspec" },
}, lazy_config)

-- load theme
dofile(vim.g.base46_cache .. "defaults")
dofile(vim.g.base46_cache .. "statusline")

require("options")
require("run")
require("nvchad.autocmds")

vim.schedule(function()
    require("mappings")
end)
vim.opt.shell = "powershell.exe"
if vim.g.neovide then
    -- Put anything you want to happen only in Neovide here
    -- vim.o.guifont = "CaskaydiaMono Nerd Font:h14"
    vim.o.guifont = "Iosevka Nerd Font :h14"

    vim.g.neovide_floating_shadow = false

    -- vim.g.neovide_fullscreen = true
    vim.g.neovide_window_borderless = true
end
-- if vim.g.neovide then
--   -- Set Title Bar Background to White
--   vim.g.neovide_title_background_color = "#1a1b26"
--
--   -- Set Title Bar Text to Black (so it is visible on white)
--   vim.g.neovide_title_text_color = "#000000"
-- end
-- vim.opt.laststatus = 0
vim.api.nvim_create_autocmd("VimEnter", {
    once = true,
    callback = function()
        vim.fn.chdir([[C:\Users\abird\personal\CanIDoIt]])
    end,
})

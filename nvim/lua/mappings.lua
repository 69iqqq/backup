require "nvchad.mappings"

-- add yours here

-- Keymaps are automatically loaded on the VeryLazy event
-- Default keymaps that are always set: https://github.com/LazyVim/LazyVim/blob/main/lua/lazyvim/config/keymaps.lua
-- Add any additional keymaps here
-- add yours here

-- local map = vim.keymap.set
--
-- map("i", "jk", "<ESC>")

-- local discipline = require("craftzdog.discipline")
--
-- discipline.cowboy()
--
local keymap = vim.keymap
local opts = { noremap = true, silent = true }
keymap.set("n", ";", ":", { desc = "CMD enter command mode" })
-- keymap.set("n", "-", "<CMD>Oil<CR>", { desc = "Open parent directory" })
keymap.set("n", "cd", ":cd %:p:h<CR>", { noremap = true, silent = true })
keymap.set("i", "jk", "<Esc>", {})
keymap.set("v", "kj", "<Esc>", {})
keymap.set("t", "jk", "<C-\\><C-n>", {})
-- Do things without affecting the registers
-- Run cpp code in terminal
--Save using <C-s>
keymap.set("n", "<C-s>", ":w<CR>", opts)
keymap.set("i", "<C-s>", "<Esc>:w<CR>", opts)
keymap.set("v", "<C-s>", "<Esc>:w<CR>gv", opts)
--Select all using <C-a>
keymap.set("n", "<C-t>", "ggVG", opts)
keymap.set("i", "<C-t>", "<Esc>ggVG", opts)
--To quit using <C-q>
keymap.set("n", "<C-q>", ":q!<CR>", opts)
keymap.set("i", "<C-q>", "<Esc>:q!<CR>", opts)
-- Increment/decrement
-- keymap.set('n', '+', '<C-a>')
-- keymap.set('n', '-', '<C-x>')
-- Keymap to close the current buffer
keymap.set("n", "<leader>q", ":bdelete<CR>", opts)

keymap.set("n", "<Leader>pv", ':echo "Hello"<CR>', opts)
-- Delete a word backwards
keymap.set("n", "dw", 'vb"_d')
-- Select all
keymap.set("n", "<C-a>", "gg<S-v>G")

-- Save with root permission (not working for now)
--vim.api.nvim_create_user_command('W', 'w !sudo tee > /dev/null %', {})

-- Disable continuations
keymap.set("n", "<Leader>o", "o<Esc>^Da", opts)
keymap.set("n", "<Leader>O", "O<Esc>^Da", opts)
--Zen mode

-- keymap.set('n', '<Leader>z', ':ZenMode<CR>', opts)
-- Jumplist
keymap.set("n", "<C-m>", "<C-i>", opts)

-- New tab
keymap.set("n", "te", ":tabedit")
keymap.set("n", "<tab>", ":bnext<Return>", opts)
keymap.set("n", "<s-tab>", ":bprev<Return>", opts)
keymap.set("n", "<C-tab>", ":tabnext<Return>", opts)
keymap.set("n", "<C-S-tab>", ":tabprev<Return>", opts)

-- Split window
keymap.set("n", "ss", ":split<Return>", opts)
keymap.set("n", "sv", ":vsplit<Return>", opts)
-- Move window
-- keymap.set('n', 'sh', '<C-w>h')
-- keymap.set('n', 'sk', '<C-w>k')
-- keymap.set('n', 'sj', '<C-w>j')
-- keymap.set('n', 'sl', '<C-w>l')

-- Resize window
-- keymap.set('n', '<C-w><left>', '<C-w><')
-- keymap.set('n', '<C-w><right>', '<C-w>>')
-- keymap.set('n', '<C-w><up>', '<C-w>+')
-- keymap.set('n', '<C-w><down>', '<C-w>-')

--  See `:help wincmd` for a list of all window commands
vim.keymap.set("n", "<C-h>", "<C-w><C-h>", { desc = "Move focus to the left window" })
vim.keymap.set("n", "<C-l>", "<C-w><C-l>", { desc = "Move focus to the right window" })
vim.keymap.set("n", "<C-j>", "<C-w><C-j>", { desc = "Move focus to the lower window" })
vim.keymap.set("n", "<C-k>", "<C-w><C-k>", { desc = "Move focus to the upper window" })
-- Diagnostics
-- keymap.set("n", "<C-j>", function()
--   vim.diagnostic.goto_next()
-- end, opts;)
-- vim.keymap.set("n", "<leader>S", function()
--   vim.cmd("e code.cpp")
--   vim.cmd("vsplit")
--   vim.cmd("e input.txt")
--   vim.cmd("split")
--   vim.cmd("e output.txt")
--   vim.cmd("wincmd h")
--   for _ = 1, 5 do
--     vim.cmd("wincmd >")
--   end
-- end)
--
-- vim.keymap.set("n", "<leader>r", function()
--   -- Compile the program
--   local build_cmd = "g++ -o code code.cpp 2> error.log"
--   local build_result = os.execute(build_cmd)
--
--   -- Check if there are any errors in error.log
--   local error_file = io.open("error.log", "r")
--   local errors = error_file:read("*a")
--   error_file:close()
--
--   if errors == "" then
--     -- No errors, run the program and redirect output to output.txt
--     os.execute("cat input.txt | ./code > output.txt")
--
--     -- Check if output.txt is already open in any buffer
--     local output_bufnr = vim.fn.bufnr("output.txt")
--
--     if output_bufnr ~= -1 then
--       -- If output.txt is open, reload it without switching buffers
--       vim.api.nvim_buf_call(output_bufnr, function()
--         vim.cmd("edit")
--       end)
--     end
--     -- If output.txt is not open, do nothing (no new splits or buffer replacements)
--   else
--     -- If there are errors, show them in the error.log buffer in the current window
--     vim.cmd("e error.log")
--   end
-- end)
-- Open terminal in a 40% view and run C++ code
-- vim.api.nvim_set_keymap(
--   "n",
--   "<leader>r",
--   ':vsplit | terminal cmd /c "g++ % -o %< && %<.exe"<CR>',
--   { noremap = true, silent = true }
-- )
-- vim.keymap.set(
--   "n",
--   "<leader>r",
--   ":w<CR>:vert 40vsplit | terminal g++ % -o %<.out && ./%<.out<CR>",
--   { noremap = true, silent = true }
-- )
vim.keymap.set(
  "n",
  "<leader>r",
  ":w<CR>:vert 40vsplit | terminal g++ % -o %<.out && ./%<.out < input.txt<CR>",
  { noremap = true, silent = true }
)
-- vim.keymap.set("n", "<leader>i", function()
--   vim.cmd("vert resize 20 | vnew | terminal node " .. vim.fn.expand("%"))
-- end, { noremap = true, silent = true, desc = "Run current JS file in vertical terminal" })

-- Map to compile and run the C++ code
-- Map to compile and run the C++ code on Windows

-- --Extraaaaaaa
vim.g.mapleader = " "
vim.keymap.set("n", "<leader>pv", vim.cmd.Ex)

vim.keymap.set("v", "J", ":m '>+1<CR>gv=gv")
vim.keymap.set("v", "K", ":m '<-2<CR>gv=gv")

vim.keymap.set("n", "J", "mzJ`z")
vim.keymap.set("n", "<C-d>", "<C-d>zz")
vim.keymap.set("n", "<C-u>", "<C-u>zz")
vim.keymap.set("n", "n", "nzzzv")
vim.keymap.set("n", "N", "Nzzzv")
vim.keymap.set("n", "<leader>zig", "<cmd>LspRestart<cr>")

vim.keymap.set("n", "<leader>vwm", function()
  require("vim-with-me").StartVimWithMe()
end)
vim.keymap.set("n", "<leader>svwm", function()
  require("vim-with-me").StopVimWithMe()
end)

-- greatest remap ever
vim.keymap.set("x", "<leader>p", [["_dP]])

-- next greatest remap ever : asbjornHaland
vim.keymap.set({ "n", "v" }, "<leader>y", [["+y]])
vim.keymap.set("n", "<leader>Y", [["+Y]])

vim.keymap.set({ "n", "v" }, "<leader>d", [["_d]])

-- This is going to get me cancelled
vim.keymap.set("i", "<C-c>", "<Esc>")

vim.keymap.set("n", "Q", "<nop>")
-- vim.keymap.set("n", "<C-k>", "<cmd>cnext<CR>zz")
-- vim.keymap.set("n", "<C-j>", "<cmd>cprev<CR>zz")
vim.keymap.set("n", "<leader>k", "<cmd>lnext<CR>zz")
vim.keymap.set("n", "<leader>j", "<cmd>lprev<CR>zz")

vim.keymap.set("n", "<leader>s", [[:%s/\<<C-r><C-w>\>/<C-r><C-w>/gI<Left><Left><Left>]])
vim.keymap.set("n", "<leader>x", "<cmd>!chmod +x %<CR>", { silent = true })
-- Set keybinding for <leader>h to toggle terminal with specific options
-- vim.keymap.set("n", "<leader>h", function()
--   vim.cmd "ToggleTerm size=10 dir=~/Desktop direction=horizontal name=desktop"
-- end, { noremap = true, silent = true })
-- Set the keymap to toggle nvim-tree with <Space>e
vim.keymap.set("n", "<Space>e", ":NvimTreeToggle<CR>", { noremap = true, silent = true })
-- map({ "n", "i", "v" }, "<C-s>", "<cmd> w <cr>")
-- tansparency toggle
vim.keymap.set("n", "<leader>tt", function()
  require("base46").toggle_transparency()
end, { desc = "Toggle Transparency" })
vim.keymap.set("n", "<leader>ww", function()
  vim.cmd "edit ~/.config/NvChad/lua/chadrc.lua"
end, { desc = "Edit chadrc.lua" })
vim.keymap.set({ 'n' }, '<A-f>', function()
  vim.g.neovide_fullscreen = not vim.g.neovide_fullscreen
end, { desc = "Toggle Neovide Fullscreen" })

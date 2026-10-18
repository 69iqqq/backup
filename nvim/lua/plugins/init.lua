return {
	{
		"stevearc/conform.nvim",
		-- event = 'BufWritePre', -- uncomment for format on save
		opts = require("configs.conform"),
	},

	-- These are some examples, uncomment them if you want to see them work!
	{
		"neovim/nvim-lspconfig",
		config = function()
			require("configs.lspconfig")
		end,
	},

{
    "nvim-treesitter/nvim-treesitter",
    opts = {
        ensure_installed = {
            "vim",
            "lua",
            "vimdoc",
            "html",
            "css",
            "cpp",
            "tsx",
            "c",
        },
    },
},
	-- {
	-- 	"zbirenbaum/copilot.lua",
	-- 	lazy = false,
	-- 	cmd = "Copilot",
	-- 	event = "InsertEnter",
	-- 	config = function()
	-- 		require("copilot").setup({})
	-- 	end,
	-- },
	-- {
	-- 	{
	-- 		"CopilotC-Nvim/CopilotChat.nvim",
	-- 		dependencies = {
	-- 			{ "zbirenbaum/copilot.lua" },
	-- 			{ "nvim-lua/plenary.nvim", branch = "master" }, -- for curl, log and async functions
	-- 		},
	-- 		build = "make tiktoken", -- Only on MacOS or Linux
	-- 		opts = {
	-- 			-- See Configuration section for options
	-- 		},
	-- 		-- See Commands section for default commands if you want to lazy load on them
	-- 	},
	-- },
}

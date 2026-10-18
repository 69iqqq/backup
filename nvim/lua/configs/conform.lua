local options = {
    formatters_by_ft = {
        lua = { "stylua" },
        cpp = { "clang-format" }, -- Add formatter for C++
        -- You can also include other filetypes if needed
        -- css = { "prettier" },
        -- html = { "prettier" },
    },

    -- format_on_save = {
    --   timeout_ms = 500,
    --   lsp_fallback = true,
    -- },
}

return options

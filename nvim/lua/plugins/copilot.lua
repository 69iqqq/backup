return {
    { import = "nvchad.blink.lazyspec" },

    -- 1. Configure the backend (copilot
    {
        "zbirenbaum/copilot.lua",
        enabled = false,
        cmd = "Copilot",
        event = "InsertEnter",
        config = function()
            require("copilot").setup({
                -- DISABLE the built-in ghost text so Blink can take over
                suggestion = { enabled = true },
                panel = { enabled = false },
            })
        end,
    },

    -- 2. Configure the frontend (blink.cmp)
    {
        "saghen/blink.cmp",
        dependencies = { "fang2hou/blink-copilot" },
        opts = {
            sources = {
                -- IMPORTANT: You must include "copilot" alongside your other sources.
                -- If you only list "copilot", you lose LSP and snippets!
                default = { "lsp", "path", "snippets", "buffer", "copilot" },

                providers = {
                    copilot = {
                        name = "copilot",
                        module = "blink-copilot",
                        score_offset = 100, -- Shows Copilot on top
                        async = true,

                        -- transform_items is sometimes needed to clean up duplicate suggestions
                        transform_items = function(_, items)
                            local CompletionItemKind = require("blink.cmp.types").CompletionItemKind
                            local kind_idx = #CompletionItemKind + 1
                            CompletionItemKind[kind_idx] = "Copilot"
                            for _, item in ipairs(items) do
                                item.kind = kind_idx
                            end
                            return items
                        end,
                    },
                },
            },

            -- Optional: Add an icon for Copilot in the menu
            appearance = {
                kind_icons = {
                    Copilot = "",
                },
            },
        },
    },
}
-- 3. (Optional) Configure the frontend for Blink (blink.lua)
-- This step is only needed if you haven't already set up Blink.
-- Make sure to include "copilot" in the default sources as shown above.
-- it is there
-- but just in case, here is the blink.lua config snippet:

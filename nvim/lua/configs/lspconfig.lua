-- Load NvChad defaults
require("nvchad.configs.lspconfig").defaults()

-- Define tsserver manually
vim.lsp.config("tsserver", {
  cmd = { "typescript-language-server", "--stdio" },
  filetypes = {
    "javascript",
    "javascriptreact",
    "typescript",
    "typescriptreact",
  },
  root_dir = vim.fs.root(0, {
    "package.json",
    "tsconfig.json",
    "jsconfig.json",
    ".git",
  }),
})
--rust
-- Enable LSP servers
vim.lsp.enable({
  "html",
  "cssls",
  "clangd",
  "tsserver",
  "rust_analyzer",
  "gopls"
})

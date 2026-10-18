vim.g.mapleader = " "

local function run_with_input()
  local file = vim.fn.expand("%:p")
  local ext = vim.fn.expand("%:e")
  local filename = vim.fn.expand("%:t:r")

  if file == "" then
    vim.notify("No file open", vim.log.levels.ERROR)
    return
  end

  local has_input =
      vim.fn.filereadable("input.txt") == 1 and
      vim.fn.getfsize("input.txt") > 0

  local is_cargo = vim.fn.filereadable("Cargo.toml") == 1
  local is_go_mod = vim.fn.filereadable("go.mod") == 1

  local cmd

  -- C++
  if ext == "cpp" then
    if has_input then
      cmd = string.format(
        [[
        g++ "%s" -o output.exe;
        if ($?) { Get-Content input.txt | .\output.exe }
        ]],
        file
      )
    else
      cmd = string.format(
        [[
        g++ "%s" -o output.exe;
        if ($?) { .\output.exe }
        ]],
        file
      )
    end

    -- Python
  elseif ext == "py" then
    if has_input then
      cmd = string.format(
        'Get-Content input.txt | python "%s"',
        file
      )
    else
      cmd = string.format(
        'python "%s"',
        file
      )
    end

    -- Rust
  elseif ext == "rs" then
    if is_cargo then
      if has_input then
        cmd = 'Get-Content input.txt | cargo run'
      else
        cmd = 'cargo run'
      end
    else
      if has_input then
        cmd = string.format(
          [[
          rustc "%s" -o "%s.exe";
          if ($?) { Get-Content input.txt | .\%s.exe }
          ]],
          file,
          filename,
          filename
        )
      else
        cmd = string.format(
          [[
          rustc "%s" -o "%s.exe";
          if ($?) { .\%s.exe }
          ]],
          file,
          filename,
          filename
        )
      end
    end

    -- Go 🐹
  elseif ext == "go" then
    if is_go_mod then
      if has_input then
        cmd = 'Get-Content input.txt | go run .'
      else
        cmd = 'go run .'
      end
    else
      if has_input then
        cmd = string.format(
          'Get-Content input.txt | go run "%s"',
          file
        )
      else
        cmd = string.format(
          'go run "%s"',
          file
        )
      end
    end
  else
    vim.notify("Unsupported file type: " .. ext, vim.log.levels.ERROR)
    return
  end

  local width = math.floor(vim.o.columns * 0.8)
  local height = math.floor(vim.o.lines * 0.7)

  local buf = vim.api.nvim_create_buf(false, true)

  local win = vim.api.nvim_open_win(buf, true, {
    relative = "editor",
    width = width,
    height = height,
    col = (vim.o.columns - width) / 2,
    row = (vim.o.lines - height) / 2,
    style = "minimal",
    border = "rounded",
  })

  vim.keymap.set("n", "q", function()
    vim.api.nvim_win_close(win, true)
  end, { buffer = buf, silent = true })

  vim.fn.termopen(
    { "powershell", "-NoLogo", "-Command", cmd },
    { cwd = vim.fn.getcwd() }
  )
end

vim.keymap.set(
  "n",
  "<leader>i",
  run_with_input,
  { desc = "Run (C++ / Python / Rust / Go, auto input + project)", silent = true }
)

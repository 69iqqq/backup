# Oh My Posh
# oh-my-posh init pwsh --config "takuya" | Invoke-Expression
oh-my-posh init pwsh --config "tokyonight_storm" | Invoke-Expression
Invoke-Expression (&starship init powershell)
#oh-my-posh init pwsh | Invoke-Expression
# Zoxide
Invoke-Expression (& { (zoxide init powershell | Out-String) })

if ($env:TERM_PROGRAM -eq "kiro") { . "$(kiro --locate-shell-integration-path pwsh)" }

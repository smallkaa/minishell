add-auto-load-safe-path /src/.gdbinit
# Содержимое файла .gdbinit
define print_cmd_chain
  set $current_cmd = $arg0
  set $cmd_index = 0
  while $current_cmd != 0
    printf "--- Command %d at %p ---\n", $cmd_index, $current_cmd
    p *$current_cmd
    printf "  Argv:\n"
    set $i = 0
    if $current_cmd->argv != 0
      while $current_cmd->argv[$i] != 0
        printf "    [%d]: %s\n", $i, $current_cmd->argv[$i]
        set $i = $i + 1
      end
    else
      printf "    (NULL)\n"
    end
    printf "  Binary: %s\n", $current_cmd->binary ? $current_cmd->binary : "NULL"
    printf "  Redirs:\n"
    set $node = $current_cmd->redirs
    if $node == 0
       printf "    (None)\n"
    end
    while $node != 0
       printf "    Redir at %p:\n", $node
       p *(t_redir *)($node->content)
       set $node = $node->next
    end
    set $current_cmd = $current_cmd->next
    set $cmd_index = $cmd_index + 1
  end
end

# Сюда можно добавить и другие полезные команды или настройки GDB
# Например, команду для печати токенов:
define print_tokens
  set $i = 0
  while $i < $arg0->count
    printf "Token %d:\n", $i
    p $arg0->tokens[$i]
    set $i = $i + 1
  end
end
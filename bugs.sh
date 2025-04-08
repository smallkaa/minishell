######################################################################
#REDIRECTION
#####################################################################

#Case 59
#42_minishell_tester/cmds/mand/1_redirs.sh:121 
# works in minishell

#Case 60
# 42_minishell_tester/cmds/mand/1_redirs.sh:125  
# no '\n' parsing
/bin/echo "cat this text\n" > tmp_redir_out
cat <tmp_redir_out

# Case 65
# 42_minishell_tester/cmds/mand/1_redirs.sh:138  
# works in minishell

# Case 66
# 42_minishell_tester/cmds/mand/1_redirs.sh:142 
# works in minishell

# Case 67
# 42_minishell_tester/cmds/mand/1_redirs.sh:153 
# works in minishell

# Case 80
# 42_minishell_tester/cmds/mand/1_redirs.sh:186 
# works in minishell


# Case 81
# 42_minishell_tester/cmds/mand/1_redirs.sh:186 
<<lim cat
 "lim"
HELLO
42
 lim
testing your minishell
limm
lim

# Case 82
# 42_minishell_tester/cmds/mand/1_redirs.sh:204
# works in minishell

# Case 83
# 42_minishell_tester/cmds/mand/1_redirs.sh:208
# works in minishell

# Case 84
# 42_minishell_tester/cmds/mand/1_redirs.sh:217
#!!!--------- no need to expand now, hope to pass without it
cat << hello
$USER
$NOVAR
$HOME
hello

# Case 85
# 42_minishell_tester/cmds/mand/1_redirs.sh:223
# works in minishell


# Case 86
# 42_minishell_tester/cmds/mand/1_redirs.sh:229
# works in minishell

# Case 87
# 42_minishell_tester/cmds/mand/1_redirs.sh:235
# works in minishell

# Case 88
# 42_minishell_tester/cmds/mand/1_redirs.sh:241
#!!!--------- no need to expand now, hope to pass without it
cat << 'lim'
$USER
$NOVAR
$HOME
lim

# Case 89
# 42_minishell_tester/cmds/mand/1_redirs.sh:247
> out
/bin/rm -f out

# Case 90
# 42_minishell_tester/cmds/mand/1_redirs.sh:250
< out
/bin/rm -f out

# Case 91
# 42_minishell_tester/cmds/mand/1_redirs.sh:253
>> out
/bin/rm -f out
#!/bin/bash

gcc -o Commandcat Commandcat.c
gcc -o Commandchmod Commandchmod.c
gcc -o Commandhelp Commandhelp.c
gcc -o Commandls Commandls.c
gcc -o Commandrmdir Commandrmdir.c
gcc -o Commandmkdir Commandmkdir.c
gcc -o Commandtouch Commandtouch.c
gcc -o minishell minishell.c

./minishell

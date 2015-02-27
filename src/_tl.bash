#!/usr/bin/env bash

_tl () {
  local cur prev opts
  COMPREPLY=()
  cur=${COMP_WORDS[COMP_CWORD]}
  prev=${COMP_WORDS[COMP_CWORD-1]}
  cmds='init timepoint pending pop-drop merge-add unlog report'

  case "${prev}" in
    init)
      return 0 ;;
    timepoint)
      COMPREPLY=(
        '-l <location>'
        '-m <message>'
        '-t <timestamp>'
        ''
      )
      return 0 ;;
    pending)
      return 0 ;;
    pop-drop)
      return 0 ;;
    merge-add)
      return 0 ;;
    unlog)
      COMPREPLY=(
        '-d'
        '<id>'
      )
      return 0 ;;
    report)
      return 0 ;;
  esac

  COMPREPLY=( $(compgen -W "${cmds}" -- ${cur}) )
}

complete -F _tl tl

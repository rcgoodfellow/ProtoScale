" ProtoScale Modeling vim syntax file
" Language: ProtoScale Description Language (.pdl)
" Maintainer: Ryan Goodfellow
" Version 0.1

if exists("b:current_syntax")
  finish
endif

syn keyword pmKeyword create destroy import connect symmetric
syn match pmComment '`.*`'
syn match pmSpecial '\:=\|\:\|\[\|\]\|{\|}\|(\|)\|;\|,'

hi link pmKeyword Keyword
hi link pmComment Comment
hi link pmSpecial Special

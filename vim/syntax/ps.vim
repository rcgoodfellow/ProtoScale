" ProtoScale vim syntax file
" Language: ProtoScale Description Language (.pdl)
" Maintainer: Ryan Goodfellow
" Version 0.1

if exists("b:current_syntax")
  finish
endif

syn keyword psKeyword object
syn keyword psType real complex complex; real;

syn match psSpecial '\:\|\;\|\[\|\]'
syn match psComment '`.*`'

hi link psKeyword Keyword
hi link psComment Comment
hi link psSpecial Special
hi link psType Type

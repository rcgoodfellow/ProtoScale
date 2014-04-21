" ProtoScale vim syntax file
" Language: ProtoScale Description Language (.pdl)
" Maintainer: Ryan Goodfellow
" Version 0.1

if exists("b:current_syntax")
  finish
endif

syn keyword pmmKeyword module node link info alias interlate
syn keyword pmmType real complex
syn match pmmTypedVar '\h\a*\:\s*\h\a*' contains=pmmSpecial
" syn match pmmConnect '\(\h\a*\)(\h\a*\:\s*\h\a*,\s*\h\a*\:\s*\h\a*)\:*' contains=pmmTypedVar,pmmSpecial
syn match pmmMathOp '+\|-\|\*\|/\|=\|,\|\''
syn match pmmSpecial '<-\|->\|\:=\|\:\|\[\|\]\|{\|}\|;'
syn match pmmComment '`.*`'

hi link pmmKeyword Keyword
hi link pmmConnect Function
hi link pmmSpecial Special
hi link pmmComment Comment
hi link pmmMathOp Operator
hi link pmmType Type


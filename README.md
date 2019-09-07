# rasm  
Assembler, which will be used in Munix  
  
building rasm:  
$make  
  
delete libs and objects:  
$make clean  
  
after building, binary file may be launched from core folder using  
$rasm/rasm [options] <file>

to get (debug)releaze version,  
(un)comment _DEBUG param (in stddefs.mk)

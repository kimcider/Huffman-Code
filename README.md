# Huffman-Code
Compress/Decompress txt files.

- To compress  
  ./huffman -c filename  
  If you compress file, the output file name add .zz at the end of the file name.  
  For example, if you compress 'Hamlet.txt', the output file name is 'Hamlet.txt.zz'  
  
- To decompress  
  ./huffman -d filename  
  If you decompress file, the output file name add .yy at the end of the file name.   
  For example, if you decompress 'Hamlet.txt.zz', the output file name is 'Hamlet.txt.zz.yy'  

All characters in the file must be characters in the ASCII code table.  
The file size shall not exceed 100 megabytes.

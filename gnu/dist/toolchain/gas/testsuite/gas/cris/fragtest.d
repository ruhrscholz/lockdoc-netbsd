#objdump: -dr
#name: fragtest

.*:     file format .*-cris
Disassembly of section \.text:
00000000 <l1-(0x)?100>:
[	 ]+0:[	 ]+fee0[ 	]+ba[ ]+(0x[0]?100|100 <l1>)
[ 	]+2:[		]+0f05[	 ]+nop[ ]*
^[ 	]+\.\.\.
[	 ]+80:[	 ]+e2e0[ 	]+ba[ ]+(0x[0]?164|164 <l2>)
^[ 	]+82:[		]+0f05[	 ]+nop[ ]*
^[ 	]+\.\.\.
00000100 <l1>:
^[ 	]+\.\.\.
00000164 <l2>:
[	 ]+164:[	 ]+ffed 0001[ 	]+ba (0x[0]?268|268 <l3>)
[ 	]+168:[		]+0f05[	 ]+nop[ ]*
^[ 	]+\.\.\.
[	 ]+1e6:[	 ]+ffed 6604[ 	]+ba (0x[0]?650|650 <l4>)
[ 	]+1ea:[		]+0f05[	 ]+nop[ ]*
^[ 	]+\.\.\.
00000268 <l3>:
^[ 	]+\.\.\.
00000650 <l4>:
[	 ]+650:[	 ]+ffed 0801[ 	]+ba (0x[0]?75c|75c <l5>)
[ 	]+654:[	]+0f05[	 ]+nop[ ]*
^[ 	]+\.\.\.
[	 ]+6d2:[	 ]+08e0[ 	]+ba[ ]+(0x[0]?6dc|[0]?6dc <l4\+0x8c>)
[	 ]+6d4:[	 ]+0f05[ 	]+nop[ ]*
[	 ]+6d6:[	 ]+3f0d 4488 0000[ 	]+jump[ ]+(0x8844|8844 <l6>)
[ 	]+6d8:[ 	]+(R_CRIS_)?32[ 	]+\.text\+(0x[0]*8844|8844 <l6>)
[	 ]+6dc:[	 ]+f9e0[ 	]+ba[ ]+(0x[0]?6d6|6d6 <l4\+0x86>)
[ 	]+6de:[		]+0f05[	 ]+nop[ ]*
^[ 	]+\.\.\.
0000075c <l5>:
^[ 	]+\.\.\.
00008844 <l6>:
^[ 	]+\.\.\.
[	 ]+88c0:[	 ]+fee0[ 	]+ba[ ]+(0x89c0|89c0 <l8>)
[	 ]+88c2:[	 ]+0f05[ 	]+nop[ ]*
^[ 	]+\.\.\.
[	 ]+8940:[	 ]+0000[ 	]+bcc[ ]+.*
[	 ]+8942:[	 ]+01e0[ 	]+ba[ ]+(0x8844|8844 <l6>)
[	 ]+8944:[	 ]+0f05[ 	]+nop[ ]*
^[ 	]+\.\.\.
000089c0 <l8>:
^[ 	]+\.\.\.
[	 ]+8a3c:[	 ]+ffed 0201[ 	]+ba (0x8b42|8b42 <l10>)
[	 ]+8a40:[	 ]+0f05[ 	]+nop[ ]*
^[ 	]+\.\.\.
[	 ]+8abe:[	 ]+0000[ 	]+bcc[ ]+.*
[	 ]+8ac0:[	 ]+ffed fcfe[ 	]+ba (0x89c0|89c0 <l8>)
[	 ]+8ac4:[	 ]+0f05[ 	]+nop[ ]*
^[ 	]+\.\.\.
00008b42 <l10>:
^[ 	]+\.\.\.
[	 ]+8bbe:[	 ]+0000[ 	]+bcc[ ]+.*
[	 ]+8bc0:[	 ]+ffed 0001[ 	]+ba (0x8cc4|8cc4 <l12>)
[	 ]+8bc4:[	 ]+0f05[ 	]+nop[ ]*
^[ 	]+\.\.\.
[	 ]+8c42:[	 ]+0000[ 	]+bcc[ ]+.*
[	 ]+8c44:[	 ]+ffed fafe[ 	]+ba (0x8b42|8b42 <l10>)
[	 ]+8c48:[	 ]+0f05[ 	]+nop[ ]*
^[ 	]+\.\.\.

#! /usr/bin/awk -f
#
#	create IOCS call interface from iocs.h
#
#	written by Yasha (ITOH Yasufumi)
#	public domain
#
#	$NetBSD: makeiocscalls.awk,v 1.1 1998/09/01 19:53:54 itohy Exp $

BEGIN {
	argsiz["l"] = 4; argsiz["w"] = 2
	argsiz["lb31"] = 4; argsiz["wb8"] = 2

	for (i = 0; i < 16; i++) {
		reg = substr("d0d1d2d3d4d5d6d7a0a1a2a3a4a5a6a7", i*2+1, 2)
		regno[reg] = i
	}
}

$1 == "/*" && ($2 ~ /^[0-9a-f][0-9a-f]$/ || $2 == "(none)") {
	funcnam=""
	iocsno=$2
	narg=0
	retd2=0
	err_d0=0
	noret=0
	c_md=0
	b_super=0
	sp_regst=0
	b_curmod = 0
	b_curpat = 0
	b_scroll = 0
	iocs_trap15=0
	for (i = 3; i <= NF && $i != "*/" && $i != ";"; i++) {
		arg[narg] = $i
		narg++
	}
	if ($i == ";") {
		# process opts
		for (i++; i <= NF && $i != "*/"; i++) {
			if ($i == "retd2")
				retd2 = 1
			else if ($i == "err_d0")
				err_d0 = 1
			else if ($i == "noret")
				noret = 1
			else if ($i == "c_md")
				c_md = 1
			else if ($i == "b_super")
				b_super = 1
			else if ($i == "sp_regst")
				sp_regst = 1
			else if ($i == "b_curmod")
				b_curmod = 1
			else if ($i == "b_curpat")
				b_curpat = 1
			else if ($i == "b_scroll")
				b_scroll = 1
			else if ($i == "trap15")
				iocs_trap15 = 1
			else {
				print FILENAME ":" NR ": unknown opt", $i
				exit(1)
			}
		}
	}
	if ($i != "*/") {
		print FILENAME ":" NR ": malformed input line:" $0
		exit(1)
	}

	# find out func name
	printf "|"
	for (i++; i <= NF; i++) {
		printf " %s", $i
		if ($i ~ /^\**IOCS_[A-Z0-9_]*$/) {
			funcnam = $i
			while (funcnam ~ /^\*/)
				funcnam = substr(funcnam, 2, length(funcnam) -1)
		}
	}
	print ""
	if (!funcnam) {
		print FILENAME ":" NR ": can't find function name"
		exit(1)
	}

	# output assembly code
	print "\t.text\n\t.even"
	print "\t.globl\t_" funcnam
	print "_" funcnam ":"

	# SAVE REGISTERS
	for (i = 0; i < 16; i++) {
		savereg[i] = 0
	}
	for (i = 0; i < narg; i++) {
		r = arg[i]
		if (r ~ /^o[ad][0-7]$/)
			r = substr(r, 2, 2)
		else if (r ~ /^d[0-7]=/)
			r = substr(r, 1, 2)
		if (r != "d0" && !regno[r]) {
			print FILENAME ":" NR ": unknown arg type:", arg[i]
			exit(1)
		}
		if (r !~ /^[da][01]$/)		# may not be saved
			savereg[regno[r]] = r
	}
	# count reg to save
	nsave = 0
	for (i = 0; i < 16; i++) {
		if (savereg[i])
			nsave++
	}

	if (iocs_trap15) {
		print "\tmoveml\td2-d7/a2-a6,sp@-"
		nsave = 11
	} else if (nsave == 1 || nsave == 2){
		# use movel
		for (i = 0; i < 16; i++) {
			if (savereg[i])
				print "\tmovel\t" savereg[i] ",sp@-"
		}
	} else if (nsave > 2) {
		# use moveml
		saveregs = ""
		for (i = 0; i < 16; i++) {
			if (savereg[i])
				saveregs = saveregs "/" savereg[i]
		}
		saveregs = substr(saveregs, 2, length(saveregs) - 1)
		print "\tmoveml\t" saveregs ",sp@-"
	}

	# LOAD ARGS
	# XXX this should be more intelligent
	argoff = nsave * 4 + 4
	# input arguments for IOCS call
	iarg = ""
	niarg = 0
	iarg_incorder = 1
	immarg = ""
	nimmarg = 0
	for (i = 0; i < narg && arg[i] ~ /^[ad]/; i++) {
		a = arg[i]
		if (a ~ /^d[1-7]=[0-9][0-9]*$/) {
			immarg = immarg " " a
			nimmarg++
		} else {
			if (iarg) {
				if (regno[a1] >= regno[a])
					iarg_incorder = 0
			}
			a1 = a
			iarg = iarg "/" a
			niarg++
		}
	}
	oarg = ""
	noarg = 0
	for ( ; i < narg; i++) {
		if (arg[i] ~ /^[o]d[0-7]/) {
			oarg = oarg " " arg[i]
			noarg++
		} else {
			print "unknown arg:", arg[i]
			exit(1)
		}
	}
	# remove leading char
	iarg = substr(iarg, 2, length(iarg) - 1);
	immarg = substr(immarg, 2, length(immarg) - 1);
	oarg = substr(oarg, 2, length(oarg) - 1);
	# load input args
	if (niarg == 0)
		;
	else if (niarg == 1 && iarg !~ /\=/) {
		print "\tmovel\tsp@(" argoff ")," iarg	"\t| 1arg"
	} else if (iarg_incorder && iarg !~ /\=/) {
		print "\tmoveml\tsp@(" argoff ")," iarg	"\t| inc order"
	} else if (iarg == "a1/d1") {
		print "\tmoveal\tsp@(" argoff "),a1"
		print "\tmovel\tsp@(" argoff + 4 "),d1"
	} else if (iarg == "d1/a1/d2") {
		print "\tmoveml\tsp@(" argoff "),d1-d2/a1"
		print "\texg\td2,a1"
	} else if (iarg == "a1/a2/d1") {
		print "\tmoveml\tsp@(" argoff "),a1/a2"
		print "\tmovel\tsp@(" argoff + 8 "),d1"
	} else if (iarg == "a1/a2/d1/d2") {
		print "\tmoveml\tsp@(" argoff "),d1-d2/a1-a2"
		print "\texg\td1,a1"
		print "\texg\td2,a2"
	} else if (iarg == "a1/d1/d2") {
		print "\tmoveml\tsp@(" argoff "),d0-d2"
		print "\tmovel\td0,a1"
	} else if (iarg == "d1=bb") {
		print "\tmoveq\t#0,d1"
		print "\tmoveb\tsp@(" argoff + 3 "),d1"
		print "\tlslw\t#8,d1"
		print "\tmoveb\tsp@(" argoff + 7 "),d1"
		niarg = 2
	} else if (iarg == "d1=ww") {
		print "\tmovew\tsp@(" argoff + 2 "),d1"
		print "\tswap\td1"
		print "\tmovew\tsp@(" argoff + 6 "),d1"
		niarg = 2
	} else if (iarg == "d1=hsv") {
		print "\tmoveb\tsp@(" argoff + 3 "),d1"
		print "\tswap\td1"
		print "\tmoveb\tsp@(" argoff + 7 "),d1"
		print "\tlslw\t#8,d1"
		print "\tmoveb\tsp@(" argoff + 11 "),d1"
		print "\tandl\t#0x00ff1f1f,d1"
		niarg = 3
	} else if (iarg == "a1/d1=bb") {
		print "\tmoveal\tsp@(" argoff "),a1"
		print "\tmoveq\t#0,d1"
		print "\tmoveb\tsp@(" argoff + 7 "),d1"
		print "\tlslw\t#8,d1"
		print "\tmoveb	sp@(" argoff + 11 "),d1"
		niarg = 3
	} else if (iarg == "d1/d2=ww") {
		print "\tmovel\tsp@(" argoff "),d1"
		print "\tmovew\tsp@(" argoff + 6 "),d2"
		print "\tswap\td2"
		print "\tmovew\tsp@(" argoff + 10 "),d2"
		niarg = 3
	} else if (iarg == "d1=ww/a1") {
		print "\tmoveml\tsp@(" argoff "),d0-d1/a1"
		print "\tswap\td1"
		print "\tmovew\td0,d1"
		print "\tswap\td1"
		niarg = 3
	} else if (iarg == "d1=ww/d2=ww") {
		print "\tmoveml\tsp@(" argoff "),d1-d2"
		print "\tswap\td1"
		print "\tmovew\td2,d1"
		print "\tmovew\tsp@(" argoff + 10 "),d2"
		print "\tswap\td2"
		print "\tmovew\tsp@(" argoff + 14 "),d2"
		niarg = 4
	} else {
		print "unsupported iarg:", iarg
		exit(1)
	}
	argoff += niarg * 4

	if (sp_regst) {
		print "\tandl\t#0x80000000,d1"
		print "\tmoveb\td0,d1"
	}

	if (b_curmod) {
		print "\tmoveq\t#1,d0"
		print "\tcmpl\td1,d0"
#		print "\tbcss\tLerr"
		print "\tbcss\t6f"
	}

	if (b_curpat) {
		print "\ttstw\td2"
#		print "\tbeqs\tLerr"
		print "\tbeqs\t6f"
	}

	if (b_super) {
		print "\tmoval\tsp@+,a0"
		print "\tmoval\tsp@,a1"
	}

	# load imm args
	if (nimmarg) {
		for (i = 0; i < narg && arg[i] ~ /^[ad]/; i++) {
			a = arg[i]
			if (a ~ /^d[1-7]=[0-9][0-9]*$/) {
				r = substr(a, 1, 2)
				v = substr(a, 4, length(a)-3)
				print "\tmoveq\t#" v "," r
			}
		}
	}

	if (c_md) {
		# -1: flush(3), -2: set default(2), other: set by the value(4)
		print "\tmovel\td2,d0"
		print "\taddql\t#1,d0"
		print "\tbeqs\tLcachemd"
		print "\tmoveq\t#2,d1"
		print "\taddql\t#1,d0"
		print "\tbnes\tLcachemd"
		print "\tmoveq\t#4,d1"
		print "Lcachemd:"
	}

	if (b_scroll) {
		# d1 has 16
		print "\tcmpl\td1,d2"
		print "\tbcss\tLscriocs"
		print "\tmovel\td2,d1"
		print "Lscriocs:"
	}

	if (iocs_trap15) {
		print "\tmoveal\tsp@(" argoff "),a0	| inregs"
		print "\tmoveml\ta0@,d0-d7/a1-a6"
		argoff += 4
	}

	if (iocsno != "(none)") {
		if (iocsno ~ /^[89abcdef]./)
			iocsno = "ffffff" iocsno
		print "\tmoveq\t#0x" iocsno ",d0"
	}
	print "\ttrap\t#15"

	if (iocs_trap15) {
		print "\tmoveal\tsp@(" argoff "),a0	| outregs"
		print "\tmoveml\td0-d7/a1-a6,a0@"
	}

	if (err_d0 && noarg) {
		print "\ttstl\td0"
#		print "\tbnes\tLerr"
		print "\tbnes\t6f"
	}

	# SAVERESULTS
	# XXX this should be more intelligent
	if (noarg == 0)
		;
	else if (oarg == "od2") {
		print "\tmoveal\tsp@(" argoff "),a0"
		argoff += 4
		print "\tmovel\td2,a0@"
	} else if (oarg == "od1 od2 od0") {
		print "\tmoveml\tsp@(" argoff "),a0/a1"
		argoff += 8
		print "\tmovel\td1,a0@"
		print "\tmovel\td2,a1@"
		print "\tmoveal\tsp@(" argoff "),a0"
		argoff += 4
		print "\tmovel\td0,a0@"
	} else if (oarg == "od2 od3") {
		print "\tmoveml\tsp@(" argoff "),a0/a1"
		argoff += 8
		print "\tmovel\td2,a0@"
		print "\tmovel\td3,a1@"
	} else if (oarg == "od2 od3 od4 od5") {
		print "\tmoveml\tsp@(" argoff "),a0/a1"
		argoff += 8
		print "\tmovel\td2,a0@"
		print "\tmovel\td3,a1@"
		print "\tmoveml\tsp@(" argoff "),a0/a1"
		argoff += 8
		print "\tmovel\td4,a0@"
		print "\tmovel\td5,a1@"
	} else {
		print "unsupported oarg:", oarg
		exit(1)
	}

	if ((err_d0 && noarg) || b_curmod || b_curpat)
#		print "Lerr:"
		print "6:"

	# return value
	if (retd2)
		print "\tmovel\td2,d0"

	# RESTORE REGISTERS
	if (iocs_trap15) {
		print "\tmoveml\tsp@+,d2-d7/a2-a6"
	} else if (nsave == 1 || nsave == 2){
		# use movel
		for (i = 16 - 1; i >= 0; i--) {
			if (savereg[i])
				print "\tmovel\tsp@+," savereg[i]
		}
	} else if (nsave > 2) {
		# use moveml
		print "\tmoveml\tsp@+," saveregs
	}


	if (b_super)
		print "\tjmp\ta0@"
	else if (!noret)
		print "\trts"
}

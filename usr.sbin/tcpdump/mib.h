/*
 * This file was generated by tcpdump/makemib on Wed Sep 26 12:12:31 EDT 1990
 * You probably don't want to edit this by hand!
 *
 * struct mib somename = { desc, oid-octet, type, child-pointer, next-pointer
};
 */

/* parse problem: new name "mib" for mgmt.mib(1) ignored */
/* parse problem: no parent for 0.nullSpecific(0) */
struct obj
_proteon_obj = {
	"proteon", 1, 0,
	NULL, NULL
},
_ibm_obj = {
	"ibm", 2, 0,
	NULL, &_proteon_obj
},
_cmu_obj = {
	"cmu", 3, 0,
	NULL, &_ibm_obj
},
_unix_obj = {
	"unix", 4, 0,
	NULL, &_cmu_obj
},
_acc_obj = {
	"acc", 5, 0,
	NULL, &_unix_obj
},
_twg_obj = {
	"twg", 6, 0,
	NULL, &_acc_obj
},
_cayman_obj = {
	"cayman", 7, 0,
	NULL, &_twg_obj
},
_nysernet_obj = {
	"nysernet", 8, 0,
	NULL, &_cayman_obj
},
_cisco_obj = {
	"cisco", 9, 0,
	NULL, &_nysernet_obj
},
_nsc_obj = {
	"nsc", 10, 0,
	NULL, &_cisco_obj
},
_hp_obj = {
	"hp", 11, 0,
	NULL, &_nsc_obj
},
_epilogue_obj = {
	"epilogue", 12, 0,
	NULL, &_hp_obj
},
_utennessee_obj = {
	"utennessee", 13, 0,
	NULL, &_epilogue_obj
},
_bbn_obj = {
	"bbn", 14, 0,
	NULL, &_utennessee_obj
},
_xylogics_obj = {
	"xylogics", 15, 0,
	NULL, &_bbn_obj
},
_unisys_obj = {
	"unisys", 16, 0,
	NULL, &_xylogics_obj
},
_canstar_obj = {
	"canstar", 17, 0,
	NULL, &_unisys_obj
},
_wellfleet_obj = {
	"wellfleet", 18, 0,
	NULL, &_canstar_obj
},
_trw_obj = {
	"trw", 19, 0,
	NULL, &_wellfleet_obj
},
_mit_obj = {
	"mit", 20, 0,
	NULL, &_trw_obj
},
_eon_obj = {
	"eon", 21, 0,
	NULL, &_mit_obj
},
_spartacus_obj = {
	"spartacus", 22, 0,
	NULL, &_eon_obj
},
_excelan_obj = {
	"excelan", 23, 0,
	NULL, &_spartacus_obj
},
_spider_obj = {
	"spider", 24, 0,
	NULL, &_excelan_obj
},
_nsfnet_obj = {
	"nsfnet", 25, 0,
	NULL, &_spider_obj
},
_sytek_obj = {
	"sytek", 26, 0,
	NULL, &_nsfnet_obj
},
_intergraph_obj = {
	"intergraph", 27, 0,
	NULL, &_sytek_obj
},
_interlan_obj = {
	"interlan", 28, 0,
	NULL, &_intergraph_obj
},
_vitalink_obj = {
	"vitalink", 29, 0,
	NULL, &_interlan_obj
},
_ulana_obj = {
	"ulana", 30, 0,
	NULL, &_vitalink_obj
},
_nswc_obj = {
	"nswc", 31, 0,
	NULL, &_ulana_obj
},
_santacruzoperation_obj = {
	"santacruzoperation", 32, 0,
	NULL, &_nswc_obj
},
_xyplex_obj = {
	"xyplex", 33, 0,
	NULL, &_santacruzoperation_obj
},
_cray_obj = {
	"cray", 34, 0,
	NULL, &_xyplex_obj
},
_bellnorthernresearch_obj = {
	"bellnorthernresearch", 35, 0,
	NULL, &_cray_obj
},
_dec_obj = {
	"dec", 36, 0,
	NULL, &_bellnorthernresearch_obj
},
_touch_obj = {
	"touch", 37, 0,
	NULL, &_dec_obj
},
_networkresearchcorp_obj = {
	"networkresearchcorp", 38, 0,
	NULL, &_touch_obj
},
_baylor_obj = {
	"baylor", 39, 0,
	NULL, &_networkresearchcorp_obj
},
_nmfeccllnl_obj = {
	"nmfeccllnl", 40, 0,
	NULL, &_baylor_obj
},
_sri_obj = {
	"sri", 41, 0,
	NULL, &_nmfeccllnl_obj
},
_sun_obj = {
	"sun", 42, 0,
	NULL, &_sri_obj
},
_3com_obj = {
	"3com", 43, 0,
	NULL, &_sun_obj
},
_cmc_obj = {
	"cmc", 44, 0,
	NULL, &_3com_obj
},
_synoptics_obj = {
	"synoptics", 45, 0,
	NULL, &_cmc_obj
},
_cheyenne_obj = {
	"cheyenne", 46, 0,
	NULL, &_synoptics_obj
},
_prime_obj = {
	"prime", 47, 0,
	NULL, &_cheyenne_obj
},
_mcnc_obj = {
	"mcnc", 48, 0,
	NULL, &_prime_obj
},
_chipcom_obj = {
	"chipcom", 49, 0,
	NULL, &_mcnc_obj
},
_opticaldatasystems_obj = {
	"opticaldatasystems", 50, 0,
	NULL, &_chipcom_obj
},
_gated_obj = {
	"gated", 51, 0,
	NULL, &_opticaldatasystems_obj
},
_cabletron_obj = {
	"cabletron", 52, 0,
	NULL, &_gated_obj
},
_apollo_obj = {
	"apollo", 53, 0,
	NULL, &_cabletron_obj
},
_desktalksystems_obj = {
	"desktalksystems", 54, 0,
	NULL, &_apollo_obj
},
_ssds_obj = {
	"ssds", 55, 0,
	NULL, &_desktalksystems_obj
},
_castlerock_obj = {
	"castlerock", 56, 0,
	NULL, &_ssds_obj
},
_mips_obj = {
	"mips", 57, 0,
	NULL, &_castlerock_obj
},
_tgv_obj = {
	"tgv", 58, 0,
	NULL, &_mips_obj
},
_silicongraphics_obj = {
	"silicongraphics", 59, 0,
	NULL, &_tgv_obj
},
_ubc_obj = {
	"ubc", 60, 0,
	NULL, &_silicongraphics_obj
},
_merit_obj = {
	"merit", 61, 0,
	NULL, &_ubc_obj
},
_fibercom_obj = {
	"fibercom", 62, 0,
	NULL, &_merit_obj
},
_apple_obj = {
	"apple", 63, 0,
	NULL, &_fibercom_obj
},
_gandalf_obj = {
	"gandalf", 64, 0,
	NULL, &_apple_obj
},
_dartmouth_obj = {
	"dartmouth", 65, 0,
	NULL, &_gandalf_obj
},
_davidsystems_obj = {
	"davidsystems", 66, 0,
	NULL, &_dartmouth_obj
},
_reuter_obj = {
	"reuter", 67, 0,
	NULL, &_davidsystems_obj
},
_cornell_obj = {
	"cornell", 68, 0,
	NULL, &_reuter_obj
},
_tmac_obj = {
	"tmac", 69, 0,
	NULL, &_cornell_obj
},
_locus_obj = {
	"locus", 70, 0,
	NULL, &_tmac_obj
},
_nasa_obj = {
	"nasa", 71, 0,
	NULL, &_locus_obj
},
_retix_obj = {
	"retix", 72, 0,
	NULL, &_nasa_obj
},
_boeing_obj = {
	"boeing", 73, 0,
	NULL, &_retix_obj
},
_att_obj = {
	"att", 74, 0,
	NULL, &_boeing_obj
},
_ungermannbass_obj = {
	"ungermannbass", 75, 0,
	NULL, &_att_obj
},
_digitalanalysis_obj = {
	"digitalanalysis", 76, 0,
	NULL, &_ungermannbass_obj
},
_hplanman_obj = {
	"hplanman", 77, 0,
	NULL, &_digitalanalysis_obj
},
_netlabs_obj = {
	"netlabs", 78, 0,
	NULL, &_hplanman_obj
},
_icl_obj = {
	"icl", 79, 0,
	NULL, &_netlabs_obj
},
_auspex_obj = {
	"auspex", 80, 0,
	NULL, &_icl_obj
},
_lannet_obj = {
	"lannet", 81, 0,
	NULL, &_auspex_obj
},
_ncd_obj = {
	"ncd", 82, 0,
	NULL, &_lannet_obj
},
_raycom_obj = {
	"raycom", 83, 0,
	NULL, &_ncd_obj
},
_pirellifocom_obj = {
	"pirellifocom", 84, 0,
	NULL, &_raycom_obj
},
_datability_obj = {
	"datability", 85, 0,
	NULL, &_pirellifocom_obj
},
_networkappltech_obj = {
	"networkappltech", 86, 0,
	NULL, &_datability_obj
},
_link_obj = {
	"link", 87, 0,
	NULL, &_networkappltech_obj
},
_nyu_obj = {
	"nyu", 88, 0,
	NULL, &_link_obj
},
_rnd_obj = {
	"rnd", 89, 0,
	NULL, &_nyu_obj
},
_intercon_obj = {
	"intercon", 90, 0,
	NULL, &_rnd_obj
},
_learningtree_obj = {
	"learningtree", 91, 0,
	NULL, &_intercon_obj
},
_webstercomputer_obj = {
	"webstercomputer", 92, 0,
	NULL, &_learningtree_obj
},
_frontier_obj = {
	"frontier", 93, 0,
	NULL, &_webstercomputer_obj
},
_nokia_obj = {
	"nokia", 94, 0,
	NULL, &_frontier_obj
},
_allenbradley_obj = {
	"allenbradley", 95, 0,
	NULL, &_nokia_obj
},
_cern_obj = {
	"cern", 96, 0,
	NULL, &_allenbradley_obj
},
_sigma_obj = {
	"sigma", 97, 0,
	NULL, &_cern_obj
},
_emergingtech_obj = {
	"emergingtech", 98, 0,
	NULL, &_sigma_obj
},
_snmpresearch_obj = {
	"snmpresearch", 99, 0,
	NULL, &_emergingtech_obj
},
_ohiostate_obj = {
	"ohiostate", 100, 0,
	NULL, &_snmpresearch_obj
},
_ultra_obj = {
	"ultra", 101, 0,
	NULL, &_ohiostate_obj
},
_ccur_obj = {
	"ccur", 136, 0,
	NULL, &_ultra_obj
},
_enterprises_obj = {
	"enterprises", 1, 0,
	&_ccur_obj, NULL
},
_snmpInPkts_obj = {
	"snmpInPkts", 1, 0,
	NULL, NULL
},
_snmpOutPkts_obj = {
	"snmpOutPkts", 2, 0,
	NULL, &_snmpInPkts_obj
},
_snmpInBadVersions_obj = {
	"snmpInBadVersions", 3, 0,
	NULL, &_snmpOutPkts_obj
},
_snmpInBadCommunityNames_obj = {
	"snmpInBadCommunityNames", 4, 0,
	NULL, &_snmpInBadVersions_obj
},
_snmpInBadCommunityUses_obj = {
	"snmpInBadCommunityUses", 5, 0,
	NULL, &_snmpInBadCommunityNames_obj
},
_snmpInASNParseErrs_obj = {
	"snmpInASNParseErrs", 6, 0,
	NULL, &_snmpInBadCommunityUses_obj
},
_snmpInBadTypes_obj = {
	"snmpInBadTypes", 7, 0,
	NULL, &_snmpInASNParseErrs_obj
},
_snmpInTooBigs_obj = {
	"snmpInTooBigs", 8, 0,
	NULL, &_snmpInBadTypes_obj
},
_snmpInNoSuchNames_obj = {
	"snmpInNoSuchNames", 9, 0,
	NULL, &_snmpInTooBigs_obj
},
_snmpInBadValues_obj = {
	"snmpInBadValues", 10, 0,
	NULL, &_snmpInNoSuchNames_obj
},
_snmpInReadOnlys_obj = {
	"snmpInReadOnlys", 11, 0,
	NULL, &_snmpInBadValues_obj
},
_snmpInGenErrs_obj = {
	"snmpInGenErrs", 12, 0,
	NULL, &_snmpInReadOnlys_obj
},
_snmpInTotalReqVars_obj = {
	"snmpInTotalReqVars", 13, 0,
	NULL, &_snmpInGenErrs_obj
},
_snmpInTotalSetVars_obj = {
	"snmpInTotalSetVars", 14, 0,
	NULL, &_snmpInTotalReqVars_obj
},
_snmpInGetRequests_obj = {
	"snmpInGetRequests", 15, 0,
	NULL, &_snmpInTotalSetVars_obj
},
_snmpInGetNexts_obj = {
	"snmpInGetNexts", 16, 0,
	NULL, &_snmpInGetRequests_obj
},
_snmpInSetRequests_obj = {
	"snmpInSetRequests", 17, 0,
	NULL, &_snmpInGetNexts_obj
},
_snmpInGetResponses_obj = {
	"snmpInGetResponses", 18, 0,
	NULL, &_snmpInSetRequests_obj
},
_snmpInTraps_obj = {
	"snmpInTraps", 19, 0,
	NULL, &_snmpInGetResponses_obj
},
_snmpOutTooBigs_obj = {
	"snmpOutTooBigs", 20, 0,
	NULL, &_snmpInTraps_obj
},
_snmpOutNoSuchNames_obj = {
	"snmpOutNoSuchNames", 21, 0,
	NULL, &_snmpOutTooBigs_obj
},
_snmpOutBadValues_obj = {
	"snmpOutBadValues", 22, 0,
	NULL, &_snmpOutNoSuchNames_obj
},
_snmpOutReadOnlys_obj = {
	"snmpOutReadOnlys", 23, 0,
	NULL, &_snmpOutBadValues_obj
},
_snmpOutGenErrs_obj = {
	"snmpOutGenErrs", 24, 0,
	NULL, &_snmpOutReadOnlys_obj
},
_snmpOutGetRequests_obj = {
	"snmpOutGetRequests", 25, 0,
	NULL, &_snmpOutGenErrs_obj
},
_snmpOutGetNexts_obj = {
	"snmpOutGetNexts", 26, 0,
	NULL, &_snmpOutGetRequests_obj
},
_snmpOutSetRequests_obj = {
	"snmpOutSetRequests", 27, 0,
	NULL, &_snmpOutGetNexts_obj
},
_snmpOutGetResponses_obj = {
	"snmpOutGetResponses", 28, 0,
	NULL, &_snmpOutSetRequests_obj
},
_snmpOutTraps_obj = {
	"snmpOutTraps", 29, 0,
	NULL, &_snmpOutGetResponses_obj
},
_snmpEnableAuthTraps_obj = {
	"snmpEnableAuthTraps", 30, 0,
	NULL, &_snmpOutTraps_obj
},
_egpNeighState_obj = {
	"egpNeighState", 1, 0,
	NULL, NULL
},
_egpNeighAddr_obj = {
	"egpNeighAddr", 2, 0,
	NULL, &_egpNeighState_obj
},
_egpNeighAs_obj = {
	"egpNeighAs", 3, 0,
	NULL, &_egpNeighAddr_obj
},
_egpNeighInMsgs_obj = {
	"egpNeighInMsgs", 4, 0,
	NULL, &_egpNeighAs_obj
},
_egpNeighInErrs_obj = {
	"egpNeighInErrs", 5, 0,
	NULL, &_egpNeighInMsgs_obj
},
_egpNeighOutMsgs_obj = {
	"egpNeighOutMsgs", 6, 0,
	NULL, &_egpNeighInErrs_obj
},
_egpNeighOutErrs_obj = {
	"egpNeighOutErrs", 7, 0,
	NULL, &_egpNeighOutMsgs_obj
},
_egpNeighInErrMsgs_obj = {
	"egpNeighInErrMsgs", 8, 0,
	NULL, &_egpNeighOutErrs_obj
},
_egpNeighOutErrMsgs_obj = {
	"egpNeighOutErrMsgs", 9, 0,
	NULL, &_egpNeighInErrMsgs_obj
},
_egpNeighStateUps_obj = {
	"egpNeighStateUps", 10, 0,
	NULL, &_egpNeighOutErrMsgs_obj
},
_egpNeighStateDowns_obj = {
	"egpNeighStateDowns", 11, 0,
	NULL, &_egpNeighStateUps_obj
},
_egpNeighIntervalHello_obj = {
	"egpNeighIntervalHello", 12, 0,
	NULL, &_egpNeighStateDowns_obj
},
_egpNeighIntervalPoll_obj = {
	"egpNeighIntervalPoll", 13, 0,
	NULL, &_egpNeighIntervalHello_obj
},
_egpNeighMode_obj = {
	"egpNeighMode", 14, 0,
	NULL, &_egpNeighIntervalPoll_obj
},
_egpNeighEventTrigger_obj = {
	"egpNeighEventTrigger", 15, 0,
	NULL, &_egpNeighMode_obj
},
_egpNeighEntry_obj = {
	"egpNeighEntry", 1, 0,
	&_egpNeighEventTrigger_obj, NULL
},
_egpInMsgs_obj = {
	"egpInMsgs", 1, 0,
	NULL, NULL
},
_egpInErrors_obj = {
	"egpInErrors", 2, 0,
	NULL, &_egpInMsgs_obj
},
_egpOutMsgs_obj = {
	"egpOutMsgs", 3, 0,
	NULL, &_egpInErrors_obj
},
_egpOutErrors_obj = {
	"egpOutErrors", 4, 0,
	NULL, &_egpOutMsgs_obj
},
_egpNeighTable_obj = {
	"egpNeighTable", 5, 0,
	&_egpNeighEntry_obj, &_egpOutErrors_obj
},
_egpAs_obj = {
	"egpAs", 6, 0,
	NULL, &_egpNeighTable_obj
},
_udpLocalAddress_obj = {
	"udpLocalAddress", 1, 0,
	NULL, NULL
},
_udpLocalPort_obj = {
	"udpLocalPort", 2, 0,
	NULL, &_udpLocalAddress_obj
},
_udpEntry_obj = {
	"udpEntry", 1, 0,
	&_udpLocalPort_obj, NULL
},
_udpInDatagrams_obj = {
	"udpInDatagrams", 1, 0,
	NULL, NULL
},
_udpNoPorts_obj = {
	"udpNoPorts", 2, 0,
	NULL, &_udpInDatagrams_obj
},
_udpInErrors_obj = {
	"udpInErrors", 3, 0,
	NULL, &_udpNoPorts_obj
},
_udpOutDatagrams_obj = {
	"udpOutDatagrams", 4, 0,
	NULL, &_udpInErrors_obj
},
_udpTable_obj = {
	"udpTable", 5, 0,
	&_udpEntry_obj, &_udpOutDatagrams_obj
},
_tcpConnState_obj = {
	"tcpConnState", 1, 0,
	NULL, NULL
},
_tcpConnLocalAddress_obj = {
	"tcpConnLocalAddress", 2, 0,
	NULL, &_tcpConnState_obj
},
_tcpConnLocalPort_obj = {
	"tcpConnLocalPort", 3, 0,
	NULL, &_tcpConnLocalAddress_obj
},
_tcpConnRemAddress_obj = {
	"tcpConnRemAddress", 4, 0,
	NULL, &_tcpConnLocalPort_obj
},
_tcpConnRemPort_obj = {
	"tcpConnRemPort", 5, 0,
	NULL, &_tcpConnRemAddress_obj
},
_tcpConnEntry_obj = {
	"tcpConnEntry", 1, 0,
	&_tcpConnRemPort_obj, NULL
},
_tcpRtoAlgorithm_obj = {
	"tcpRtoAlgorithm", 1, 0,
	NULL, NULL
},
_tcpRtoMin_obj = {
	"tcpRtoMin", 2, 0,
	NULL, &_tcpRtoAlgorithm_obj
},
_tcpRtoMax_obj = {
	"tcpRtoMax", 3, 0,
	NULL, &_tcpRtoMin_obj
},
_tcpMaxConn_obj = {
	"tcpMaxConn", 4, 0,
	NULL, &_tcpRtoMax_obj
},
_tcpActiveOpens_obj = {
	"tcpActiveOpens", 5, 0,
	NULL, &_tcpMaxConn_obj
},
_tcpPassiveOpens_obj = {
	"tcpPassiveOpens", 6, 0,
	NULL, &_tcpActiveOpens_obj
},
_tcpAttemptFails_obj = {
	"tcpAttemptFails", 7, 0,
	NULL, &_tcpPassiveOpens_obj
},
_tcpEstabResets_obj = {
	"tcpEstabResets", 8, 0,
	NULL, &_tcpAttemptFails_obj
},
_tcpCurrEstab_obj = {
	"tcpCurrEstab", 9, 0,
	NULL, &_tcpEstabResets_obj
},
_tcpInSegs_obj = {
	"tcpInSegs", 10, 0,
	NULL, &_tcpCurrEstab_obj
},
_tcpOutSegs_obj = {
	"tcpOutSegs", 11, 0,
	NULL, &_tcpInSegs_obj
},
_tcpRetransSegs_obj = {
	"tcpRetransSegs", 12, 0,
	NULL, &_tcpOutSegs_obj
},
_tcpConnTable_obj = {
	"tcpConnTable", 13, 0,
	&_tcpConnEntry_obj, &_tcpRetransSegs_obj
},
_tcpInErrs_obj = {
	"tcpInErrs", 14, 0,
	NULL, &_tcpConnTable_obj
},
_tcpOutRsts_obj = {
	"tcpOutRsts", 15, 0,
	NULL, &_tcpInErrs_obj
},
_icmpInMsgs_obj = {
	"icmpInMsgs", 1, 0,
	NULL, NULL
},
_icmpInErrors_obj = {
	"icmpInErrors", 2, 0,
	NULL, &_icmpInMsgs_obj
},
_icmpInDestUnreachs_obj = {
	"icmpInDestUnreachs", 3, 0,
	NULL, &_icmpInErrors_obj
},
_icmpInTimeExcds_obj = {
	"icmpInTimeExcds", 4, 0,
	NULL, &_icmpInDestUnreachs_obj
},
_icmpInParmProbs_obj = {
	"icmpInParmProbs", 5, 0,
	NULL, &_icmpInTimeExcds_obj
},
_icmpInSrcQuenchs_obj = {
	"icmpInSrcQuenchs", 6, 0,
	NULL, &_icmpInParmProbs_obj
},
_icmpInRedirects_obj = {
	"icmpInRedirects", 7, 0,
	NULL, &_icmpInSrcQuenchs_obj
},
_icmpInEchos_obj = {
	"icmpInEchos", 8, 0,
	NULL, &_icmpInRedirects_obj
},
_icmpInEchoReps_obj = {
	"icmpInEchoReps", 9, 0,
	NULL, &_icmpInEchos_obj
},
_icmpInTimestamps_obj = {
	"icmpInTimestamps", 10, 0,
	NULL, &_icmpInEchoReps_obj
},
_icmpInTimestampReps_obj = {
	"icmpInTimestampReps", 11, 0,
	NULL, &_icmpInTimestamps_obj
},
_icmpInAddrMasks_obj = {
	"icmpInAddrMasks", 12, 0,
	NULL, &_icmpInTimestampReps_obj
},
_icmpInAddrMaskReps_obj = {
	"icmpInAddrMaskReps", 13, 0,
	NULL, &_icmpInAddrMasks_obj
},
_icmpOutMsgs_obj = {
	"icmpOutMsgs", 14, 0,
	NULL, &_icmpInAddrMaskReps_obj
},
_icmpOutErrors_obj = {
	"icmpOutErrors", 15, 0,
	NULL, &_icmpOutMsgs_obj
},
_icmpOutDestUnreachs_obj = {
	"icmpOutDestUnreachs", 16, 0,
	NULL, &_icmpOutErrors_obj
},
_icmpOutTimeExcds_obj = {
	"icmpOutTimeExcds", 17, 0,
	NULL, &_icmpOutDestUnreachs_obj
},
_icmpOutParmProbs_obj = {
	"icmpOutParmProbs", 18, 0,
	NULL, &_icmpOutTimeExcds_obj
},
_icmpOutSrcQuenchs_obj = {
	"icmpOutSrcQuenchs", 19, 0,
	NULL, &_icmpOutParmProbs_obj
},
_icmpOutRedirects_obj = {
	"icmpOutRedirects", 20, 0,
	NULL, &_icmpOutSrcQuenchs_obj
},
_icmpOutEchos_obj = {
	"icmpOutEchos", 21, 0,
	NULL, &_icmpOutRedirects_obj
},
_icmpOutEchoReps_obj = {
	"icmpOutEchoReps", 22, 0,
	NULL, &_icmpOutEchos_obj
},
_icmpOutTimestamps_obj = {
	"icmpOutTimestamps", 23, 0,
	NULL, &_icmpOutEchoReps_obj
},
_icmpOutTimestampReps_obj = {
	"icmpOutTimestampReps", 24, 0,
	NULL, &_icmpOutTimestamps_obj
},
_icmpOutAddrMasks_obj = {
	"icmpOutAddrMasks", 25, 0,
	NULL, &_icmpOutTimestampReps_obj
},
_icmpOutAddrMaskReps_obj = {
	"icmpOutAddrMaskReps", 26, 0,
	NULL, &_icmpOutAddrMasks_obj
},
_ipNetToMediaIfIndex_obj = {
	"ipNetToMediaIfIndex", 1, 0,
	NULL, NULL
},
_ipNetToMediaPhysAddress_obj = {
	"ipNetToMediaPhysAddress", 2, 0,
	NULL, &_ipNetToMediaIfIndex_obj
},
_ipNetToMediaNetAddress_obj = {
	"ipNetToMediaNetAddress", 3, 0,
	NULL, &_ipNetToMediaPhysAddress_obj
},
_ipNetToMediaType_obj = {
	"ipNetToMediaType", 4, 0,
	NULL, &_ipNetToMediaNetAddress_obj
},
_ipNetToMediaEntry_obj = {
	"ipNetToMediaEntry", 1, 0,
	&_ipNetToMediaType_obj, NULL
},
_ipRouteDest_obj = {
	"ipRouteDest", 1, 0,
	NULL, NULL
},
_ipRouteIfIndex_obj = {
	"ipRouteIfIndex", 2, 0,
	NULL, &_ipRouteDest_obj
},
_ipRouteMetric1_obj = {
	"ipRouteMetric1", 3, 0,
	NULL, &_ipRouteIfIndex_obj
},
_ipRouteMetric2_obj = {
	"ipRouteMetric2", 4, 0,
	NULL, &_ipRouteMetric1_obj
},
_ipRouteMetric3_obj = {
	"ipRouteMetric3", 5, 0,
	NULL, &_ipRouteMetric2_obj
},
_ipRouteMetric4_obj = {
	"ipRouteMetric4", 6, 0,
	NULL, &_ipRouteMetric3_obj
},
_ipRouteNextHop_obj = {
	"ipRouteNextHop", 7, 0,
	NULL, &_ipRouteMetric4_obj
},
_ipRouteType_obj = {
	"ipRouteType", 8, 0,
	NULL, &_ipRouteNextHop_obj
},
_ipRouteProto_obj = {
	"ipRouteProto", 9, 0,
	NULL, &_ipRouteType_obj
},
_ipRouteAge_obj = {
	"ipRouteAge", 10, 0,
	NULL, &_ipRouteProto_obj
},
_ipRouteMask_obj = {
	"ipRouteMask", 11, 0,
	NULL, &_ipRouteAge_obj
},
_ipRouteEntry_obj = {
	"ipRouteEntry", 1, 0,
	&_ipRouteMask_obj, NULL
},
_ipAdEntAddr_obj = {
	"ipAdEntAddr", 1, 0,
	NULL, NULL
},
_ipAdEntIfIndex_obj = {
	"ipAdEntIfIndex", 2, 0,
	NULL, &_ipAdEntAddr_obj
},
_ipAdEntNetMask_obj = {
	"ipAdEntNetMask", 3, 0,
	NULL, &_ipAdEntIfIndex_obj
},
_ipAdEntBcastAddr_obj = {
	"ipAdEntBcastAddr", 4, 0,
	NULL, &_ipAdEntNetMask_obj
},
_ipAdEntReasmMaxSize_obj = {
	"ipAdEntReasmMaxSize", 5, 0,
	NULL, &_ipAdEntBcastAddr_obj
},
_ipAddrEntry_obj = {
	"ipAddrEntry", 1, 0,
	&_ipAdEntReasmMaxSize_obj, NULL
},
_ipForwarding_obj = {
	"ipForwarding", 1, 0,
	NULL, NULL
},
_ipDefaultTTL_obj = {
	"ipDefaultTTL", 2, 0,
	NULL, &_ipForwarding_obj
},
_ipInReceives_obj = {
	"ipInReceives", 3, 0,
	NULL, &_ipDefaultTTL_obj
},
_ipInHdrErrors_obj = {
	"ipInHdrErrors", 4, 0,
	NULL, &_ipInReceives_obj
},
_ipInAddrErrors_obj = {
	"ipInAddrErrors", 5, 0,
	NULL, &_ipInHdrErrors_obj
},
_ipForwDatagrams_obj = {
	"ipForwDatagrams", 6, 0,
	NULL, &_ipInAddrErrors_obj
},
_ipInUnknownProtos_obj = {
	"ipInUnknownProtos", 7, 0,
	NULL, &_ipForwDatagrams_obj
},
_ipInDiscards_obj = {
	"ipInDiscards", 8, 0,
	NULL, &_ipInUnknownProtos_obj
},
_ipInDelivers_obj = {
	"ipInDelivers", 9, 0,
	NULL, &_ipInDiscards_obj
},
_ipOutRequests_obj = {
	"ipOutRequests", 10, 0,
	NULL, &_ipInDelivers_obj
},
_ipOutDiscards_obj = {
	"ipOutDiscards", 11, 0,
	NULL, &_ipOutRequests_obj
},
_ipOutNoRoutes_obj = {
	"ipOutNoRoutes", 12, 0,
	NULL, &_ipOutDiscards_obj
},
_ipReasmTimeout_obj = {
	"ipReasmTimeout", 13, 0,
	NULL, &_ipOutNoRoutes_obj
},
_ipReasmReqds_obj = {
	"ipReasmReqds", 14, 0,
	NULL, &_ipReasmTimeout_obj
},
_ipReasmOKs_obj = {
	"ipReasmOKs", 15, 0,
	NULL, &_ipReasmReqds_obj
},
_ipReasmFails_obj = {
	"ipReasmFails", 16, 0,
	NULL, &_ipReasmOKs_obj
},
_ipFragOKs_obj = {
	"ipFragOKs", 17, 0,
	NULL, &_ipReasmFails_obj
},
_ipFragFails_obj = {
	"ipFragFails", 18, 0,
	NULL, &_ipFragOKs_obj
},
_ipFragCreates_obj = {
	"ipFragCreates", 19, 0,
	NULL, &_ipFragFails_obj
},
_ipAddrTable_obj = {
	"ipAddrTable", 20, 0,
	&_ipAddrEntry_obj, &_ipFragCreates_obj
},
_ipRoutingTable_obj = {
	"ipRoutingTable", 21, 0,
	&_ipRouteEntry_obj, &_ipAddrTable_obj
},
_ipNetToMediaTable_obj = {
	"ipNetToMediaTable", 22, 0,
	&_ipNetToMediaEntry_obj, &_ipRoutingTable_obj
},
_atIfIndex_obj = {
	"atIfIndex", 1, 0,
	NULL, NULL
},
_atPhysAddress_obj = {
	"atPhysAddress", 2, 0,
	NULL, &_atIfIndex_obj
},
_atNetAddress_obj = {
	"atNetAddress", 3, 0,
	NULL, &_atPhysAddress_obj
},
_atEntry_obj = {
	"atEntry", 1, 0,
	&_atNetAddress_obj, NULL
},
_atTable_obj = {
	"atTable", 1, 0,
	&_atEntry_obj, NULL
},
_ifIndex_obj = {
	"ifIndex", 1, 0,
	NULL, NULL
},
_ifDescr_obj = {
	"ifDescr", 2, 0,
	NULL, &_ifIndex_obj
},
_ifType_obj = {
	"ifType", 3, 0,
	NULL, &_ifDescr_obj
},
_ifMtu_obj = {
	"ifMtu", 4, 0,
	NULL, &_ifType_obj
},
_ifSpeed_obj = {
	"ifSpeed", 5, 0,
	NULL, &_ifMtu_obj
},
_ifPhysAddress_obj = {
	"ifPhysAddress", 6, 0,
	NULL, &_ifSpeed_obj
},
_ifAdminStatus_obj = {
	"ifAdminStatus", 7, 0,
	NULL, &_ifPhysAddress_obj
},
_ifOperStatus_obj = {
	"ifOperStatus", 8, 0,
	NULL, &_ifAdminStatus_obj
},
_ifLastChange_obj = {
	"ifLastChange", 9, 0,
	NULL, &_ifOperStatus_obj
},
_ifInOctets_obj = {
	"ifInOctets", 10, 0,
	NULL, &_ifLastChange_obj
},
_ifInUcastPkts_obj = {
	"ifInUcastPkts", 11, 0,
	NULL, &_ifInOctets_obj
},
_ifInNUcastPkts_obj = {
	"ifInNUcastPkts", 12, 0,
	NULL, &_ifInUcastPkts_obj
},
_ifInDiscards_obj = {
	"ifInDiscards", 13, 0,
	NULL, &_ifInNUcastPkts_obj
},
_ifInErrors_obj = {
	"ifInErrors", 14, 0,
	NULL, &_ifInDiscards_obj
},
_ifInUnknownProtos_obj = {
	"ifInUnknownProtos", 15, 0,
	NULL, &_ifInErrors_obj
},
_ifOutOctets_obj = {
	"ifOutOctets", 16, 0,
	NULL, &_ifInUnknownProtos_obj
},
_ifOutUcastPkts_obj = {
	"ifOutUcastPkts", 17, 0,
	NULL, &_ifOutOctets_obj
},
_ifOutNUcastPkts_obj = {
	"ifOutNUcastPkts", 18, 0,
	NULL, &_ifOutUcastPkts_obj
},
_ifOutDiscards_obj = {
	"ifOutDiscards", 19, 0,
	NULL, &_ifOutNUcastPkts_obj
},
_ifOutErrors_obj = {
	"ifOutErrors", 20, 0,
	NULL, &_ifOutDiscards_obj
},
_ifOutQLen_obj = {
	"ifOutQLen", 21, 0,
	NULL, &_ifOutErrors_obj
},
_ifSpecific_obj = {
	"ifSpecific", 22, 0,
	NULL, &_ifOutQLen_obj
},
_ifEntry_obj = {
	"ifEntry", 1, 0,
	&_ifSpecific_obj, NULL
},
_ifNumber_obj = {
	"ifNumber", 1, 0,
	NULL, NULL
},
_ifTable_obj = {
	"ifTable", 2, 0,
	&_ifEntry_obj, &_ifNumber_obj
},
_sysDescr_obj = {
	"sysDescr", 1, 0,
	NULL, NULL
},
_sysObjectID_obj = {
	"sysObjectID", 2, 0,
	NULL, &_sysDescr_obj
},
_sysUpTime_obj = {
	"sysUpTime", 3, 0,
	NULL, &_sysObjectID_obj
},
_sysContact_obj = {
	"sysContact", 4, 0,
	NULL, &_sysUpTime_obj
},
_sysName_obj = {
	"sysName", 5, 0,
	NULL, &_sysContact_obj
},
_sysLocation_obj = {
	"sysLocation", 6, 0,
	NULL, &_sysName_obj
},
_sysServices_obj = {
	"sysServices", 7, 0,
	NULL, &_sysLocation_obj
},
_system_obj = {
	"system", 1, 0,
	&_sysServices_obj, NULL
},
_interfaces_obj = {
	"interfaces", 2, 0,
	&_ifTable_obj, &_system_obj
},
_at_obj = {
	"at", 3, 0,
	&_atTable_obj, &_interfaces_obj
},
_ip_obj = {
	"ip", 4, 0,
	&_ipNetToMediaTable_obj, &_at_obj
},
_icmp_obj = {
	"icmp", 5, 0,
	&_icmpOutAddrMaskReps_obj, &_ip_obj
},
_tcp_obj = {
	"tcp", 6, 0,
	&_tcpOutRsts_obj, &_icmp_obj
},
_udp_obj = {
	"udp", 7, 0,
	&_udpTable_obj, &_tcp_obj
},
_egp_obj = {
	"egp", 8, 0,
	&_egpAs_obj, &_udp_obj
},
_transmission_obj = {
	"transmission", 10, 0,
	NULL, &_egp_obj
},
_snmp_obj = {
	"snmp", 11, 0,
	&_snmpEnableAuthTraps_obj, &_transmission_obj
},
_mib_obj = {
	"mib", 1, 0,
	&_snmp_obj, NULL
},
_directory_obj = {
	"directory", 1, 0,
	NULL, NULL
},
_mgmt_obj = {
	"mgmt", 2, 0,
	&_mib_obj, &_directory_obj
},
_experimental_obj = {
	"experimental", 3, 0,
	NULL, &_mgmt_obj
},
_private_obj = {
	"private", 4, 0,
	&_enterprises_obj, &_experimental_obj
},
_internet_obj = {
	"internet", 1, 0,
	&_private_obj, NULL
},
_dod_obj = {
	"dod", 6, 0,
	&_internet_obj, NULL
},
_org_obj = {
	"org", 3, 0,
	&_dod_obj, NULL
},
_iso_obj = {
	"iso", 1, 0,
	&_org_obj, NULL
},
*mibroot = &_iso_obj;

/*
 * This software is Copyright (c) 2019 Denis Burykin
 * [denis_burykin yahoo com], [denis-burykin2014 yandex ru]
 * and it is hereby released to the general public under the following terms:
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 */

#include "../list.h"
#include "../options.h"

#include "fpga_common.h"
#include "ztex/ztex_sn.h"
#include "ulx3s/ulx3s_sn.h"
#include "ztex/ztex_scan.h"
#include "ulx3s/ulx3s_scan.h"


struct list_main *ztex_detected_list;
struct list_main *ulx3s_detected_list;
struct list_main *ztex_use_list;
struct list_main *ulx3s_use_list;
int ztex_devices_per_fork;
int ulx3s_devices_per_fork;
int ztex_fork_num;
int ulx3s_fork_num;

#ifdef HAVE_FPGA_ZTEX
void ztex_init()
{
	static int ztex_initialized;

#ifdef HAVE_FUZZ
	if (options.flags & FLG_FUZZ_CHK)
		return;
#endif

	if (ztex_initialized)
		return;

	// Initialize [List.ZTEX:Devices] configuration section.
	ztex_sn_init_conf_devices();

	// Check -dev command-line option.
	struct list_entry *entry;
	int found_error = 0;
	for (entry = options.acc_devices->head; entry; entry = entry->next) {
		if (ztex_sn_alias_is_valid(entry->data)) {
			if (!ztex_sn_check_alias(entry->data))
				found_error = 1;
		}
		else if (!ztex_sn_is_valid(entry->data)) {
			fprintf(stderr, "Error: bad Serial Number '%s'\n", entry->data);
			found_error = 1;
		}
	}
	if (found_error)
		error();

	ztex_detect(options.acc_devices, &ztex_detected_list);
	ztex_use_list = ztex_detected_list;
	ztex_initialized = 1;
}
#endif

#ifdef HAVE_FPGA_ULX3S
void ulx3s_init()
{
	int found_error = 0;
	fprintf(stderr, "Error: we actually hit this\n");
	found_error = 1;
}
#endif
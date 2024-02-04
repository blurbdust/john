/*
 * This software is Copyright (c) 2019 Denis Burykin
 * [denis_burykin yahoo com], [denis-burykin2014 yandex ru]
 * and it is hereby released to the general public under the following terms:
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ulx3s_sn.h"

#include "../../config.h"
#include "../../list.h"
#include "../../misc.h"
#include "../../common.h"

int ulx3s_sn_is_valid(char *sn)
{
	int i;
	for (i = 0; i < ULX3S_SNSTRING_LEN; i++) {
		if (!sn[i])
			return i < ULX3S_SNSTRING_MIN_LEN ? 0 : 1;
		if ( !( (sn[i] >= '0' && sn[i] <= '9') || (sn[i] >= 'A' && sn[i] <= 'F')
				|| (sn[i] >= 'a' && sn[i] <= 'f')) )
			return 0;
	}

	return 0;
}


struct cfg_list *ulx3s_sn_conf_devices = NULL;

void ulx3s_sn_init_conf_devices(void)
{
	if (ulx3s_sn_conf_devices)
		return;

	ulx3s_sn_conf_devices = cfg_get_list("List.ULX3S:", "Devices");
	if (!ulx3s_sn_conf_devices)
		return;

	struct cfg_line *line;
	int found_error = 0;
	for (line = ulx3s_sn_conf_devices->head; line; line = line->next) {
		if (!ulx3s_sn_is_valid(line->data)) {
			fprintf(stderr, "Error: [List.ULX3S:Devices] device %d "
				"(line %d) bad Serial Number '%s'\n",
				line->id + 1, line->number, line->data);
			found_error = 1;
		}
	}

	if (found_error)
		error();
}


int ulx3s_sn_alias_is_valid(char *alias)
{
	const int num_digits_max = 4;
	int i;

	if (!alias[0])
		return 0;
	for (i = 0; i <= num_digits_max; i++) {
		if (!alias[i])
			return 1;
		if (alias[i] < '0' || alias[i] > '9')
			return 0;
	}
	return 0;
}


int ulx3s_sn_check_alias(char *alias)
{
	static int ulx3s_sn_get_by_alias_error = 0;

	if (!ulx3s_sn_conf_devices) {
		if (!ulx3s_sn_get_by_alias_error) {
			fprintf(stderr, "Error: [List.ULX3S:Devices] not found\n");
			ulx3s_sn_get_by_alias_error = 1;
		}
		return 0;
	}

	if (!ulx3s_sn_alias_is_valid(alias)) {
		fprintf(stderr, "Error: invalid device alias '%s'\n", alias);
		return 0;
	}
	int id = atoi(alias);
	if (!id) {
		fprintf(stderr, "Error: invalid -dev=0. Devices are numbered "
			"starting from 1.\n");
		return 0;
	}

	struct cfg_line *line;
	int last_id = 0;
	for (line = ulx3s_sn_conf_devices->head; line; line = line->next) {
		if (id == line->id + 1)
			return 1;
		if (!line->next)
			last_id = line->id + 1;
	}

	fprintf(stderr, "Error: invalid -dev=%d. Total %d devices are "
		"defined in [List.ULX3S:Devices]\n", id, last_id);
	ulx3s_sn_get_by_alias_error = 1;
	return 0;
}


char *ulx3s_sn_get_by_sn_orig(char *sn_orig)
{
	if (!ulx3s_sn_conf_devices)
		return sn_orig;

	struct cfg_line *line;
	int found = 0;
	for (line = ulx3s_sn_conf_devices->head; line; line = line->next) {
		if (!strncmp(line->data, sn_orig, ULX3S_SNSTRING_LEN)) {
			found = 1;
			break;
		}
	}
	if (!found)
		return sn_orig;

	static char result[ULX3S_SNSTRING_LEN];
	snprintf(result, sizeof(result), "%u", line->id + 1);
	return result;
}

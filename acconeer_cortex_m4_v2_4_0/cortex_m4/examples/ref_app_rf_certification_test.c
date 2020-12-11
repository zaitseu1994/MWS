// Copyright (c) Acconeer AB, 2019-2020
// All rights reserved

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "acc_hal_definitions.h"
#include "acc_hal_integration.h"
#include "acc_rf_certification_test.h"
#include "acc_rss.h"
#include "acc_version.h"


bool acc_ref_app_rf_certification_test(void);


bool acc_ref_app_rf_certification_test(void)
{
	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_t *hal = acc_hal_integration_get_implementation();

	if (!acc_rss_activate(hal))
	{
		fprintf(stderr, "Failed to activate RSS\n");
		return false;
	}

	bool     tx_disable = false;
	uint32_t iterations = 0; // 0 means 'run forever'

	bool success = acc_rf_certification_test(tx_disable, iterations);

	acc_rss_deactivate();

	return success;
}

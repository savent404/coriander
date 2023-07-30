/**
 * @file coriander.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-30
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef void* coriander_instance_t;

coriander_instance_t create_coriander_instance(void);
void coriander_run(coriander_instance_t instance);
void destroy_coriander_instance(coriander_instance_t instance);

#ifdef __cplusplus
}
#endif

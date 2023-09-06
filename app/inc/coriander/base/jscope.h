/**
 * @file jscope.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-09-08
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#if CONFIG_JSCOPE_ENABLE
#define ATTR_JSCOPE \
  __attribute__((section(".data.jscope"))) __attribute__((used))
#else
#define ATTR_JSCOPE
#endif

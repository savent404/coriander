# SPDX-License-Identifier: All Rights Reserved

board_runner_args(jlink "--device=STM32F405RG" "--speed=4000")

include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)
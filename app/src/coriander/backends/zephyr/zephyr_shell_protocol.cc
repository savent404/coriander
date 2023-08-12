/**
 * @file zephyr_shell_protocol.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-12
 *
 * Copyright 2023 savent_gate
 *
 */
#include "zephyr_shell_protocol.h"

#include <zephyr/fs/fs.h>
#include <zephyr/shell/shell.h>

namespace {
static std::shared_ptr<coriander::BoardCtl> ctl = nullptr;
}

namespace coriander {
namespace zephyr {
void ShellProtocol::enable() { ctl = mBoardCtl; }
void ShellProtocol::disable() { ctl = nullptr; }
void ShellProtocol::loop() {}
}  // namespace zephyr
}  // namespace coriander

extern "C" {
static int shell_enable_motor(const struct shell* shell, size_t argc,
                              char** argv) {
  if (!ctl) {
    return -1;
  }
  ctl->motorEnable();
  return 0;
}

static int shell_disable_motor(const struct shell* shell, size_t argc,
                               char** argv) {
  if (!ctl) {
    return -1;
  }
  ctl->motorDisable();
  return 0;
}

static int shell_fs_ls(const struct shell* shell, size_t argc, char** argv) {
  struct fs_dir_t dir = {0};
  struct fs_dirent dentry;
  const char* path = argv[1];
  int ret;

  if (argc != 2) {
    shell_print(shell, "Usage: ls <dir>");
    return -EINVAL;
  }

  ret = fs_opendir(&dir, path);
  if (ret != 0) {
    shell_print(shell, "opendir err:%d", ret);
    return ret;
  }

  do {
    ret = fs_readdir(&dir, &dentry);
    if (ret != 0) {
      shell_print(shell, "readdir err:%d", ret);
      return ret;
    }
    if (!dentry.name[0]) {
      break;
    }

    if (dentry.type == FS_DIR_ENTRY_FILE) {
      shell_print(shell, "file\t%u\t%s", dentry.size, dentry.name);
    } else {
      shell_print(shell, "dir\t%s", dentry.name);
    }
  } while (ret == 0 && dentry.name[0]);

  fs_closedir(&dir);
  return 0;
}

static int shell_fs_cat(const struct shell* shell, size_t argc, char** argv) {
  struct fs_file_t f = {0};
  int ret;

  if (argc != 2) {
    shell_print(shell, "Usage: cat <file>");
    return -EINVAL;
  }

  ret = fs_open(&f, argv[1], FS_O_READ);
  if (ret != 0) {
    shell_print(shell, "open err:%d", ret);
    return ret;
  }

  while (1) {
    char buf[64];
    ssize_t br = fs_read(&f, buf, sizeof(buf));

    if (br <= 0) {
      break;
    }

    shell_fprintf(shell, SHELL_NORMAL, "%.*s", br, buf);
  }

  fs_close(&f);
  return 0;
}

static int shell_fs_rm(const struct shell* shell, size_t argc, char** argv) {
  int ret;
  if (argc != 2) {
    shell_print(shell, "Usage: rm <file/dir>");
    return -EINVAL;
  }

  ret = fs_unlink(argv[1]);
  if (ret != 0) {
    shell_print(shell, "unlink err:%d", ret);
  }
  return ret;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
    motor_cmd,
    SHELL_CMD_ARG(enable, NULL, "enable motor", shell_enable_motor, 1, 0),
    SHELL_CMD_ARG(disable, NULL, "disable motor", shell_disable_motor, 1, 0),
    SHELL_SUBCMD_SET_END /* Array terminated. */
);

SHELL_STATIC_SUBCMD_SET_CREATE(
    fs_cmd, SHELL_CMD_ARG(ls, NULL, "list files", shell_fs_ls, 2, 0),
    SHELL_CMD_ARG(cat, NULL, "read file", shell_fs_cat, 2, 0),
    SHELL_CMD_ARG(rm, NULL, "remove file", shell_fs_rm, 2, 0));

SHELL_CMD_REGISTER(motor, &motor_cmd, "motor control", NULL);
SHELL_CMD_REGISTER(fs, &fs_cmd, "fs ops", NULL);
}
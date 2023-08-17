/**
 * @file exflash_shell.c
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-17
 *
 * Copyright 2023 savent_gate
 *
 */
#include <zephyr/fs/fs.h>
#include <zephyr/fs/littlefs.h>
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/storage/flash_map.h>

#define MKFS_FS_TYPE FS_LITTLEFS
#define MKFS_DEV_ID FIXED_PARTITION_ID(fs_partition)
#define MKFS_FLASH 0

static int shell_fs_format(const struct shell* shell, size_t argc,
                           char** argv) {
  int ret;

#if CONFIG_FILE_SYSTEM_MKFS
  if (argc != 2) {
    shell_print(shell, "Usage: format <fs_type>");
    return -EINVAL;
  }

  if (strcmp(argv[1], "littlefs") == 0) {
    ret = fs_mkfs(MKFS_FS_TYPE, MKFS_DEV_ID, NULL, NULL);
    if (ret != 0) {
      shell_print(shell, "mkfs err:%d", ret);
      return ret;
    }
  } else {
    shell_print(shell, "fs type not support");
    shell_print(shell, "support fs_type: littlefs");
    return -EINVAL;
  }
  return 0;
#else
  shell_print(shell, "mkfs not support");
  return -ENOTSUP;
#endif
}
static int shell_fs_fstat(const struct shell* shell, size_t argc, char** argv) {
  struct fs_statvfs stat = {0};
  int ret;

  if (argc != 2) {
    shell_print(shell, "Usage: fstat <dir>");
    return -EINVAL;
  }

  ret = fs_statvfs(argv[1], &stat);
  if (ret != 0) {
    shell_print(shell, "statvfs err:%d", ret);
    return ret;
  }

  shell_print(shell, "f_bsize:%lu", stat.f_bsize);
  shell_print(shell, "f_frsize:%lu", stat.f_frsize);
  shell_print(shell, "f_blocks:%lu", stat.f_blocks);
  shell_print(shell, "f_bfree:%lu", stat.f_bfree);

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
    fs_cmd,
    SHELL_CMD_ARG(fstat, NULL, "stat file system", shell_fs_fstat, 2, 0),
    SHELL_CMD_ARG(ls, NULL, "list files", shell_fs_ls, 2, 0),
    SHELL_CMD_ARG(cat, NULL, "read file", shell_fs_cat, 2, 0),
    SHELL_CMD_ARG(rm, NULL, "remove file", shell_fs_rm, 2, 0),
    SHELL_CMD_ARG(format, NULL, "format file system", shell_fs_format, 2, 0),
    SHELL_SUBCMD_SET_END /* Array terminated. */);

SHELL_CMD_REGISTER(fs, &fs_cmd, "fs ops", NULL);

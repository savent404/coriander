import os
import re
import sys

# 指定目录路径
directory = "./"

# 文件后缀列表
extensions = [".h", ".c", ".cpp", ".hpp", ".cc", ".in"]

# 关键字
keyword = "ATTR_JSCOPE"

# 变量名的正则表达式模式
pattern = r"ATTR_JSCOPE.*?(_d\w+).*?;"

# 保存符合条件的变量名列表
variables = []

def find_variables(file_path):
    with open(file_path, 'r') as file:
        data = file.read()
        matches = re.findall(pattern, data)
        for match in matches:
            variables.append(match)

# 递归遍历目录
for root, dirs, files in os.walk(directory):
    for file in files:
        # 检查文件后缀是否满足要求
        if file.endswith(tuple(extensions)):
            file_path = os.path.join(root, file)
            find_variables(file_path)

# 读取vars.txt文件中的变量及说明
vars_file = "jscope_vars.txt"
vars_dict = {}

with open(vars_file, 'r') as file:
    lines = file.readlines()
    for line in lines:
        # 获取变量名和说明
        variable, description = line.strip().split(" - ")
        vars_dict[variable] = description

# 输出找到的变量及提示信息
status = True
for variable in variables:
    if variable in vars_dict:
        print("[INFO] Variable {} found with description: {}".format(variable, vars_dict[variable]))
    else:
        print("[ERROR] Variable {} not found in jscope_vars.txt".format(variable))
        status = False
if status == False:
    sys.exit(1)
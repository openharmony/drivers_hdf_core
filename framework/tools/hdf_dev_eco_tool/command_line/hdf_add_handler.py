#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2020-2021 Huawei Device Co., Ltd.
# 
# HDF is dual licensed: you can use it either under the terms of
# the GPL, or the BSD license, at your option.
# See the LICENSE file in the root of this repository for complete details.


import json
from string import Template
import os

import hdf_utils
from hdf_tool_settings import HdfToolSettings
from hdf_tool_exception import HdfToolException
from command_line.operate_group_passwd import OperateGroupPasswd
from .driver_add.hdf_add_driver import HdfAddDriver
from .hdf_command_handler_base import HdfCommandHandlerBase
from .hdf_command_error_code import CommandErrorCode
from .hdf_device_info_hcs import HdfDeviceInfoHcsFile
from .hdf_get_handler import HdfGetHandler
from .hdf_vendor_build_file import HdfVendorBuildFile
from .hdf_vendor_kconfig_file import HdfVendorKconfigFile
from .hdf_vendor_mk_file import HdfVendorMkFile
from .hdf_driver_config_file import HdfDriverConfigFile
from .hdf_vendor_makefile import HdfVendorMakeFile
from .hdf_defconfig_patch import HdfDefconfigAndPatch


class HdfAddHandler(HdfCommandHandlerBase):
    def __init__(self, args):
        super(HdfAddHandler, self).__init__()
        self.cmd = 'add'
        self.handlers = {
            'vendor': self._add_vendor_handler,
            'module': self._add_module_handler,
            'driver': self._add_driver_handler,
            'config': self._add_config_handler,
        }
        self.parser.add_argument("--action_type",
                                 help=' '.join(self.handlers.keys()),
                                 required=True)
        self.parser.add_argument("--root_dir", required=True)
        self.parser.add_argument("--vendor_name")
        self.parser.add_argument("--module_name")
        self.parser.add_argument("--driver_name")
        self.parser.add_argument("--board_name")
        self.parser.add_argument("--kernel_name")
        self.parser.add_argument("--runmode")
        self.parser.add_argument("--device_name")
        self.args_original = args
        self.args = self.parser.parse_args(args)
        self.hdf_tool = HdfToolSettings()

    @staticmethod
    def _render(template_path, output_path, data_model):
        if not os.path.exists(template_path):
            return
        raw_content = hdf_utils.read_file(template_path)
        contents = Template(raw_content).safe_substitute(data_model)
        hdf_utils.write_file(output_path, contents)

    def _file_gen_lite(self, template, out_dir, filename, model):
        templates_dir = hdf_utils.get_templates_lite_dir()
        template_path = os.path.join(templates_dir, template)
        file_path = os.path.join(out_dir, filename)
        self._render(template_path, file_path, model)

    def _add_vendor_handler(self):
        self.check_arg_raise_if_not_exist("vendor_name")
        root, vendor, _, _, board = self.get_args()
        target_dir = hdf_utils.get_vendor_hdf_dir(root, vendor)
        if os.path.exists(target_dir):
            raise HdfToolException(
                "%s already exists" %
                target_dir, CommandErrorCode.TARGET_ALREADY_EXIST)
        os.makedirs(target_dir)
        self._file_gen_lite('hdf_vendor_kconfig.template', target_dir,
                            'Kconfig', {})
        board_parent_path = self.hdf_tool.get_board_parent_path(board)
        if not board_parent_path:
            board_parent_path = 'vendor/hisilicon'
        data_model = {
            "board_parent_path": board_parent_path
        }
        self._file_gen_lite('hdf_vendor_mk.template', target_dir,
                            'hdf_vendor.mk', data_model)

    def _add_module_handler(self):
        self.check_arg_raise_if_not_exist("vendor_name")
        self.check_arg_raise_if_not_exist("module_name")
        self.check_arg_raise_if_not_exist("kernel_name")
        self.check_arg_raise_if_not_exist("board_name")
        self.check_arg_raise_if_not_exist("driver_name")
        args_tuple = self.get_args()
        root, vendor, module, driver, board, kernel, _ = args_tuple
        board_list = self.hdf_tool.get_board_list()
        if board not in board_list:
            raise HdfToolException(
                'supported boards name : %s not exits ' % board)
        framework_hdf = hdf_utils.get_vendor_hdf_dir_framework(root)
        if not os.path.exists(framework_hdf):
            raise HdfToolException(
                ' framework model path  "%s" not exist' %
                framework_hdf, CommandErrorCode.TARGET_NOT_EXIST)
        # framework create file .c
        framework_drv_root_dir = hdf_utils.get_drv_root_dir(
            root, module)
        if os.path.exists(framework_drv_root_dir):
            raise HdfToolException('module "%s" already exist' % module,
                                   CommandErrorCode.TARGET_ALREADY_EXIST)
        os.makedirs(framework_drv_root_dir)
        # create .c template driver file
        state, driver_file_path = self._add_driver(*args_tuple)
        if not state:
            raise HdfToolException(
                'create drivers file fail  "%s" ' %
                driver_file_path.split("\\")[-1])
        config_item, config_file_out = \
            self.diff_type_module_add_operation(
                framework_hdf, driver_file_path, args_tuple)
        config_name = "create_model.config"
        config_file = hdf_utils.read_file(
            os.path.join('resources', config_name))
        config_file_json = json.loads(config_file)
        config_file_json[module] = config_file_out
        hdf_utils.write_config(
            root_path=root, config_file_json=config_file_json,
            config_name=config_name)
        return json.dumps(config_item)

    def diff_type_module_add_operation(self, framework_hdf,
                                       driver_file_path, args_tuple):
        root, vendor, module, driver, board, kernel, _ = args_tuple
        converter = hdf_utils.WordsConverter(self.args.module_name)
        driver_name_converter = hdf_utils.WordsConverter(self.args.driver_name)
        if board.endswith("user"):
            file_path, model_level_config = \
                self._add_module_handler_linux_user(
                    framework_hdf, driver_file_path,
                    *args_tuple)
        else:
            adapter_hdf = hdf_utils.get_vendor_hdf_dir_adapter(root, kernel)
            if not os.path.exists(adapter_hdf):
                raise HdfToolException(
                    ' adapter model path  "%s" not exist' %
                    adapter_hdf, CommandErrorCode.TARGET_NOT_EXIST)
            # create module folder under the adapter path
            adapter_model_path = os.path.join(adapter_hdf, 'model', module)
            if not os.path.exists(adapter_model_path):
                os.makedirs(adapter_model_path)
            data_model = {
                "module_upper_case": converter.upper_case(),
                "module_lower_case": converter.lower_case(),
                "driver_file_name": ("%s_driver.c" % driver_name_converter.lower_case()),
                "driver_name": driver_name_converter.lower_case()
            }
            # create files in the module under the adapter
            if kernel == 'liteos':
                file_path, model_level_config = \
                    self._add_module_handler_liteos(
                        framework_hdf, adapter_model_path,
                        data_model, converter, *args_tuple)
            elif kernel == "linux":
                file_path, model_level_config = \
                    self._add_module_handler_linux(
                        framework_hdf, adapter_model_path,
                        data_model, *args_tuple)
            else:
                model_level_config = ""
        base_config = {
            'module_name': module,
            'module_path': file_path,
            'driver_name': "%s_driver" % driver,
            'driver_file_path': driver_file_path,
        }
        config_item = {'enabled': True}
        config_item.update(base_config)
        config_file_out = {'module_level_config_path': model_level_config}
        config_file_out.update(base_config)
        return config_item, config_file_out

    def _add_module_handler_liteos(self, framework_hdf, adapter_model_path,
                                   data_model, converter, *args_tuple):
        root, vendor, module, driver, board, kernel, _ = args_tuple
        liteos_file_path = {}
        liteos_level_config_file_path = {}
        liteos_file_name = ['BUILD.gn', 'Kconfig', 'Makefile']
        temp_path = self.hdf_tool.get_template_path()
        template_path = os.path.join(framework_hdf, temp_path)
        for file_name in liteos_file_name:
            for i in hdf_utils.template_filename_filtrate(
                    template_path, kernel):
                if i.find(file_name.split(".")[0]) > 0:
                    out_path = os.path.join(adapter_model_path, file_name)
                    self._render(os.path.join(template_path, i),
                                 out_path, data_model)
                    liteos_file_path[file_name] = out_path

        # Modify Kconfig file
        vendor_k = HdfVendorKconfigFile(root, vendor, kernel, path="")
        vendor_k_path = vendor_k.add_module([module, 'Kconfig'])
        liteos_level_config_file_path[module + "_Kconfig"] = vendor_k_path

        # Modify hdf_lite.mk file
        vendor_mk = HdfVendorMkFile(root, vendor)
        vendor_mk_path = vendor_mk.add_module(module)
        liteos_level_config_file_path[module + "_hdf_lite"] = vendor_mk_path

        # Modify Build.gn file
        vendor_gn = HdfVendorBuildFile(root, vendor)
        vendor_gn_path = vendor_gn.add_module(module)
        liteos_level_config_file_path[module + "Build"] = vendor_gn_path

        # Modify config file
        device_info = HdfDeviceInfoHcsFile(
            root, vendor, module, board, driver, path="")
        hcs_file_path = device_info.add_model_hcs_file_config()
        liteos_file_path["devices_info.hcs"] = hcs_file_path

        dot_file_list = hdf_utils.get_dot_configs_path(root, vendor, board)
        template_string = "LOSCFG_DRIVERS_HDF_${module_upper_case}=y\n"
        new_demo_config = Template(template_string).substitute(
            {"module_upper_case": converter.upper_case()})
        for dot_file in dot_file_list:
            file_lines = hdf_utils.read_file_lines(dot_file)
            file_lines[-1] = "{}\n".format(file_lines[-1].strip())
            if new_demo_config != file_lines[-1]:
                file_lines.append(new_demo_config)
                hdf_utils.write_file_lines(dot_file, file_lines)
        liteos_level_config_file_path[module + "_dot_configs"] = dot_file_list
        return liteos_file_path, liteos_level_config_file_path

    def _add_module_handler_linux(self, framework_hdf, adapter_model_path,
                                  data_model, *args_tuple):
        root, vendor, module, driver, board, kernel, _ = args_tuple
        linux_file_path = {}
        linux_level_config_file_path = {}
        linux_file_name = ['Kconfig', 'Makefile']
        temp_path = self.hdf_tool.get_template_path()
        template_path = os.path.join(framework_hdf, temp_path)
        for file_name in linux_file_name:
            for i in hdf_utils.template_filename_filtrate(
                    template_path, kernel):
                if i.find(file_name.split(".")[0]) > 0:
                    out_path = os.path.join(adapter_model_path, file_name)
                    self._render(os.path.join(template_path, i),
                                 out_path, data_model)
                    linux_file_path[file_name] = out_path

        # Modify Kconfig file
        vendor_k = HdfVendorKconfigFile(root, vendor, kernel, path="")
        vendor_k_path = vendor_k.add_module([module, 'Kconfig'])
        linux_level_config_file_path[module + "_Kconfig"] = vendor_k_path

        # Modify Makefile file
        vendor_mk = HdfVendorMakeFile(root, vendor, kernel, path='')
        vendor_mk_path = vendor_mk.add_module(data_model)
        linux_level_config_file_path[module + "_Makefile"] = vendor_mk_path
        # device_info.hcs
        device_info = HdfDeviceInfoHcsFile(
            root, vendor, module, board, driver, path="")
        hcs_file_path = device_info.add_model_hcs_file_config()
        linux_file_path["devices_info.hcs"] = hcs_file_path

        # dot_configs config file
        template_string = "CONFIG_DRIVERS_HDF_${module_upper_case}=y\n"
        new_demo_config = [Template(template_string).substitute(data_model)]
        defconfig_patch = HdfDefconfigAndPatch(
            root, vendor, kernel, board,
            data_model, new_demo_config)

        config_path = defconfig_patch.get_config_config()
        files = []
        patch_list = defconfig_patch.add_module(config_path, files=files, codetype=None)
        config_path = defconfig_patch.get_config_patch()
        files1 = []
        defconfig_list = defconfig_patch.add_module(config_path, files=files1, codetype=None)
        linux_level_config_file_path[module + "_dot_configs"] = \
            list(set(patch_list + defconfig_list))
        return linux_file_path, linux_level_config_file_path

    def _add_module_handler_linux_user(self, framework_hdf, driver_file_path,
                                       *args_tuple):
        linux_file_path = {}
        linux_level_config_file_path = {}
        # create user build.gn files
        root, vendor, module, driver, board, kernel, _ = args_tuple
        relative_path = self.hdf_tool.get_user_adapter_path()
        user_model_path = os.path.join(root, relative_path, module)
        if not os.path.exists(user_model_path):
            os.makedirs(user_model_path)
        user_model_file_path = os.path.join(user_model_path, "BUILD.gn")
        temp_path = self.hdf_tool.get_template_path()
        template_path = os.path.join(framework_hdf, temp_path)
        user_file_path = driver_file_path.split(root)[-1].replace("\\", "/")
        if user_file_path.startswith("/"):
            driver_file_name = "".join(["/", user_file_path.replace("\\", "/")])
        else:
            driver_file_name = "".join(["//", user_file_path.replace("\\", "/")])
        data_model = {
            "model_path": "/".join(["/", relative_path, module]),
            "driver_file_name": driver_file_name,
            "model_name": module,
        }
        for file_name in os.listdir(template_path):
            if file_name.startswith("User_build"):
                user_template_build = os.path.join(template_path, file_name)
                if os.path.exists(user_template_build):
                    self._render(user_template_build,
                                 user_model_file_path, data_model)
                    linux_file_path["BUILD.gn"] = user_model_file_path
        linux_file_path = self.revise_passwd_group_file(
            root_path=root, linux_file_path=linux_file_path, model_name=module)
        # build.gn file add path
        ohos_relative_path = '/'.join(relative_path.split("/")[:-1])
        ohos_path = os.path.join(root, ohos_relative_path, 'BUILD.gn')
        user_build_info = hdf_utils.read_file_lines(ohos_path)
        ohos_template_line = "${model_path}:libhdf_${model_name}_hotplug"
        need_add_line = Template(ohos_template_line).substitute(data_model)
        temp_line = ('      "%s",' % need_add_line) + '\n'
        for index, info in enumerate(user_build_info):
            if info.find("else") > 0 and temp_line not in user_build_info:
                user_build_info.insert(index + 3, temp_line)
                hdf_utils.write_file_lines(ohos_path, user_build_info)
                break
        linux_file_path["adapter_build.gn"] = ohos_path
        # add hcs file
        device_info = HdfDeviceInfoHcsFile(
            root, vendor, module, board, driver, path="")
        hcs_file_path = device_info.add_model_hcs_file_config_user()
        linux_file_path["devices_info.hcs"] = hcs_file_path
        return linux_file_path, linux_level_config_file_path

    def _add_driver(self, *args_tuple):
        root, vendor, module, driver, board, kernel, _ = args_tuple
        drv_converter = hdf_utils.WordsConverter(self.args.driver_name)
        drv_src_dir = hdf_utils.get_drv_src_dir(root, module)

        parent_depth = len(drv_src_dir.split(os.path.sep))
        dir_dict = {}
        for root, dirs, _ in os.walk(drv_src_dir, topdown=True):
            for dir_name in dirs:
                dir_path = os.path.join(root, dir_name)
                if len(dir_path.split(os.path.sep)) == parent_depth + 1:
                    dir_dict[dir_name] = []
                if len(dir_path.split(os.path.sep)) == parent_depth + 2:
                    dir_dict.get(dir_path.split(os.path.sep)[-2]).append(dir_name)

        common_dir_list = dir_dict.get("common")
        if common_dir_list is not None:
            if "src" in common_dir_list:
                driver_file_path = os.path.join(drv_src_dir, "common", "src")
            else:
                driver_file_path = os.path.join(drv_src_dir, "common")
        else:
            driver_file_path = drv_src_dir

        if not os.path.exists(driver_file_path):
            raise HdfToolException(
                '"%s" is path not exist' %
                driver_file_path, CommandErrorCode.TARGET_NOT_EXIST)
        data_model = {
            'driver_lower_case': drv_converter.lower_case(),
            'driver_upper_camel_case': drv_converter.upper_camel_case(),
            'driver_lower_camel_case': drv_converter.lower_camel_case(),
            'driver_upper_case': drv_converter.upper_case()
        }
        self._file_gen_lite('hdf_driver.c.template', driver_file_path,
                            '%s_driver.c' % driver, data_model)
        result_path = os.path.join(driver_file_path, '%s_driver.c' % driver)
        return True, result_path

    def _add_driver_handler(self):
        self.check_arg_raise_if_not_exist("vendor_name")
        self.check_arg_raise_if_not_exist("module_name")
        self.check_arg_raise_if_not_exist("kernel_name")
        self.check_arg_raise_if_not_exist("board_name")
        self.check_arg_raise_if_not_exist("driver_name")
        self.check_arg_raise_if_not_exist("device_name")
        args_tuple = self.get_args()
        root, vendor, module, driver, board, kernel, device = args_tuple
        board_list = self.hdf_tool.get_create_board_type()
        if board not in board_list:
            raise HdfToolException(
                'supported boards name : %s not exits ' % board,
                CommandErrorCode.TARGET_NOT_EXIST)
        if "--runmode" in self.args_original:
            runmode_index = self.args_original.index("--runmode")
            temp_args = self.args_original[:runmode_index] + \
                        self.args_original[runmode_index + 2:]
        else:
            temp_args = self.args_original
        get_board = HdfGetHandler(temp_args)
        temp_board_dict = json.loads(get_board.judge_create_driver_exist())
        if temp_board_dict:
            temp_board_device_dict = temp_board_dict.get(board)
            if temp_board_device_dict is not None and\
                    temp_board_device_dict.get("driver_file_list") is not None:
                driver_name_list = temp_board_device_dict.get("driver_file_list").get(device, [])
                if driver_name_list and driver in driver_name_list:
                    raise HdfToolException(
                        'driver name %s exist in device %s' %
                        (driver, device),
                        CommandErrorCode.TARGET_ALREADY_EXIST)
        framework_hdf = hdf_utils.get_vendor_hdf_dir_framework(root)
        hdf_utils.judge_file_path_exists(framework_hdf)
        framework_drv_root_dir = hdf_utils.get_module_dir(root, module)
        hdf_utils.judge_file_path_exists(framework_drv_root_dir)
        add_driver = HdfAddDriver(args=args_tuple)
        # create driver Source File (.c 、.h)
        resources_path, temp_file_name, config_item, file_path =\
            self._add_create_common(add_driver, args_tuple)
        model_driver_file_path = os.path.join(resources_path, temp_file_name)
        config_file = hdf_utils.read_file(model_driver_file_path)
        config_file_json = json.loads(config_file)
        result_config_file_json = add_driver.driver_create_info_format(
            config_file_json, config_item, file_path)
        hdf_utils.write_config(root_path=root,
                               config_file_json=result_config_file_json,
                               config_name=temp_file_name)
        return json.dumps(config_item, indent=4)

    def _add_create_common(self, add_driver, args_tuple):
        _, _, module, driver, board, _, device = args_tuple
        state, file_list, head_list = add_driver.add_driver(*args_tuple)
        if board == "hispark_taurus":
            file_path = add_driver.add_liteos(file_list, head_list)
        elif board.endswith("linux"):
            file_path = add_driver.add_linux(file_list, head_list)
        elif board.startswith("rk3568_kernel"):
            file_path = add_driver.add_kernel(file_list, head_list)
        elif board.startswith("hispark_taurus_standard_kernel"):
            file_path = add_driver.add_kernel(file_list, head_list)

        config_item = {
            'module_name': module,
            'module_path': file_path,
            'driver_name': "-".join([device, driver]),
            'driver_file_path': file_list,
            'head_file_path': head_list,
            'enabled': "true"
        }
        resources_path = self.hdf_tool.get_resources_file_path()
        config_setting_dict = self.hdf_tool.get_config_setting_info()
        temp_file_name = config_setting_dict["create_driver_file"]
        return resources_path, temp_file_name, config_item, file_path

    def _add_config_handler(self):
        self.check_arg_raise_if_not_exist("module_name")
        self.check_arg_raise_if_not_exist("driver_name")
        self.check_arg_raise_if_not_exist("board_name")
        root, _, module, driver, board, kernel = self.get_args()
        drv_config = HdfDriverConfigFile(root, board, module, driver, kernel)
        drv_config.create_driver()
        return drv_config.get_drv_config_path()

    def revise_passwd_group_file(self, root_path, linux_file_path, model_name):
        group_passwd = OperateGroupPasswd(tool_settings=self.hdf_tool, root_path=root_path)
        # group
        peripheral_name = "_".join([model_name, "user"])
        group_file_path = group_passwd.operate_group(name=peripheral_name)
        # passwd
        passwd_file_path = group_passwd.operate_passwd(name=peripheral_name)
        linux_file_path["passwd"] = passwd_file_path
        linux_file_path["group"] = group_file_path
        return linux_file_path

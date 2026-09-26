# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: Copyright The WasmEdge Authors

wasmedge_add_executable(wasmedgePluginTests
  pluginTest.cpp
)

add_test(wasmedgePluginTests wasmedgePluginTests)

target_link_libraries(wasmedgePluginTests
  PRIVATE
  ${GTEST_BOTH_LIBRARIES}
  wasmedgePlugin
)

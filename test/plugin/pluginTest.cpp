// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright The WasmEdge Authors

#include "plugin/plugin.h"
#include <gtest/gtest.h>

namespace {

using namespace WasmEdge;

// Mock module instance
class MockModuleInstance : public Runtime::Instance::ModuleInstance {
public:
  MockModuleInstance() : Runtime::Instance::ModuleInstance("MockModule") {}
};

// Mock component instance
class MockComponentInstance : public Runtime::Instance::ComponentInstance {
public:
  MockComponentInstance() : Runtime::Instance::ComponentInstance("MockComponent") {}
};

Runtime::Instance::ModuleInstance *createMockModule(const Plugin::PluginModule::ModuleDescriptor *) noexcept {
  return new MockModuleInstance();
}

Runtime::Instance::ComponentInstance *createMockComponent(const Plugin::PluginComponent::ComponentDescriptor *) noexcept {
  return new MockComponentInstance();
}

TEST(PluginTest, RegistrationAndLookup) {
  // Set up mock descriptors
  Plugin::PluginModule::ModuleDescriptor ModDesc{
      "mock_module", "A mock module", createMockModule};

  Plugin::PluginComponent::ComponentDescriptor CompDesc{
      "mock_component", "A mock component", createMockComponent};

  Plugin::Plugin::PluginDescriptor Desc{
      "mock_plugin",
      "A mock plugin for testing",
      Plugin::Plugin::CurrentAPIVersion,
      {1, 0, 0, 0},
      1,
      &ModDesc,
      1,
      &CompDesc,
      nullptr // AddOptions
  };

  // Register the plugin
  EXPECT_TRUE(Plugin::Plugin::registerPlugin(&Desc));

  // Cannot register the same plugin again
  EXPECT_FALSE(Plugin::Plugin::registerPlugin(&Desc));

  // Lookup the plugin
  const Plugin::Plugin *P = Plugin::Plugin::find("mock_plugin");
  ASSERT_NE(P, nullptr);
  EXPECT_STREQ(P->name(), "mock_plugin");
  EXPECT_STREQ(P->description(), "A mock plugin for testing");
  EXPECT_EQ(P->version().Major, 1);
  EXPECT_EQ(P->version().Minor, 0);

  // Lookup module
  const Plugin::PluginModule *Mod = P->findModule("mock_module");
  ASSERT_NE(Mod, nullptr);
  EXPECT_STREQ(Mod->name(), "mock_module");
  EXPECT_STREQ(Mod->description(), "A mock module");

  // Create module instance
  auto ModInst = Mod->create();
  EXPECT_NE(ModInst, nullptr);
  EXPECT_EQ(ModInst->getModuleName(), "MockModule");

  // Lookup component
  const Plugin::PluginComponent *Comp = P->findComponent("mock_component");
  ASSERT_NE(Comp, nullptr);
  EXPECT_STREQ(Comp->name(), "mock_component");
  EXPECT_STREQ(Comp->description(), "A mock component");

  // Create component instance
  auto CompInst = Comp->create();
  EXPECT_NE(CompInst, nullptr);
  EXPECT_EQ(CompInst->getComponentName(), "MockComponent");
}

TEST(PluginTest, APIVersionMismatch) {
  Plugin::Plugin::PluginDescriptor Desc{
      "mock_plugin_invalid_api",
      "A mock plugin with invalid API version",
      Plugin::Plugin::CurrentAPIVersion - 1,
      {1, 0, 0, 0},
      0,
      nullptr,
      0,
      nullptr,
      nullptr};

  // Should fail to register due to version mismatch
  EXPECT_FALSE(Plugin::Plugin::registerPlugin(&Desc));
  EXPECT_EQ(Plugin::Plugin::find("mock_plugin_invalid_api"), nullptr);
}

} // namespace

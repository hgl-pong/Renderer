#pragma once
#include <gtest/gtest.h>
#include "Engine/Window.h"

class EditorWindowTest : public ::testing::Test {
protected:
    EditorWindow window;

    void SetUp() override {
        window.CreateRenderWindow("Test", 800, 600);
    }

    void TearDown() override {
        window.DestroyRenderWindow();
    }
};

TEST_F(EditorWindowTest, IsOpenAfterCreation) {
    EXPECT_TRUE(window.IsOpen());
}

TEST_F(EditorWindowTest, SetVSync) {
    window.SetVSync(true);
    EXPECT_TRUE(window.IsVSync());
}

TEST_F(EditorWindowTest, SetFullscreen) {
    window.SetFullscreen(false);
    EXPECT_FALSE(window.IsFullscreen());
}

TEST_F(EditorWindowTest, SetVisible) {
    window.SetVisible(true);
    EXPECT_TRUE(window.IsVisible());
}

TEST_F(EditorWindowTest, SetMouseCursorVisible) {
    window.SetMouseCursorVisible(true);
    EXPECT_TRUE(window.IsMouseCursorVisible());
}

TEST_F(EditorWindowTest, SetPosition) {
    window.SetPosition(100, 100);
    Vector2f position = window.GetPosition();
    EXPECT_FLOAT_EQ(position.x(), 100);
    EXPECT_FLOAT_EQ(position.y(), 100);
}

TEST_F(EditorWindowTest, SetIcon) {
    // 此测试依赖于实现细节，如果SetIcon不改变可测试的状态，则可能无法直接测试
}

TEST_F(EditorWindowTest, SetClearColorAndClear) {
    // 此测试可能需要视觉验证或特定的渲染系统mock来验证ClearColor是否被正确设置和使用
}

TEST_F(EditorWindowTest, Display) {
    // 测试Display功能可能需要集成测试或特定的渲染系统mock
}

TEST_F(EditorWindowTest, GetWidthHeight) {
    EXPECT_EQ(window.GetWidth(), 800);
    EXPECT_EQ(window.GetHeight(), 600);
}

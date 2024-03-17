#pragma once
#include <string>
#include <vector>
#include <deque>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <memory>
#include <algorithm>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <chrono>

// Third-party
#include <vulkan/vulkan.h>

#include <Eigen/Eigen>
#include <SDL2/SDL.h>

#include "imgui.h"
#include "RenderSystem/imgui/imgui_impl_sdl2.h"
#include "RenderSystem/imgui/imgui_impl_sdlrenderer2.h"

// Custom
#include "Types.h"
#include "Functions.h"
#include "LogUtils.h"
#include "Macro.h"
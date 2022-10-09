#pragma once

#include <map>
#include <mutex>
#include <shared_mutex>

#include <Windows.h>

#include "Mod.hpp"
#include "uevr/API.h"

namespace uevr {
extern UEVR_RendererData g_renderer_data;
}

class PluginLoader : public Mod {
public:
    static std::shared_ptr<PluginLoader>& get();

    // This is called prior to most UEVR initialization so that all plugins are at least **loaded** early on. UEVR_ramework plugin
    // specifics (`uevr_plugin_initialize`) are still delayed until UEVR_ramework is fully setup.
    void early_init();

    std::string_view get_name() const override { return "PluginLoader"; }
    std::optional<std::string> on_initialize() override;
    void on_draw_ui() override;

    void on_present();
    void on_device_reset() override;
    bool on_message(HWND wnd, UINT message, WPARAM w_param, LPARAM l_param) override;
    
    void on_pre_engine_tick(sdk::UGameEngine* engine, float delta) override;
    void on_post_engine_tick(sdk::UGameEngine* engine, float delta) override;
    void on_pre_slate_draw_window(void* renderer, void* command_list, sdk::FViewportInfo* viewport_info) override;
    void on_post_slate_draw_window(void* renderer, void* command_list, sdk::FViewportInfo* viewport_info) override;
    void on_pre_calculate_stereo_view_offset(void* stereo_device, const int32_t view_index, Rotator<float>* view_rotation, 
                                             const float world_to_meters, Vector3f* view_location, bool is_double) override;
    void on_post_calculate_stereo_view_offset(void* stereo_device, const int32_t view_index, Rotator<float>* view_rotation, 
                                              const float world_to_meters, Vector3f* view_location, bool is_double) override;
    
public:
    using UEVR_OnPresentCb = std::function<std::remove_pointer<::UEVR_OnPresentCb>::type>;
    using UEVR_OnDeviceResetCb = std::function<std::remove_pointer<::UEVR_OnDeviceResetCb>::type>;
    using UEVR_OnMessageCb = std::function<std::remove_pointer<::UEVR_OnMessageCb>::type>;

    /* Engine specific callbacks */
    using UEVR_Engine_TickCb = std::function<std::remove_pointer<::UEVR_Engine_TickCb>::type>;
    using UEVR_Slate_DrawWindow_RenderThreadCb = std::function<std::remove_pointer<::UEVR_Slate_DrawWindow_RenderThreadCb>::type>;
    using UEVR_Stereo_CalculateStereoViewOffsetCb = std::function<std::remove_pointer<::UEVR_Stereo_CalculateStereoViewOffsetCb>::type>;

    bool add_on_present(UEVR_OnPresentCb cb);
    bool add_on_device_reset(UEVR_OnDeviceResetCb cb);
    bool add_on_message(UEVR_OnMessageCb cb);

    bool add_on_pre_engine_tick(UEVR_Engine_TickCb cb);
    bool add_on_post_engine_tick(UEVR_Engine_TickCb cb);
    bool add_on_pre_slate_draw_window_render_thread(UEVR_Slate_DrawWindow_RenderThreadCb cb);
    bool add_on_post_slate_draw_window_render_thread(UEVR_Slate_DrawWindow_RenderThreadCb cb);
    bool add_on_pre_calculate_stereo_view_offset(UEVR_Stereo_CalculateStereoViewOffsetCb cb);
    bool add_on_post_calculate_stereo_view_offset(UEVR_Stereo_CalculateStereoViewOffsetCb cb);

private:
    std::shared_mutex m_api_cb_mtx;
    std::vector<PluginLoader::UEVR_OnPresentCb> m_on_present_cbs{};
    std::vector<PluginLoader::UEVR_OnDeviceResetCb> m_on_device_reset_cbs{};
    std::vector<PluginLoader::UEVR_OnMessageCb> m_on_message_cbs{};

    std::vector<PluginLoader::UEVR_Engine_TickCb> m_on_pre_engine_tick_cbs{};
    std::vector<PluginLoader::UEVR_Engine_TickCb> m_on_post_engine_tick_cbs{};
    std::vector<PluginLoader::UEVR_Slate_DrawWindow_RenderThreadCb> m_on_pre_slate_draw_window_render_thread_cbs{};
    std::vector<PluginLoader::UEVR_Slate_DrawWindow_RenderThreadCb> m_on_post_slate_draw_window_render_thread_cbs{};
    std::vector<PluginLoader::UEVR_Stereo_CalculateStereoViewOffsetCb> m_on_pre_calculate_stereo_view_offset_cbs{};
    std::vector<PluginLoader::UEVR_Stereo_CalculateStereoViewOffsetCb> m_on_post_calculate_stereo_view_offset_cbs{};

private:
    std::mutex m_mux{};
    std::map<std::string, HMODULE> m_plugins{};
    std::map<std::string, std::string> m_plugin_load_errors{};
    std::map<std::string, std::string> m_plugin_load_warnings{};
};
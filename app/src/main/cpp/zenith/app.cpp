#include <app.h>
#include <java/device_res.h>

static zenith::java::JvmManager deviceRes{};

namespace zenith {
    std::unique_ptr<CoreApplication> zenithApp;

    CoreApplication::CoreApplication() :
        virtualBlocks(std::make_shared<console::GlobalMemory>()),
        simulatedDevices(std::make_shared<console::VirtualDevices>(virtualBlocks)) {
        // Kickstart the user readable log system also called as, PalePaper
        userLog = std::make_shared<PalePaper>();

        auto osState{deviceRes.getOSState()};
        osState.lock()->syncSettings();
    }
}

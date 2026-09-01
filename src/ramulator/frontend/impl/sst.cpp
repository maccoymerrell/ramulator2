// SST frontend.
//
// The SST memHierarchy ramulator2 backend drives the model by ticking the
// *frontend*, where ChampSim ticks the memory system itself -- so this is the
// External frontend with a tick that advances the memory system, and nothing
// else. It exists here rather than being taken from sst-elements unchanged
// because this fork's receive_external_requests carries a size, which upstream's
// does not.

#include "ramulator/base/param.h"
#include "ramulator/frontend/i_frontend.h"

namespace Ramulator {

class SSTFrontEnd : public IFrontEnd, public Implementation {
  RAMULATOR_REGISTER_IMPLEMENTATION(IFrontEnd, SSTFrontEnd, "SST")

 public:
  void init() override {
    RAMULATOR_PARSE_PARAM(m_clock_ratio, unsigned int, "clock_ratio").required();
  }

  void tick() override { m_memory_system->tick(); }

  // SST decides when the simulation ends, not the memory model.
  bool is_finished() override { return true; }

  bool receive_external_requests(int req_type_id, Addr_t addr, int source_id,
                                 std::function<void(Request&)> callback,
                                 int size_bytes) override {
    return receive_external_requests(req_type_id, addr, source_id, -1,
                                     std::move(callback), size_bytes);
  }

  bool receive_external_requests(int req_type_id, Addr_t addr, int source_id,
                                 int ingress_id,
                                 std::function<void(Request&)> callback,
                                 int size_bytes) override {
    Request req(addr, req_type_id, source_id, std::move(callback));
    req.ingress_id = ingress_id;
    req.size_bytes = size_bytes;
    return m_memory_system->send(req);
  }
};

}  // namespace Ramulator

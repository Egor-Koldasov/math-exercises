#include <csignal>

volatile std::sig_atomic_t shouldStop = 0;

namespace {
void handleShutdownSignal(int) { shouldStop = 1; }

} // namespace

void listenShutdownSignal() {
  std::signal(SIGINT, handleShutdownSignal);
  std::signal(SIGTERM, handleShutdownSignal);
}

#pragma once
#include <csignal>

extern volatile std::sig_atomic_t shouldStop;
void listenShutdownSignal();

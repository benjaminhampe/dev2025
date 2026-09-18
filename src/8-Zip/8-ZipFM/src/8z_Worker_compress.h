#pragma once
#include <FL/Fl_Widget.H>

namespace EightZip {
namespace worker {

void workerThread_CommonScanInit();

// ---------------- callbacks ----------------
void compress_pause_cb(Fl_Widget*, void*);
void compress_cancel_cb(Fl_Widget*, void*);
void compress_finish_cb(void*);

} // end namespace worker.
} // end namespace EightZip.


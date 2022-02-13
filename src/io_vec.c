#include <smolrtsp/io_vec.h>

size_t SmolRTSP_IoVecSlice_len(SmolRTSP_IoVecSlice self) {
    size_t result = 0;
    for (size_t i = 0; i < self.len; i++) {
        result += self.ptr[i].iov_len;
    }

    return result;
}

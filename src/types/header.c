#include <smolrtsp/types/header.h>

#include "crlf.h"
#include "parsing.h"

#include <assert.h>
#include <string.h>

ssize_t SmolRTSP_Header_serialize(SmolRTSP_Header self, SmolRTSP_Writer w) {
    assert(w.self && w.vptr);

    return SMOLRTSP_WRITE_SLICES(
        w, {
               self.key,
               CharSlice99_from_str(": "),
               self.value,
               SMOLRTSP_CRLF,
           });
}

SmolRTSP_ParseResult SmolRTSP_Header_parse(SmolRTSP_Header *restrict self, CharSlice99 input) {
    assert(self);

    const CharSlice99 backup = input;

    SmolRTSP_Header header;

    MATCH(smolrtsp_match_whitespaces(input));
    header.key = input;
    MATCH(smolrtsp_match_header_name(input));
    header.key = CharSlice99_from_ptrdiff(header.key.ptr, input.ptr);

    MATCH(smolrtsp_match_whitespaces(input));
    MATCH(smolrtsp_match_char(input, ':'));
    MATCH(smolrtsp_match_whitespaces(input));

    header.value = input;
    MATCH(smolrtsp_match_until_crlf(input));
    header.value = CharSlice99_from_ptrdiff(header.value.ptr, input.ptr - strlen("\r\n"));

    *self = header;

    return SmolRTSP_ParseResult_complete(input.ptr - backup.ptr);
}

bool SmolRTSP_Header_eq(SmolRTSP_Header lhs, SmolRTSP_Header rhs) {
    return CharSlice99_primitive_eq(lhs.key, rhs.key) &&
           CharSlice99_primitive_eq(lhs.value, rhs.value);
}

void SmolRTSP_Header_dbg_to_file(SmolRTSP_Header self, FILE *stream) {
    assert(stream);

    fprintf(
        stream, "'%.*s': '%.*s'\n", (int)CharSlice99_size(self.key), (const char *)self.key.ptr,
        (int)CharSlice99_size(self.value), (const char *)self.value.ptr);
}

void SmolRTSP_Header_dbg(SmolRTSP_Header self) {
    SmolRTSP_Header_dbg_to_file(self, stderr);
}
